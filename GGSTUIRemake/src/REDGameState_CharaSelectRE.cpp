#include "REDGameState_CharaSelectRE.h"

IMPLEMENT_EXTERNAL_OBJECT_CLASS(AGameStateBase);
IMPLEMENT_EXTERNAL_OBJECT_CLASS(AREDGameState);
IMPLEMENT_EXTERNAL_OBJECT_CLASS(AREDGameState_CharaSelectRE);

void FCharaSelectPlayerParam::InitializeStruct()
{
    Struct = {
        .Properties = {
            FDynamicProperty{
                .Flags = CPF_Edit | CPF_BlueprintVisible | CPF_IsPlainOldData | CPF_NoDestructor |
                CPF_HasGetValueTypeHash | CPF_NativeAccessSpecifierPublic,
                .Name = FString(STR("Cursor")),
                .Type = FString(STR("IntProperty")),
                .ArrayDim = 1,
                .Offset = offsetof(FCharaSelectPlayerParam, Cursor),
            },
            FDynamicProperty{
                .Flags = CPF_Edit | CPF_BlueprintVisible | CPF_ZeroConstructor | CPF_IsPlainOldData | CPF_NoDestructor |
                CPF_HasGetValueTypeHash | CPF_NativeAccessSpecifierPublic,
                .Name = FString(STR("CursorPos")),
                .Type = FString(STR("StructProperty")),
                .ArrayDim = 1,
                .Offset = offsetof(FCharaSelectPlayerParam, CursorPos),
                .Struct = FString(STR("/Script/CoreUObject.Vector")),
            },
            FDynamicProperty{
                .Flags = CPF_Edit | CPF_BlueprintVisible | CPF_IsPlainOldData | CPF_NoDestructor |
                CPF_HasGetValueTypeHash | CPF_NativeAccessSpecifierPublic,
                .Name = FString(STR("RandomCounter")),
                .Type = FString(STR("IntProperty")),
                .ArrayDim = 1,
                .Offset = offsetof(FCharaSelectPlayerParam, RandomCounter),
            },
            FDynamicProperty{
                .Flags = CPF_Edit | CPF_BlueprintVisible | CPF_ZeroConstructor | CPF_IsPlainOldData | CPF_NoDestructor |
                CPF_HasGetValueTypeHash | CPF_NativeAccessSpecifierPublic,
                .Name = FString(STR("bRandomSelect")),
                .Type = FString(STR("BoolProperty")),
                .ArrayDim = 1,
                .Offset = offsetof(FCharaSelectPlayerParam, bRandomSelect),
            },
            FDynamicProperty{
                .Flags = CPF_Edit | CPF_BlueprintVisible | CPF_IsPlainOldData | CPF_NoDestructor |
                CPF_HasGetValueTypeHash | CPF_NativeAccessSpecifierPublic,
                .Name = FString(STR("PrevCursor")),
                .Type = FString(STR("IntProperty")),
                .ArrayDim = 1,
                .Offset = offsetof(FCharaSelectPlayerParam, PrevCursor),
            },
            FDynamicProperty{
                .Flags = CPF_Edit | CPF_BlueprintVisible | CPF_IsPlainOldData | CPF_NoDestructor |
                CPF_HasGetValueTypeHash | CPF_NativeAccessSpecifierPublic,
                .Name = FString(STR("SelectColor")),
                .Type = FString(STR("IntProperty")),
                .ArrayDim = 1,
                .Offset = offsetof(FCharaSelectPlayerParam, SelectColor),
            },
            FDynamicProperty{
                .Flags = CPF_Edit | CPF_BlueprintVisible | CPF_IsPlainOldData | CPF_NoDestructor |
                CPF_HasGetValueTypeHash | CPF_NativeAccessSpecifierPublic,
                .Name = FString(STR("SelectChara")),
                .Type = FString(STR("IntProperty")),
                .ArrayDim = 1,
                .Offset = offsetof(FCharaSelectPlayerParam, SelectColor),
            },
            FDynamicProperty{
                .Flags = CPF_Edit | CPF_BlueprintVisible | CPF_ZeroConstructor | CPF_IsPlainOldData | CPF_NoDestructor |
                CPF_HasGetValueTypeHash | CPF_NativeAccessSpecifierPublic,
                .Name = FString(STR("bSelectCPU")),
                .Type = FString(STR("BoolProperty")),
                .ArrayDim = 1,
                .Offset = offsetof(FCharaSelectPlayerParam, bSelectCPU),
            }
        },
        .Size = sizeof(FCharaSelectPlayerParam),
    };
}

FDynamicClass AREDGameState_CharaSelectRE::Class = {
    .Ctor = &AREDGameState_CharaSelectRE::AREDGameState_CharaSelectRE_Ctor,
};

void AREDGameState_CharaSelectRE::AREDGameState_CharaSelectRE_Ctor(const FObjectInitializer* ObjectInitializer)
{
    auto Super = StaticClass()->GetSuperClass();
    uintptr_t CtorAddr = (uintptr_t)Super->GetClassConstructor() + 0xE;
    CtorAddr = *(int*)(CtorAddr + 0x1) + (CtorAddr + 0x5);
    auto Ctor = reinterpret_cast<void(*)(UObject*, const FObjectInitializer*)>(CtorAddr);
    Ctor(ObjectInitializer->Obj, ObjectInitializer);
}

void AREDGameState_CharaSelectRE::InitializeClass()
{
    Class.ClassDefaultObjectPath = FString(STR("/Script/REDExtend.Default__REDGameState_CharaSelectRE"));
    Class.SuperStruct = FString(STR("/Script/RED.REDGameState"));
    Class.Properties = {
        FDynamicProperty{
            .Flags = CPF_Edit | CPF_BlueprintVisible | CPF_HasGetValueTypeHash | CPF_NativeAccessSpecifierPublic,
            .Name = FString(STR("PlayerParam")),
            .Type = FString(STR("StructProperty")),
            .ArrayDim = 2,
            .Offset = offsetof(AREDGameState_CharaSelectRE, PlayerParam),
            .Struct = FString(STR("/Script/REDExtend.CharaSelectPlayerParam")),
        },
        FDynamicProperty{
            .Flags = CPF_Edit | CPF_BlueprintVisible | CPF_ZeroConstructor | CPF_IsPlainOldData | CPF_NoDestructor |
            CPF_HasGetValueTypeHash | CPF_NativeAccessSpecifierPublic,
            .Name = FString(STR("bInitialized")),
            .Type = FString(STR("BoolProperty")),
            .ArrayDim = 1,
            .Offset = offsetof(AREDGameState_CharaSelectRE, bInitialized),
        },
    };
}
