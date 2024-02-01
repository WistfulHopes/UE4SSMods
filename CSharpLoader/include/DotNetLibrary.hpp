#pragma once

#include <Common.hpp>
#include <cstdint>
#include <CoreCLR.hpp>
#include <DynamicOutput/OutputDevice.hpp>
#include <UnrealCoreStructs.hpp>
#include <Unreal/Quat.hpp>
#include <Unreal/Transform.hpp>
#include <Unreal/AActor.hpp>
#include <unordered_map>

namespace RC::Unreal
{
    class FOutputDevice;
    class UStruct;
    class UEnum;
    struct FWeakObjectPtr;
    struct FSoftObjectPtr;
    class UInterface;
}

namespace PLH
{
    class x64Detour;
}

using namespace RC::Unreal;

#define CSHARPLOADER_API __declspec(dllexport)
namespace RC::DotNetLibrary
{
    enum struct CallbackType : int32_t
    {
        ActorOverlapDelegate,
        ActorHitDelegate,
        ActorCursorDelegate,
        ActorKeyDelegate,
        ComponentOverlapDelegate,
        ComponentHitDelegate,
        ComponentCursorDelegate,
        ComponentKeyDelegate,
        CharacterLandedDelegate
    };

    enum struct ArgumentType : int32_t
    {
        None,
        Single,
        Integer,
        Pointer,
        Callback
    };

    enum struct CommandType : int32_t
    {
        Initialize = 1,
        LoadAssemblies = 2,
        UnloadAssemblies = 3,
        Find = 4,
        Execute = 5
    };

    enum struct PropertyType : int32_t
    {
        ObjectProperty,
        ClassProperty,
        Int8Property,
        Int16Property,
        IntProperty,
        Int64Property,
        ByteProperty,
        UInt16Property,
        UInt32Property,
        UInt64Property,
        StructProperty,
        ArrayProperty,
        FloatProperty,
        DoubleProperty,
        BoolProperty,
        EnumProperty,
        WeakObjectProperty,
        NameProperty,
        TextProperty,
        StrProperty,
        SoftClassProperty,
        InterfaceProperty,
        Invalid,
    };

    enum
    {
        StartMod,
        StopMod,
        ProgramStart,
        UnrealInit,
        Update,
    };

    struct StaticState
    {
        static inline std::unordered_map<int32_t, PropertyType> m_property_type_map;
    };

    struct Callback
    {
        void** Parameters;
        CallbackType Type;

        FORCEINLINE Callback(void** Parameters, CallbackType Type)
        {
            this->Parameters = Parameters;
            this->Type = Type;
        }
    };

    struct Argument
    {
        union {
            float Single;
            uint32_t Integer;
            void* Pointer;
            Callback Callback;
        };
        ArgumentType Type;

        FORCEINLINE Argument(float Value)
        {
            this->Single = Value;
            this->Type = ArgumentType::Single;
        }

        FORCEINLINE Argument(uint32_t Value)
        {
            this->Integer = Value;
            this->Type = ArgumentType::Integer;
        }

        FORCEINLINE Argument(void* Value)
        {
            this->Pointer = Value;
            this->Type = !Value ? ArgumentType::None : ArgumentType::Pointer;
        }

        FORCEINLINE Argument(DotNetLibrary::Callback Value)
        {
            this->Callback = Value;
            this->Type = ArgumentType::Callback;
        }
    };

    struct Command
    {
        union {
            struct
            {
                void* Buffer;
            };
            struct
            {
                char* Method;
                int32_t Optional;
            };
            struct
            {
                void* Function;
                Argument Value;
            };
        };
        CommandType Type;

        FORCEINLINE Command(void* const Functions[2])
        {
            this->Buffer = (void*)Functions;
            this->Type = CommandType::Initialize;
        }

        FORCEINLINE Command(CommandType Type)
        {
            this->Type = Type;
        }

        FORCEINLINE Command(const char* Method, bool Optional)
        {
            this->Method = (char*)Method;
            this->Optional = Optional;
            this->Type = CommandType::Find;
        }

        FORCEINLINE Command(void* Function)
        {
            this->Function = Function;
            this->Value = nullptr;
            this->Type = CommandType::Execute;
        }

        FORCEINLINE Command(void* Function, Argument Value)
        {
            this->Function = Function;
            this->Value = Value;
            this->Type = CommandType::Execute;
        }
    };

    static_assert(sizeof(Callback) == 16, "Invalid size of the [Callback] structure");
    static_assert(sizeof(Argument) == 24, "Invalid size of the [Argument] structure");
    static_assert(sizeof(Command) == 40, "Invalid size of the [Command] structure");

    static void* (*ManagedCommand)(Command);

    class CSHARPLOADER_API Runtime
    {
        std::wstring m_runtime_directory;

        static inline CoreCLR* CLR;

    public:
        Runtime(std::wstring runtime_directory) : m_runtime_directory(runtime_directory) {}

        static auto log(LogLevel::LogLevel Level, const char* Message);
        auto initialize() -> void;
        auto load_assemblies() -> void;
        auto unload_assemblies() -> void;
        auto start_mods() -> void;
        auto stop_mods() -> void;
        auto fire_program_start() -> void;
        auto fire_unreal_init() -> void;
        auto fire_update() -> void;
    };

    namespace Shared
    {
        static void* RuntimeFunctions[1];
        static void* Events[128];
    }
    
    namespace Framework
    {
        struct Vector3
        {
            double X;
            double Y;
            double Z;

            FORCEINLINE Vector3(FVector Value)
            {
                this->X = Value.X();
                this->Y = Value.Y();
                this->Z = Value.Z();
            }

            FORCEINLINE operator Unreal::FVector() const
            {
                return FVector(X, Y, Z);
            }
        };

        struct Quaternion
        {
            double X;
            double Y;
            double Z;
            double W;

            FORCEINLINE Quaternion(FQuat Value)
            {
                this->X = Value.GetX();
                this->Y = Value.GetY();
                this->Z = Value.GetZ();
                this->W = Value.GetW();
            }

            FORCEINLINE operator Unreal::FQuat() const
            {
                return FQuat(X, Y, Z, W);
            }
        };

        struct Transform
        {
            Vector3 Location;
            Quaternion Rotation;
            Vector3 Scale;

            FORCEINLINE Transform(Unreal::FTransform& Value) : Location(Value.GetTranslation()), Rotation(Value.GetRotation()), Scale(Value.GetScale3D()) { }

            FORCEINLINE operator Unreal::FTransform() const
            {
                return Unreal::FTransform(Rotation, Location, Scale);
            }
        };

        struct UnArray
        {
            void* Data;
            uint32 Length;
            PropertyType Type;
        };

        class CSHARPLOADER_API Debug
        {
        public:
            static void Log(LogLevel::LogLevel Level, const char* Message);
        };

        class CSHARPLOADER_API Hooking
        {
        public:
            static intptr_t SigScan(const char* Signature);
            static PLH::x64Detour* Hook(uint64_t fnAddress, uint64_t fnCallback, uint64_t* userTrampVar);
            static CallbackId HookUFunctionPre(const char* Name, const UnrealScriptFunctionCallable& PreCallback, void* CustomData);
            static CallbackId HookUFunctionPost(const char* Name, const UnrealScriptFunctionCallable& PostCallback, void* CustomData);
            static void Unhook(PLH::x64Detour* Hook);
            static bool UnhookUFunction(const char* Name, CallbackId CallbackId);
        };


        class CSHARPLOADER_API Object
        {
        public:
            static bool IsValid(UObject* Object);
            static void Invoke(UObject* Object, UFunction* Function, void* Params);
            static UObject* Find(const char* Name);
            static void GetFullName(UObject* Object, char* Name);
            static void GetName(UObject* Object, char* Name);
            static void GetClass(UObject* Object, UClass** Class);
            static void GetOuter(UObject* Object, UObject** Outer);
            static bool IsAnyClass(UObject* Object);
            static void GetWorld(UObject* Object, UWorld** World);
            static bool IsA(UObject* Object, UClass* Class);
            static bool HasAllFlags(UObject* Object, EObjectFlags FlagsToCheck);
            static bool HasAnyFlags(UObject* Object, EObjectFlags FlagsToCheck);
            static bool HasAnyInternalFlags(UObject* Object, EInternalObjectFlags InternalFlagsToCheck);
            static bool GetObjectReference(UObject* Object, const char* Name, UObject** Value);
            static bool GetFunction(UObject* Object, const char* Name, UFunction** Value);
            static bool GetBool(UObject* Object, const char* Name, bool* Value);
            static bool GetByte(UObject* Object, const char* Name, uint8* Value);
            static bool GetShort(UObject* Object, const char* Name, int16* Value);
            static bool GetInt(UObject* Object, const char* Name, int32* Value);
            static bool GetLong(UObject* Object, const char* Name, int64* Value);
            static bool GetUShort(UObject* Object, const char* Name, uint16* Value);
            static bool GetUInt(UObject* Object, const char* Name, uint32* Value);
            static bool GetULong(UObject* Object, const char* Name, uint64* Value);
            static bool GetStruct(UObject* Object, const char* Name, UStruct** Value);
            static bool GetArray(UObject* Object, const char* Name, UnArray* Value);
            static bool GetFloat(UObject* Object, const char* Name, float* Value);
            static bool GetDouble(UObject* Object, const char* Name, double* Value);
            static bool GetEnum(UObject* Object, const char* Name, int64* Value);
            static bool GetWeakObject(UObject* Object, const char* Name, FWeakObjectPtr* Value);
            static bool GetString(UObject* Object, const char* Name, char* Value);
            static bool GetText(UObject* Object, const char* Name, char* Value);
            static bool GetSoftClass(UObject* Object, const char* Name, FSoftObjectPtr* Value);
            static bool SetObjectReference(UObject* Object, const char* Name, UObject* Value);
            static bool SetBool(UObject* Object, const char* Name, bool Value);
            static bool SetByte(UObject* Object, const char* Name, uint8 Value);
            static bool SetShort(UObject* Object, const char* Name, int16 Value);
            static bool SetInt(UObject* Object, const char* Name, int32 Value);
            static bool SetLong(UObject* Object, const char* Name, int64 Value);
            static bool SetUShort(UObject* Object, const char* Name, uint16 Value);
            static bool SetUInt(UObject* Object, const char* Name, uint32 Value);
            static bool SetULong(UObject* Object, const char* Name, uint64 Value);
            static bool SetStruct(UObject* Object, const char* Name, UStruct* Value);
            static bool SetArray(UObject* Object, const char* Name, UnArray Value);
            static bool SetFloat(UObject* Object, const char* Name, float Value);
            static bool SetDouble(UObject* Object, const char* Name, double Value);
            static bool SetEnum(UObject* Object, const char* Name, int32 Value);
            static bool SetString(UObject* Object, const char* Name, const char* Value);
            static bool SetText(UObject* Object, const char* Name, const char* Value);
            static bool SetSoftClass(UObject* Object, const char* Name, FSoftObjectPtr Value);
        };

        class CSHARPLOADER_API Struct : public Object
        {
            static UClass* GetSuperStruct(UStruct* Struct);
            static void ForEachFunction(UStruct* Struct, void (*Callback)(UFunction* Function));
            static void ForEachProperty(UStruct* Struct, void (*Callback)(FProperty* Property));
        };

        class CSHARPLOADER_API Class : public Struct
        {
            static void GetCDO(UClass* Class, UObject** CDO);
            static bool IsChildOf(UClass* Class, UClass* Parent);
        };

        class CSHARPLOADER_API UnEnum : public Object
        {
            static void GetNameByValue(UEnum* Enum, int Value, char* Name);
            static void ForEachName(UEnum* Enum, void (*Callback)(const char* Name, int Value));
            static void GetEnumNameByIndex(UEnum* Enum, int Index, char* Name, int64* Value);
            static void InsertIntoNames(UEnum* Enum, char* Name, int Index, int64 Value, bool ShiftValues = true);
            static void EditNameAt(UEnum* Enum, int Index, char* Name);
            static void EditValueAt(UEnum* Enum, int Index, int64 Value);
            static void RemoveFromNamesAt(UEnum* Enum, int Index, int Count = 1, bool AllowShrinking = true);
        };
    } 
}