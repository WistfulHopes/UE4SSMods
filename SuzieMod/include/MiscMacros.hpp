#pragma once

#include "Concepts/GetTypeHashable.hpp"

template <typename T>
concept A_IsBitField = !requires (uint8 Byte) {
     { &Byte };
};

FORCEINLINE const TCHAR* GetPropertyName(const std::type_info& Type)
{
    if (Type == typeid(uint8) || Type == typeid(UTF8CHAR))
    {
        return STR("ByteProperty");
    }
    if (Type == typeid(uint16))
    {
        return STR("UInt16Property");
    }
    if (Type == typeid(uint32))
    {
        return STR("UInt32Property");
    }
    if (Type == typeid(uint64))
    {
        return STR("UInt64Property");
    }
    if (Type == typeid(int8))
    {
        return STR("Int8Property");
    }
    if (Type == typeid(int16))
    {
        return STR("Int16Property");
    }
    if (Type == typeid(int32))
    {
        return STR("IntProperty");
    }
    if (Type == typeid(int64))
    {
        return STR("Int64Property");
    }
    if (Type == typeid(bool))
    {
        return STR("BoolProperty");
    }
    return STR("Property");
}

#define CALL_BASE_CTOR()                                                                    \
    auto Super = StaticClass()->GetSuperClass();                                            \
    uintptr_t CtorAddr = reinterpret_cast<uintptr_t>(Super->GetClassConstructor()) + 0xE;   \
    CtorAddr = *reinterpret_cast<int*>(CtorAddr + 0x1) + (CtorAddr + 0x5);                  \
    auto Ctor = reinterpret_cast<void(*)(UObject*, const FObjectInitializer*)>(CtorAddr);   \
    Ctor(ObjectInitializer->Obj, ObjectInitializer);

#define CLONE_VTABLE(VTable, Size)                              \
    auto OrigVTable = *reinterpret_cast<uintptr_t**>(this);     \
    FMemory::Memcpy(VTable, OrigVTable, Size * 8);                 \
    *reinterpret_cast<uintptr_t*>(this) = (uintptr_t)&VTable;

#define ASSIGN_TO_VTABLE(VTable, Index, Class, OriginalFunc, OverrideFunc)  \
    OriginalFunc.assign_address((void*)VTable[Index]);                      \
    *reinterpret_cast<void(Class::**)()>(&VTable[Index]) = &Class::OverrideFunc;

#define IS_STRUCT_PROPERTY(TypeName)                            \
    bool IsStructProperty = false;                              \
    if (std::is_class<TypeName>::value)                         \
    {                                                           \
        if (!std::derived_from<TypeName, UObject>)              \
            IsStructProperty = true;                            \
    }

#define IS_OBJECT_PROPERTY(TypeName)                            \
    bool IsObjectProperty = false;                              \
    if (std::is_class<TypeName>::value)                         \
    {                                                           \
        if (std::derived_from<TypeName, UObject>)               \
            IsObjectProperty = true;                            \
    }

#define GET_PROPERTY_NAME(TypeName)                             \
    FString PropertyName;                                       \
    PropertyName = FString(GetPropertyName(typeid(TypeName)));  \
    IS_OBJECT_PROPERTY(TypeName)                                \
    if (IsObjectProperty)                                       \
        PropertyName = FString(STR("ObjectProperty"));          \
    IS_STRUCT_PROPERTY(TypeName)                                \
    if (IsStructProperty)                                       \
        PropertyName = FString(STR("StructProperty"));

#define PROPERTY_FLAGS(TypeName)                                                                    \
    EPropertyFlags Flags = CPF_None;                                                                \
    if (TIsPODType<TypeName>::Value)                                                                \
        Flags |= CPF_IsPlainOldData;                                                                \
    if (std::is_trivially_destructible_v<TypeName>)                                                 \
        Flags |= CPF_NoDestructor;                                                                  \
    if (TIsZeroConstructType<TypeName>::Value)                                                      \
        Flags |= CPF_ZeroConstructor;                                                               \
    if (TModels_V<CGetTypeHashable, TypeName>)                                                      \
        Flags |= CPF_HasGetValueTypeHash;

#define CREATE_PROPERTY(TypeName, ClassName, MemberName, InFlags)                                   \
    {                                                                                               \
        PROPERTY_FLAGS(TypeName)                                                                    \
        Flags |= InFlags;                                                                           \
        auto Property = FDynamicProperty {                                                          \
            .Flags = Flags,                                                                         \
            .Name = FString(STR(#MemberName)),                                                      \
            .ArrayDim = 1,                                                                          \
            .Offset = offsetof(ClassName, MemberName),                                              \
        };                                                                                          \
        GET_PROPERTY_NAME(TypeName)                                                                 \
        Property.Type = PropertyName;                                                               \
        Data.Properties.Add(Property);                                                              \
    }

#define CREATE_COMPLEX_PROPERTY(TypeName, ClassName, MemberName, InFlags, ScriptPath)               \
    {                                                                                               \
        PROPERTY_FLAGS(TypeName)                                                                    \
        Flags |= InFlags;                                                                           \
        auto Property = FDynamicProperty {                                                          \
            .Flags = Flags,                                                                         \
            .Name = FString(STR(#MemberName)),                                                      \
            .ArrayDim = 1,                                                                          \
            .Offset = offsetof(ClassName, MemberName),                                              \
        };                                                                                          \
        {                                                                                           \
            GET_PROPERTY_NAME(TypeName)                                                             \
            Property.Type = PropertyName;                                                           \
        }                                                                                           \
        {                                                                                           \
            IS_OBJECT_PROPERTY(TypeName)                                                            \
            if (IsObjectProperty)                                                                   \
                Property.Class = ScriptPath;                                                        \
            IS_STRUCT_PROPERTY(TypeName)                                                            \
            if (IsStructProperty)                                                                   \
                Property.Struct = ScriptPath;                                                       \
        }                                                                                           \
        Data.Properties.Add(Property);                                                              \
    }

#define CREATE_ARRAY_PROPERTY(TypeName, ClassName, MemberName, InFlags, InArrayDim)                 \
    {                                                                                               \
        PROPERTY_FLAGS(TypeName)                                                                    \
        Flags |= InFlags;                                                                           \
        auto Property = FDynamicProperty {                                                          \
            .Flags = Flags,                                                                         \
            .Name = FString(STR(#MemberName)),                                                      \
            .ArrayDim = InArrayDim,                                                                 \
            .Offset = offsetof(ClassName, MemberName),                                              \
        };                                                                                          \
        GET_PROPERTY_NAME(TypeName)                                                                 \
        Property.Type = PropertyName;                                                               \
        Data.Properties.Add(Property);                                                              \
    }

#define CREATE_COMPLEX_ARRAY_PROPERTY(TypeName, ClassName, MemberName, InFlags, ScriptPath, InArrayDim) \
    {                                                                                                   \
        PROPERTY_FLAGS(TypeName)                                                                        \
        Flags |= InFlags;                                                                               \
        auto Property = FDynamicProperty {                                                              \
            .Flags = Flags,                                                                             \
            .Name = FString(STR(#MemberName)),                                                          \
            .ArrayDim = InArrayDim,                                                                     \
            .Offset = offsetof(ClassName, MemberName),                                                  \
        };                                                                                              \
        {                                                                                               \
            GET_PROPERTY_NAME(TypeName)                                                                 \
            Property.Type = PropertyName;                                                               \
        }                                                                                               \
        {                                                                                               \
            IS_OBJECT_PROPERTY(TypeName)                                                                \
            if (IsObjectProperty)                                                                       \
                Property.Class = ScriptPath;                                                            \
            IS_STRUCT_PROPERTY(TypeName)                                                                \
            if (IsStructProperty)                                                                       \
                Property.Struct = ScriptPath;                                                           \
        }                                                                                               \
        Data.Properties.Add(Property);                                                                  \
    }

#define CREATE_FUNCTION(ClassName, FuncName, InFlags, ScriptPath, Params)       \
    {                                                                           \
        auto Function = FDynamicFunction {                                      \
            .Path = ScriptPath,                                                 \
            .Func = &ClassName::FuncName,                                       \
            .Flags = InFlags,                                                   \
            .Properties = Params,                                               \
        };                                                                      \
        Data.Functions.Add(Function);                                           \
    }

#define CREATE_EVENT(InFlags, ScriptPath, Params)                               \
    {                                                                           \
        auto Function = FDynamicFunction {                                      \
            .Path = ScriptPath,                                                 \
            .Flags = FUNC_Event | InFlags,                                      \
            .Properties = Params,                                               \
        };                                                                      \
        Data.Functions.Add(Function);                                           \
    }

#define PARAM_FLAGS(TypeName)                       \
    EPropertyFlags ParamFlags = CPF_None;           \
    if (std::is_const<TypeName>::value)             \
        ParamFlags |= CPF_ConstParm;                \
    else if (std::is_reference<TypeName>::value)    \
        ParamFlags |= CPF_OutParm;          

#define PARAM_BEGIN()                           \
    auto Params = TArray<FDynamicProperty>{};   \
    int32 Offset = 0;                           \

#define GET_PARAM_TYPE_NAME(TypeName)                           \
    FString ParamType;                                          \
    ParamType = FString(GetPropertyName(typeid(TypeName)));     \
    IS_OBJECT_PROPERTY(TypeName)                                \
    if (IsObjectProperty)                                       \
        ParamType = FString(STR("ObjectProperty"));             \
    IS_STRUCT_PROPERTY(TypeName)                                \
    if (IsStructProperty)                                       \
        ParamType = FString(STR("StructProperty"));

#define CREATE_PARAM(TypeName, ParamName, InFlags)                                              \
    {                                                                                           \
        PROPERTY_FLAGS(TypeName)                                                                \
        PARAM_FLAGS(TypeName)                                                                   \
        auto Param = FDynamicProperty {                                                         \
            .Flags = CPF_NativeAccessSpecifierPublic | CPF_Parm | Flags | ParamFlags|  InFlags, \
            .Name = FString(STR(#ParamName)),                                                   \
            .Offset = Offset,                                                                   \
        };                                                                                      \
        GET_PARAM_TYPE_NAME(TypeName)                                                           \
        Param.Type = ParamType;                                                                 \
        Params.Add(Param);                                                                      \
        Offset += sizeof(TypeName);                                                             \
    }

#define CREATE_COMPLEX_PARAM(TypeName, ParamName, InFlags, ScriptPath)                          \
    {                                                                                           \
        PROPERTY_FLAGS(TypeName)                                                                \
        PARAM_FLAGS(TypeName)                                                                   \
        auto Param = FDynamicProperty {                                                         \
            .Flags = CPF_NativeAccessSpecifierPublic | CPF_Parm | Flags | ParamFlags | InFlags, \
            .Name = FString(STR(#ParamName)),                                                   \
            .Offset = Offset,                                                                   \
        };                                                                                      \
        {                                                                                       \
            GET_PARAM_TYPE_NAME(TypeName)                                                       \
            Param.Type = ParamType;                                                             \
        }                                                                                       \
        {                                                                                       \
            IS_OBJECT_PROPERTY(TypeName)                                                        \
            if (IsObjectProperty)                                                               \
                Param.Class = ScriptPath;                                                       \
            IS_STRUCT_PROPERTY(TypeName)                                                        \
            if (IsStructProperty)                                                               \
                Param.Struct = ScriptPath;                                                      \
        }                                                                                       \
        Params.Add(Param);                                                                      \
        Offset += sizeof(TypeName);                                                             \
    }