#include "REDGameState_CharaSelectRE.h"

#include "BPMacros.hpp"
#include "UFunction.hpp"
#include "../../SuzieMod/include/ScriptMacros.hpp"
#include "Property/NumericPropertyTypes.hpp"

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
    };
}

FDynamicClass AREDGameState_CharaSelectRE::Class = {
    .Ctor = &AREDGameState_CharaSelectRE::AREDGameState_CharaSelectRE_Ctor,
};

void AREDGameState_CharaSelectRE::execGetPlayerParam(UObject* Context, [[maybe_unused]] FFrame& Stack,
                                                     [[maybe_unused]] void* Z_Param__Result)
{
    P_GET_PROPERTY(FIntProperty, Z_Param_Side);
    P_FINISH;
    P_NATIVE_BEGIN
        *(FCharaSelectPlayerParam*)Z_Param__Result = static_cast<AREDGameState_CharaSelectRE*>(Context)->GetPlayerParam(
            Z_Param_Side);
    P_NATIVE_END;
}

FCharaSelectPlayerParam AREDGameState_CharaSelectRE::GetPlayerParam(int32 Side) const
{
    if (Side >= 2 || Side <= 0) return PlayerParam[0];
    return PlayerParam[Side];
}

void AREDGameState_CharaSelectRE::execSetPlayerParam(UObject* Context, FFrame& Stack, void* Z_Param__Result)
{
    P_GET_PROPERTY(FIntProperty, Z_Param_Side);
    P_GET_STRUCT(FCharaSelectPlayerParam, Z_Param_Param);
    P_FINISH
    P_NATIVE_BEGIN
        static_cast<AREDGameState_CharaSelectRE*>(Context)->SetPlayerParam(Z_Param_Side, Z_Param_Param);
    P_NATIVE_END
}

void AREDGameState_CharaSelectRE::SetPlayerParam(int32 Side, FCharaSelectPlayerParam Param)
{
    if (Side >= 2 || Side <= 0) PlayerParam[0] = Param;
    else PlayerParam[Side] = Param;
}

void AREDGameState_CharaSelectRE::InitializeWidget()
{
    ProcessEvent(GetClassPrivate()->GetFuncMap().FindRef(FName(STR("InitializeWidget"))), nullptr);
}

void AREDGameState_CharaSelectRE::BeginPlay()
{
    BeginPlay_Func(this);

    if (UObject* GameCommon = UObjectGlobals::FindFirstOf(STR("REDGameCommon")))
    {
        UObject* LobbyState = *GameCommon->GetPropertyByNameInChain(STR("LobbyState"))->ContainerPtrToValuePtr<UObject
            *>(GameCommon);
        if (LobbyState)
        {
            FWeakObjectPtr LocalUserStateSlot = *LobbyState->GetPropertyByNameInChain(STR("LocalUserStateSlot"))->
                                                             ContainerPtrToValuePtr<FWeakObjectPtr>(LobbyState);
            if (LocalUserStateSlot.Get() != nullptr)
            {
                if (*(uint8*)((uintptr_t)GameCommon + 0x2F0) == 0x8) // GAME_MODE_TRAINING
                {
                    UObject* LocalUserMatchingRoomState = *LobbyState->GetPropertyByNameInChain(
                        STR("LocalUserMatchingRoomState"))->ContainerPtrToValuePtr<UObject*>(LobbyState);
                    if (*(uint8*)((uintptr_t)LocalUserMatchingRoomState + 0xB6)) // bIsInTraining
                    {
                        OnStartedLobbyTraining_Func(LobbyState);
                    }
                }
            }
        }
    }

    const auto HUD = GetHUD();
    AREDHUD_FadeIn_Func(HUD, 20);
}

void AREDGameState_CharaSelectRE::HandleMatchIsWaitingToStart()
{
    HandleMatchIsWaitingToStart_Func(this);

    InitializeWidget();
}

void AREDGameState_CharaSelectRE::SceneInitialize()
{
    SceneInitialize_Func(this);

    UObject* GameCommon = UObjectGlobals::FindFirstOf(STR("REDGameCommon"));
    if (!GameCommon) return;

    UObject* LobbyState = *GameCommon->GetPropertyByNameInChain(STR("LobbyState"))->ContainerPtrToValuePtr<UObject
        *>(GameCommon);
    if (!LobbyState) return;

    UObject* NetworkInterface = *LobbyState->GetPropertyByNameInChain(STR("NetworkInterface"))->ContainerPtrToValuePtr<
        UObject*>(LobbyState);
    if (!NetworkInterface) return;

    auto IsConnected = reinterpret_cast<bool(*)(UObject*)>(*reinterpret_cast<uintptr_t*>(NetworkInterface) + 0x298);

    if (IsConnected(NetworkInterface))
    {
        bStartedOnlineTraining = *reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(LobbyState) + 0x21A);
    }

    bStartedOnlineTraining = false;
}

AActor* AREDGameState_CharaSelectRE::GetHUD()
{
    using namespace RC;
        
    auto Function = PrimaryPC->GetFunctionByNameInChain(STR("GetHUD"));
    static auto ParamStructSize = Function->GetParmsSize();
    const auto ParamData = static_cast<uint8*>(_malloca(ParamStructSize));
    FMemory::Memzero(ParamData, ParamStructSize);

    PrimaryPC->ProcessEvent(Function, ParamData);

    UE_RETURN_PROPERTY(AActor*)
}

void AREDGameState_CharaSelectRE::AREDGameState_CharaSelectRE_Ctor(const FObjectInitializer* ObjectInitializer)
{
    auto Super = StaticClass()->GetSuperClass();
    uintptr_t CtorAddr = reinterpret_cast<uintptr_t>(Super->GetClassConstructor()) + 0xE;
    CtorAddr = *reinterpret_cast<int*>(CtorAddr + 0x1) + (CtorAddr + 0x5);
    auto Ctor = reinterpret_cast<void(*)(UObject*, const FObjectInitializer*)>(CtorAddr);
    Ctor(ObjectInitializer->Obj, ObjectInitializer);

    auto CastContext = static_cast<AREDGameState_CharaSelectRE*>(ObjectInitializer->Obj);

    CastContext->InitializeVTable();

    CastContext->bStartedOnlineTraining = false;
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
    };
    Class.Functions = {
        FDynamicFunction{
            .Path = FString(STR("/Script/REDExtend.REDGameState_CharaSelectRE:GetPlayerParam")),
            .Func = &AREDGameState_CharaSelectRE::execGetPlayerParam,
            .Flags = FUNC_Public | FUNC_BlueprintCallable | FUNC_BlueprintPure,
            .Properties = {
                FDynamicProperty{
                    .Flags = CPF_ConstParm | CPF_Parm | CPF_ZeroConstructor | CPF_IsPlainOldData | CPF_NoDestructor |
                    CPF_HasGetValueTypeHash | CPF_NativeAccessSpecifierPublic,
                    .Name = FString(STR("Side")),
                    .Type = FString(STR("IntProperty")),
                    .Offset = 0,
                },
                FDynamicProperty{
                    .Flags = CPF_Parm | CPF_OutParm | CPF_ZeroConstructor | CPF_ReturnParm | CPF_IsPlainOldData |
                    CPF_NoDestructor | CPF_HasGetValueTypeHash | CPF_NativeAccessSpecifierPublic,
                    .Name = FString(STR("ReturnValue")),
                    .Type = FString(STR("StructProperty")),
                    .Offset = 4,
                    .Struct = FString(STR("/Script/REDExtend.CharaSelectPlayerParam")),
                },
            },
        },
        FDynamicFunction{
            .Path = FString(STR("/Script/REDExtend.REDGameState_CharaSelectRE:SetPlayerParam")),
            .Func = &AREDGameState_CharaSelectRE::execSetPlayerParam,
            .Flags = FUNC_Public | FUNC_BlueprintCallable,
            .Properties = {
                FDynamicProperty{
                    .Flags = CPF_ConstParm | CPF_Parm | CPF_ZeroConstructor | CPF_IsPlainOldData | CPF_NoDestructor |
                    CPF_HasGetValueTypeHash | CPF_NativeAccessSpecifierPublic,
                    .Name = FString(STR("Side")),
                    .Type = FString(STR("IntProperty")),
                    .Offset = 0,
                },
                FDynamicProperty{
                    .Flags = CPF_Parm | CPF_ZeroConstructor | CPF_IsPlainOldData | CPF_NoDestructor |
                    CPF_HasGetValueTypeHash | CPF_NativeAccessSpecifierPublic,
                    .Name = FString(STR("Param")),
                    .Type = FString(STR("StructProperty")),
                    .Offset = 4,
                    .Struct = FString(STR("/Script/REDExtend.CharaSelectPlayerParam")),
                },
            },
        },
        FDynamicFunction{
            .Path = FString(STR("/Script/REDExtend.REDGameState_CharaSelectRE:InitializeWidget")),
            .Flags = FUNC_Event | FUNC_Public | FUNC_BlueprintEvent,
        },
    };
}

void AREDGameState_CharaSelectRE::InitializeVTable()
{
    auto OrigVTable = *reinterpret_cast<uintptr_t**>(this);

    FMemory::Memcpy(VTable, OrigVTable, 0x828);
    *reinterpret_cast<uintptr_t*>(this) = (uintptr_t)&VTable;

    BeginPlay_Func.assign_address((void*)VTable[0x65]);
    HandleMatchIsWaitingToStart_Func.assign_address((void*)VTable[0xD8]);
    SceneInitialize_Func.assign_address((void*)VTable[0xE3]);

    *reinterpret_cast<void(AREDGameState_CharaSelectRE::**)()>(&VTable[0x65]) = &AREDGameState_CharaSelectRE::BeginPlay;
    *reinterpret_cast<void(AREDGameState_CharaSelectRE::**)()>(&VTable[0xD8]) =
        &AREDGameState_CharaSelectRE::HandleMatchIsWaitingToStart;
    *reinterpret_cast<void(AREDGameState_CharaSelectRE::**)()>(&VTable[0xE3]) =
        &AREDGameState_CharaSelectRE::SceneInitialize;
}
