#include <polyhook2/Detour/x64Detour.hpp>
#include "DotNetLibrary.hpp"

#include <stack>
#include <UnrealDef.hpp>
#include <Unreal/UScriptStruct.hpp>
#include <File/Macros.hpp>
#include <DynamicOutput/DynamicOutput.hpp>
#include <Helpers/String.hpp>
#include <Helpers/Casting.hpp>
#include <SigScanner/SinglePassSigScanner.hpp>

#include "ExceptionHandling.hpp"

namespace RC::DotNetLibrary
{
    auto Runtime::log(LogLevel::LogLevel Level, const char* Message)
    {
        Framework::Debug::Log(Level, Message);
    }
    
    void Runtime::add_unreal_init_callback(void (*Callback)())
    {
        unreal_init_callbacks.push_back(Callback);
    }
    
    void Runtime::add_update_callback(void (*Callback)())
    {
        update_callbacks.push_back(Callback);
    }
    
    auto Runtime::initialize() -> void
    {
        int success = 0;
        CLR = new CoreCLR(&success);

        if (!success)
        {
            Output::send<LogLevel::Error>(STR("Failed to initialize CoreCLR\n"));
            CLR = nullptr;
            return;
        }

        if (!std::filesystem::exists(m_runtime_directory))
        {
            Output::send<LogLevel::Error>(STR("Could not find UE4SSDotNetRuntime\n"));
            CLR = nullptr;
            return;
        }

        const string_t runtime_config_path = m_runtime_directory + L"\\UE4SSDotNetRuntime.runtimeconfig.json";

        if (!std::filesystem::exists(runtime_config_path))
        {
            Output::send<LogLevel::Error>(STR("The runtime configuration does not exist for UE4SSDotNetRuntime\n"));
            CLR = nullptr;
            return;
        }

        if (!CLR->load_runtime(runtime_config_path))
        {
            Output::send<LogLevel::Warning>(STR("Failed to load .NET Core Runtime.\n"
                                              "If using Reloaded II, disregard this message."));
            CLR = nullptr;
            return;
        }

        const auto runtime_assembly_path = m_runtime_directory + L"\\UE4SSDotNetRuntime.dll";
        const auto runtime_type_name = L"UE4SSDotNetRuntime.Runtime.Core, UE4SSDotNetRuntime";
        const auto runtime_method_name = L"ManagedCommand";

        if (!CLR->load_assembly_and_get_function_pointer(runtime_assembly_path.c_str(),
                                                        runtime_type_name,
                                                        runtime_method_name,
                                                        UNMANAGEDCALLERSONLY_METHOD,
                                                        nullptr,
                                                        reinterpret_cast<void**>(&ManagedCommand)))
        {
            Output::send<LogLevel::Error>(STR("Host runtime assembly loading failed\n"));
            CLR = nullptr;
            return;
        }

        if (ManagedCommand)
        {
            Shared::RuntimeFunctions[0] = (void*)&Runtime::log;

            constexpr void* functions[2] = {Shared::RuntimeFunctions, Shared::Events};

            if (reinterpret_cast<intptr_t>(ManagedCommand(Command(functions))) == 0xF)
            {
                Output::send(STR("Host runtime assembly initialized successfully!\n"));
                load_assemblies();
            }
            else
            {
                Output::send<LogLevel::Error>(STR("Host runtime assembly initialization failed!\n"));
                CLR = nullptr;
                return;
            }

            return;
        }

        CLR = nullptr;
    }

    auto Runtime::load_assemblies() -> void
    {
        if (!CLR) return;
        ManagedCommand(Command(CommandType::LoadAssemblies));
        start_mods();
    }

    auto Runtime::unload_assemblies() -> void
    {
        if (!CLR) return;
        stop_mods();
        ManagedCommand(Command(CommandType::UnloadAssemblies));
    }

    auto Runtime::start_mods() -> void
    {
        if (Shared::Events[StartMod]) ManagedCommand(Command(Shared::Events[StartMod]));
    }

    auto Runtime::stop_mods() -> void
    {
        if (Shared::Events[StopMod]) ManagedCommand(Command(Shared::Events[StopMod]));
    }

    auto Runtime::fire_program_start() -> void
    {
        if (Shared::Events[ProgramStart]) ManagedCommand(Command(Shared::Events[ProgramStart]));
    }
    
    static auto script_hook([[maybe_unused]] Unreal::UObject* Context, Unreal::FFrame& Stack, [[maybe_unused]] void* RESULT_DECL) -> void
    {
        auto execute_hook = [&](std::unordered_map<StringType, Framework::CSharpCallbackData>& callback_container, bool precise_name_match) {
            if (callback_container.empty())
            {
                return;
            }
            if (auto it = callback_container.find(precise_name_match ? Stack.Node()->GetFullName() : Stack.Node()->GetName()); it != callback_container.end())
            {
                const auto& callback_data = it->second;
                for (const auto& [callback, index] : callback_data.registry_indexes)
                {
                    callback(Context, Stack.Locals(), Stack.OutParms(), RESULT_DECL);
                }
            }
        };

        TRY([&] {
            execute_hook(Framework::m_script_hook_callbacks, true);
        });
    }

    auto Runtime::fire_unreal_init() -> void
    {
        if (Shared::Events[UnrealInit]) ManagedCommand(Command(Shared::Events[UnrealInit]));

        for (const auto callback : unreal_init_callbacks) callback();
        if (Unreal::UObject::ProcessLocalScriptFunctionInternal.is_ready() && Unreal::Version::IsAtLeast(4, 22))
        {
            Output::send(STR("Enabling custom events\n"));
            Unreal::Hook::RegisterProcessLocalScriptFunctionPostCallback(script_hook);
        }
        else if (Unreal::UObject::ProcessInternalInternal.is_ready() && Unreal::Version::IsBelow(4, 22))
        {
            Output::send(STR("Enabling custom events\n"));
            Unreal::Hook::RegisterProcessInternalPostCallback(script_hook);
        }
    }

    auto Runtime::fire_update() -> void
    {
        if (Shared::Events[Update]) ManagedCommand(Command(Shared::Events[Update]));

        for (const auto callback : update_callbacks) callback();
    }

    namespace Framework
    {
        #define CLR_GET_PROPERTY_VALUE(PropertyType, Type, Object, Name, Value)                                                                                 \
            PropertyType* prop = static_cast<PropertyType*>(Object->GetPropertyByNameInChain(to_wstring(Name).c_str()));                                        \
            if (!prop) return false;                                                                                                                            \
                                                                                                                                                                \
            *Value = *prop->ContainerPtrToValuePtr<Type>(Object);                                                                                               \
            return true;

        #define CLR_SET_PROPERTY_VALUE(PropertyType, Type, Object, Name, Value)                                                                                 \
            PropertyType* prop = static_cast<PropertyType*>(Object->GetPropertyByNameInChain(to_wstring(Name).c_str()));                                        \
            if (!prop) return false;                                                                                                                            \
                                                                                                                                                                \
            *prop->ContainerPtrToValuePtr<Type>(Object) = Value;                                                                                                \
        return true;

        void Hooking::CSharpUnrealScriptFunctionHookPre(UnrealScriptFunctionCallableContext context, void* custom_data)
        {
            auto& csharp_data = *static_cast<CSharpUnrealScriptFunctionData*>(custom_data);

            if (csharp_data.callback_ref == nullptr) return;
            
            csharp_data.callback_ref(context.Context, context.TheStack.Locals(), context.TheStack.OutParms(), nullptr);
        }

        void Hooking::CSharpUnrealScriptFunctionHookPost(UnrealScriptFunctionCallableContext context, void* custom_data)
        {
            auto& csharp_data = *static_cast<CSharpUnrealScriptFunctionData*>(custom_data);

            if (csharp_data.post_callback_ref == nullptr) return;

            csharp_data.post_callback_ref(context.Context, context.TheStack.Locals(), context.TheStack.OutParms(), context.RESULT_DECL);
        }
        
        intptr_t Hooking::SigScan(const char* Signature)
        {
            intptr_t address = 0;
            SignatureContainer signature_container{
                    {{Signature}},
                    [&](const SignatureContainer& self) {
                        address = reinterpret_cast<intptr_t>(self.get_match_address());
                        return true;
                    },
                    [](SignatureContainer& self) {},
            };

            SinglePassScanner::SignatureContainerMap signature_containers = {
                    {ScanTarget::MainExe, {signature_container}},
            };

            SinglePassScanner::start_scan(signature_containers);

            return address;
        }

        PLH::x64Detour* Hooking::Hook(const uint64_t fnAddress, const uint64_t fnCallback, uint64_t* userTrampVar)
        {
            auto hook = new PLH::x64Detour(fnAddress, fnCallback, userTrampVar);
            hook->hook();
            return hook;
        }

        CallbackIds Hooking::HookUFunction(UFunction* function, UFunctionCallback pre_callback, UFunctionCallback post_callback)
        {
            int32_t generic_pre_id{};
            int32_t generic_post_id{};

            const auto func_ptr = function->GetFunc();

            if (func_ptr && func_ptr != Unreal::UObject::ProcessInternalInternal.get_function_address() &&
                function->HasAnyFunctionFlags(FUNC_Native))
            {
                const auto& custom_data = g_hooked_script_function_data.emplace_back(std::make_unique<CSharpUnrealScriptFunctionData>(
                    CSharpUnrealScriptFunctionData{0, 0, function, pre_callback, post_callback}));
                CallbackId pre_id;
                CallbackId post_id;
                if (pre_callback)
                    pre_id = function->RegisterPreHook(&CSharpUnrealScriptFunctionHookPre, custom_data.get());
                else
                    pre_id = 0;
                if (post_callback)
                    post_id = function->RegisterPostHook(&CSharpUnrealScriptFunctionHookPost, custom_data.get());
                else
                    post_id = 0;
                custom_data->pre_callback_id = pre_id;
                custom_data->post_callback_id = post_id;
                m_generic_hook_id_to_native_hook_id.emplace(++m_last_generic_hook_id, pre_id);
                generic_pre_id = m_last_generic_hook_id;
                m_generic_hook_id_to_native_hook_id.emplace(++m_last_generic_hook_id, post_id);
                generic_post_id = m_last_generic_hook_id;
                Output::send<LogLevel::Verbose>(STR("[RegisterHook] Registered native hook ({}, {}) for {}\n"),
                                    generic_pre_id,
                                    generic_post_id,
                                    function->GetFullName());
            }
            else if (func_ptr && func_ptr == Unreal::UObject::ProcessInternalInternal.get_function_address() &&
                !function->HasAnyFunctionFlags(FUNC_Native))
            {
                ++m_last_generic_hook_id;
                auto [callback_data, _] = m_script_hook_callbacks.emplace(function->GetFullName(), CSharpCallbackData{nullptr, {}});
                callback_data->second.registry_indexes.emplace_back(CSharpCallbackData::RegistryIndex{pre_callback, m_last_generic_hook_id});

                generic_pre_id = m_last_generic_hook_id;
                generic_post_id = m_last_generic_hook_id;
                Output::send<LogLevel::Verbose>(STR("[RegisterHook] Registered script hook ({}, {}) for {}\n"),
                                                generic_pre_id,
                                                generic_post_id,
                                                function->GetFullName());
            }

            return CallbackIds { generic_pre_id, generic_post_id };
        }

        void Hooking::Unhook(PLH::x64Detour* Hook)
        {
            if (Hook) Hook->unHook();
        }

        void Hooking::UnhookUFunction(UFunction* function, CallbackIds callback_ids)
        {
            if (const auto callback_data_it = m_script_hook_callbacks.find(function->GetFullName());
                callback_data_it != m_script_hook_callbacks.end())
            {
                m_script_hook_callbacks.erase(callback_data_it);
            }
            else
            {
                function->UnregisterHook(callback_ids.pre_id);
                function->UnregisterHook(callback_ids.post_id);
            }
        }

        void Debug::Log(LogLevel::LogLevel Level, const char* Message)
        {
            if (Level == LogLevel::Default)
            {
                Output::send<LogLevel::Default>(to_wstring(Message) + L"\n");
            }
            else if (Level == LogLevel::Normal)
            {
                Output::send<LogLevel::Normal>(to_wstring(Message) + L"\n");
            }
            else if (Level == LogLevel::Verbose)
            {
                Output::send<LogLevel::Verbose>(to_wstring(Message) + L"\n");
            }
            else if (Level == LogLevel::Warning)
            {
                Output::send<LogLevel::Warning>(to_wstring(Message) + L"\n");
            }
            else if (Level == LogLevel::Error)
            {
                Output::send<LogLevel::Error>(to_wstring(Message) + L"\n");
            }
        }

        bool Object::IsValid(UObject* Object)
        {
            return Object && UObjectArray::IsValid(Object->GetObjectItem(), false);
        }

        void Object::Invoke(UObject* Object, UFunction* Function, void* Params)
        {
            Object->ProcessEvent(Function, Params);
        }

        UObject* Object::Find(const char* Name)
        {
            return UObjectGlobals::StaticFindObject(nullptr, nullptr, to_wstring(Name));
        }

        UObject* Object::FindFirstOf(const char* Name)
        {
            return UObjectGlobals::FindFirstOf(to_wstring(Name));
        }

        void Object::GetFullName(UObject* Object, char* Name)
        {
            auto name = Object->GetFullName().c_str();
            
            wcstombs(Name, name, wcslen(name));
        }

        void Object::GetName(UObject* Object, char* Name)
        {
            auto name = Object->GetName().c_str();

            wcstombs(Name, name, wcslen(name));
        }

        void Object::GetClass(UObject* Object, UClass** Class)
        {
            *Class = Object->GetClassPrivate();
        }

        void Object::GetOuter(UObject* Object, UObject** Outer)
        {
            *Outer = Object->GetOuterPrivate();
        }

        bool Object::IsAnyClass(UObject* Object)
        {
            return Object->IsA<UClass>();
        }

        void Object::GetWorld(UObject* Object, UWorld** World)
        {
            *World = Object->GetWorld();
        }

        bool Object::IsA(UObject* Object, UClass* Class)
        {
            return Object->IsA(Class);
        }

        bool Object::HasAllFlags(UObject* Object, EObjectFlags FlagsToCheck)
        {
            return Object->HasAllFlags(FlagsToCheck);
        }

        bool Object::HasAnyFlags(UObject* Object, EObjectFlags FlagsToCheck)
        {
            return Object->HasAnyFlags(FlagsToCheck);
        }

        bool Object::HasAnyInternalFlags(UObject* Object, EInternalObjectFlags InternalFlagsToCheck)
        {
            return Object->HasAnyInternalFlags(InternalFlagsToCheck);
        }

        bool Object::GetObjectReference(UObject* Object, const char* Name, UObject** Value)
        {
            CLR_GET_PROPERTY_VALUE(FObjectProperty, UObject*, Object, Name, Value)
        }

        bool Object::GetFunction(UObject* Object, const char* Name, UFunction** Value)
        {
            auto* func = Object->GetFunctionByNameInChain(to_wstring(Name).c_str());
            if (!func) return false;

            *Value = func;
            return true;
        }

        bool Object::GetBool(UObject* Object, const char* Name, bool* Value)
        {
            CLR_GET_PROPERTY_VALUE(FBoolProperty, bool, Object, Name, Value)
        }

        bool Object::GetByte(UObject* Object, const char* Name, uint8* Value)
        {
            CLR_GET_PROPERTY_VALUE(FByteProperty, uint8, Object, Name, Value)
        }

        bool Object::GetShort(UObject* Object, const char* Name, int16* Value)
        {
            CLR_GET_PROPERTY_VALUE(FInt16Property, uint16, Object, Name, Value)
        }

        bool Object::GetInt(UObject* Object, const char* Name, int32* Value)
        {
            CLR_GET_PROPERTY_VALUE(FIntProperty, int, Object, Name, Value)
        }

        bool Object::GetLong(UObject* Object, const char* Name, int64* Value)
        {
            CLR_GET_PROPERTY_VALUE(FInt64Property, int64, Object, Name, Value)
        }

        bool Object::GetUShort(UObject* Object, const char* Name, uint16* Value)
        {
            CLR_GET_PROPERTY_VALUE(FUInt16Property, uint16, Object, Name, Value)
        }

        bool Object::GetUInt(UObject* Object, const char* Name, uint32* Value)
        {
            CLR_GET_PROPERTY_VALUE(FUInt32Property, uint32, Object, Name, Value)
        }

        bool Object::GetULong(UObject* Object, const char* Name, uint64* Value)
        {
            CLR_GET_PROPERTY_VALUE(FUInt64Property, uint64, Object, Name, Value)
        }

        bool Object::GetStruct(UObject* Object, const char* Name, UStruct** Value)
        {
            FStructProperty* prop = static_cast<FStructProperty*>(Object->GetPropertyByNameInChain(to_wstring(Name).c_str()));
            if (!prop) return false;

            *Value = prop->GetStruct();
            return true;
        }

        bool Object::GetArray(UObject* Object, const char* Name, UnArray* Value)
        {
            FArrayProperty* prop = static_cast<FArrayProperty*>(Object->GetPropertyByNameInChain(to_wstring(Name).c_str()));
            if (!prop) return false;

            Value->Data = prop->ContainerPtrToValuePtr<TArray<void*>>(Object)->GetData();
            Value->Length = prop->GetArrayDim();
            Value->Type = StaticState::m_property_type_map[prop->GetInner()->GetFName().GetComparisonIndex()];
            return true;
        }

        bool Object::GetFloat(UObject* Object, const char* Name, float* Value)
        {
            CLR_GET_PROPERTY_VALUE(FFloatProperty, float, Object, Name, Value)
        }

        bool Object::GetDouble(UObject* Object, const char* Name, double* Value)
        {
            CLR_GET_PROPERTY_VALUE(FDoubleProperty, double, Object, Name, Value)
        }

        bool Object::GetEnum(UObject* Object, const char* Name, int64* Value)
        {
            FNumericProperty* prop = static_cast<FNumericProperty*>(Object->GetPropertyByNameInChain(to_wstring(Name).c_str()));
            if (!prop) return false;

            *Value = prop->GetSignedIntPropertyValue(Object);

            return true;
        }

        bool Object::GetWeakObject(UObject* Object, const char* Name, FWeakObjectPtr* Value)
        {
            CLR_GET_PROPERTY_VALUE(FWeakObjectProperty, FWeakObjectPtr, Object, Name, Value)
        }

        bool Object::GetString(UObject* Object, const char* Name, char* Value)
        {
            FStrProperty* prop = static_cast<FStrProperty*>(Object->GetPropertyByNameInChain(to_wstring(Name).c_str()));
            if (!prop) return false;

            const auto str = prop->ContainerPtrToValuePtr<FString>(Object)->GetCharArray();
            wcstombs(Value, str, wcslen(str));

            return true;
        }

        bool Object::GetText(UObject* Object, const char* Name, char* Value)
        {
            FTextProperty* prop = static_cast<FTextProperty*>(Object->GetPropertyByNameInChain(to_wstring(Name).c_str()));
            if (!prop) return false;

            const auto str = prop->ContainerPtrToValuePtr<FText>(Object)->ToString().c_str();
            wcstombs(Value, str, wcslen(str));

            return true;
        }

        bool Object::GetSoftClass(UObject* Object, const char* Name, FSoftObjectPtr* Value)
        {
            CLR_GET_PROPERTY_VALUE(FSoftObjectProperty, FSoftObjectPtr, Object, Name, Value)
        }

        bool Object::SetObjectReference(UObject* Object, const char* Name, UObject* Value)
        {
            CLR_SET_PROPERTY_VALUE(FObjectProperty, UObject*, Object, Name, Value)
        }

        bool Object::SetBool(UObject* Object, const char* Name, bool Value)
        {
            CLR_SET_PROPERTY_VALUE(FBoolProperty, bool, Object, Name, Value)
        }

        bool Object::SetByte(UObject* Object, const char* Name, uint8 Value)
        {
            CLR_SET_PROPERTY_VALUE(FByteProperty, uint8, Object, Name, Value)
        }

        bool Object::SetShort(UObject* Object, const char* Name, int16 Value)
        {
            CLR_SET_PROPERTY_VALUE(FInt16Property, int16, Object, Name, Value)
        }

        bool Object::SetInt(UObject* Object, const char* Name, int32 Value)
        {
            CLR_SET_PROPERTY_VALUE(FIntProperty, int, Object, Name, Value)
        }

        bool Object::SetLong(UObject* Object, const char* Name, int64 Value)
        {
            CLR_SET_PROPERTY_VALUE(FInt64Property, int64, Object, Name, Value)
        }

        bool Object::SetUShort(UObject* Object, const char* Name, uint16 Value)
        {
            CLR_SET_PROPERTY_VALUE(FUInt16Property, uint16, Object, Name, Value)
        }

        bool Object::SetUInt(UObject* Object, const char* Name, uint32 Value)
        {
            CLR_SET_PROPERTY_VALUE(FUInt32Property, uint32, Object, Name, Value)
        }

        bool Object::SetULong(UObject* Object, const char* Name, uint64 Value)
        {
            CLR_SET_PROPERTY_VALUE(FUInt64Property, uint64, Object, Name, Value)
        }

        bool Object::SetStruct(UObject* Object, const char* Name, UStruct* Value)
        {
            FStructProperty* prop = static_cast<FStructProperty*>(Object->GetPropertyByNameInChain(to_wstring(Name).c_str()));
            if (!prop) return false;

            auto* script_struct = prop->GetStruct();
            script_struct->CopyScriptStruct(script_struct, Value);
            return true;
        }

        bool Object::SetArray(UObject* Object, const char* Name, UnArray Value)
        {
            FArrayProperty* prop = static_cast<FArrayProperty*>(Object->GetPropertyByNameInChain(to_wstring(Name).c_str()));
            if (!prop) return false;

            auto array = prop->ContainerPtrToValuePtr<TArray<void*>>(Object);
            FMemory::Memcpy(array->GetData(), Value.Data, Value.Length * prop->GetInner()->GetElementSize());
            array->SetNum(Value.Length);
            array->SetMax(Value.Length);
            return true;
        }

        bool Object::SetFloat(UObject* Object, const char* Name, float Value)
        {
            CLR_SET_PROPERTY_VALUE(FFloatProperty, float, Object, Name, Value)
        }

        bool Object::SetDouble(UObject* Object, const char* Name, double Value)
        {
            CLR_SET_PROPERTY_VALUE(FDoubleProperty, double, Object, Name, Value)
        }

        bool Object::SetEnum(UObject* Object, const char* Name, int32 Value)
        {
            FNumericProperty* prop = static_cast<FNumericProperty*>(Object->GetPropertyByNameInChain(to_wstring(Name).c_str()));
            if (!prop) return false;

            prop->SetIntPropertyValue(Object, static_cast<int64>(Value));

            return true;
        }

        bool Object::SetString(UObject* Object, const char* Name, const char* Value)
        {
            FStrProperty* prop = static_cast<FStrProperty*>(Object->GetPropertyByNameInChain(to_wstring(Name).c_str()));
            if (!prop) return false;

            *prop->ContainerPtrToValuePtr<FString>(Object) = FString(to_wstring(Value).c_str());

            return true;
        }

        bool Object::SetText(UObject* Object, const char* Name, const char* Value)
        {
            FTextProperty* prop = static_cast<FTextProperty*>(Object->GetPropertyByNameInChain(to_wstring(Name).c_str()));
            if (!prop) return false;
            
            *prop->ContainerPtrToValuePtr<FText>(Object) = FText(to_wstring(Value).c_str());

            return true;
        }

        bool Object::SetSoftClass(UObject* Object, const char* Name, FSoftObjectPtr Value)
        {
            CLR_SET_PROPERTY_VALUE(FSoftClassProperty, FSoftObjectPtr, Object, Name, Value)
        }

        UClass* Struct::GetSuperStruct(UStruct* Struct)
        {
            return static_cast<UClass*>(Struct->GetSuperStruct());
        }

        void Struct::ForEachFunction(UStruct* Struct, void (*Callback)(UFunction* Function))
        {
            for (UFunction* function : Struct->ForEachFunction())
            {
                Callback(function);
            }
        }
        
        void Struct::ForEachProperty(UStruct* Struct, void (*Callback)(FProperty* Property))
        {
            for (FProperty* prop : Struct->ForEachProperty())
            {
                Callback(prop);
            }
        }

        void Class::GetCDO(UClass* Class, UObject** CDO)
        {
            *CDO = Class->GetClassDefaultObject();
        }

        bool Class::IsChildOf(UClass* Class, UClass* Parent)
        {
            return Class->IsChildOf(Parent);
        }

        void UnEnum::GetNameByValue(UEnum* Enum, int Value, char* Name)
        {
            auto name = Enum->GetNameByValue(Value).ToString().c_str();

            wcstombs(Name, name, wcslen(name));
        }

        void UnEnum::ForEachName(UEnum* Enum, void (*Callback)(const char* Name, int Value))
        {
            for (auto& [name, value] : Enum->ForEachName())
            {
                const wchar_t* string = name.ToString().c_str();
                char* converted = static_cast<char*>(malloc(wcslen(string)));
                wcstombs(converted, string, wcslen(string));

                Callback(converted, value);
            }
        }

        void UnEnum::GetEnumNameByIndex(UEnum* Enum, int Index, char* Name, int64* Value)
        {
            auto enum_pair = Enum->GetEnumNameByIndex(Index);

            auto name = enum_pair.Key.ToString().c_str();

            wcstombs(Name, name, wcslen(name));

            *Value = enum_pair.Value;
        }

        void UnEnum::InsertIntoNames(UEnum* Enum, char* Name, int Index, int64 Value, bool ShiftValues)
        {
            const Unreal::FName key{to_wstring(Name), FNAME_Add};
            const auto pair = TPair{key, Value};

            Enum->InsertIntoNames(pair, Index, ShiftValues);
        }

        void UnEnum::EditNameAt(UEnum* Enum, int Index, char* Name)
        {
            Enum->EditNameAt(Index, FName(to_wstring(Name)));
        }

        void UnEnum::EditValueAt(UEnum* Enum, int Index, int64 Value)
        {
            Enum->EditValueAt(Index, Value);
        }

        void UnEnum::RemoveFromNamesAt(UEnum* Enum, int Index, int Count, bool AllowShrinking)
        {
            Enum->RemoveFromNamesAt(Index, Count, AllowShrinking);
        }

        int Function::GetParmsSize(UFunction* Func)
        {
            return Func->GetParmsSize();
        }

        int Function::GetOffsetOfParam(UFunction* Func, const char* Name)
        {
            const auto prop = Func->FindProperty(FName(to_wstring(Name)));
            if (!prop)
                throw std::runtime_error{std::format("Property not found: '{}'", Name)};
            return prop->GetOffset_Internal();
        }

        int Function::GetSizeOfParam(UFunction* Func, const char* Name)
        {
            const auto prop = Func->FindProperty(FName(to_wstring(Name)));
            if (!prop)
                throw std::runtime_error{std::format("Property not found: '{}'", Name)};
            return prop->GetSize();
        }

        int Function::GetReturnValueOffset(UFunction* Func)
        {
            const auto returnProp = Func->GetReturnProperty();
            if (!returnProp)
                throw std::runtime_error{std::format("ReturnProperty is null for '{}'", to_string(Func->GetFullName()))};
            return returnProp->GetOffset_Internal();
        }
    }
}
