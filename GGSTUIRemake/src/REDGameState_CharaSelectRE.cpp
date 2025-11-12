#include "REDGameState_CharaSelectRE.h"

#include "BPMacros.hpp"
#include "UFunction.hpp"
#include "../../SuzieMod/include/ScriptMacros.hpp"
#include "../../SuzieMod/include/MiscMacros.hpp"
#include "Property/NumericPropertyTypes.hpp"

IMPLEMENT_EXTERNAL_OBJECT_CLASS(AGameStateBase);
IMPLEMENT_EXTERNAL_OBJECT_CLASS(AREDGameState);
IMPLEMENT_EXTERNAL_OBJECT_CLASS(AREDGameState_CharaSelectRE);

void FCharaSelectPlayerParam::InitializeStruct()
{
    PROPERTY_DEFINE(int32, FCharaSelectPlayerParam, Cursor,
                    CPF_Edit | CPF_BlueprintVisible | CPF_NativeAccessSpecifierPublic)
    COMPLEX_PROPERTY_DEFINE(FVector, FCharaSelectPlayerParam, CursorPos,
                            CPF_Edit | CPF_BlueprintVisible | CPF_NativeAccessSpecifierPublic,
                            FString(STR("/Script/CoreUObject.Vector")))
    PROPERTY_DEFINE(int32, FCharaSelectPlayerParam, RandomCounter,
                    CPF_Edit | CPF_BlueprintVisible | CPF_NativeAccessSpecifierPublic)
    PROPERTY_DEFINE(bool, FCharaSelectPlayerParam, bRandomSelect,
                    CPF_Edit | CPF_BlueprintVisible | CPF_NativeAccessSpecifierPublic)
    PROPERTY_DEFINE(int32, FCharaSelectPlayerParam, PrevCursor,
                    CPF_Edit | CPF_BlueprintVisible | CPF_NativeAccessSpecifierPublic)
    PROPERTY_DEFINE(int32, FCharaSelectPlayerParam, SelectColor,
                    CPF_Edit | CPF_BlueprintVisible | CPF_NativeAccessSpecifierPublic)
    PROPERTY_DEFINE(int32, FCharaSelectPlayerParam, SelectChara,
                    CPF_Edit | CPF_BlueprintVisible | CPF_NativeAccessSpecifierPublic)
    PROPERTY_DEFINE(bool, FCharaSelectPlayerParam, bSelectCPU,
                    CPF_Edit | CPF_BlueprintVisible | CPF_NativeAccessSpecifierPublic)
}

FDynamicClass AREDGameState_CharaSelectRE::Data = {
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

FCharaSelectPlayerParam AREDGameState_CharaSelectRE::GetPlayerParam(const int32 Side) const
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

void AREDGameState_CharaSelectRE::SetPlayerParam(const int32 Side, FCharaSelectPlayerParam Param)
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
    CALL_BASE_CTOR()

    auto CastContext = static_cast<AREDGameState_CharaSelectRE*>(ObjectInitializer->Obj);

    CastContext->InitializeVTable();

    CastContext->bStartedOnlineTraining = false;
}

void AREDGameState_CharaSelectRE::InitializeClass()
{
    Data.ClassDefaultObjectPath = FString(STR("/Script/REDExtend.Default__REDGameState_CharaSelectRE"));
    Data.SuperStruct = FString(STR("/Script/RED.REDGameState"));

    COMPLEX_PROPERTY_DEFINE(FCharaSelectPlayerParam, AREDGameState_CharaSelectRE, PlayerParam,
                            CPF_Edit | CPF_BlueprintVisible | CPF_NativeAccessSpecifierPublic,
                            FString(STR("/Script/REDExtend.CharaSelectPlayerParam")))
    {
        PARAM_BEGIN()
        PARAM_DEFINE(const int32, Side, CPF_None)
        COMPLEX_PARAM_DEFINE(FCharaSelectPlayerParam, ReturnValue, CPF_ReturnParm,
                             FString(STR("/Script/REDExtend.CharaSelectPlayerParam")))
        FUNCTION_DEFINE(AREDGameState_CharaSelectRE, execGetPlayerParam,
                        FUNC_Public | FUNC_BlueprintCallable | FUNC_BlueprintPure,
                        FString(STR("/Script/REDExtend.REDGameState_CharaSelectRE:GetPlayerParam")), Params)
    }
    {
        PARAM_BEGIN()
        PARAM_DEFINE(const int32, Side, CPF_None)
        COMPLEX_PARAM_DEFINE(FCharaSelectPlayerParam, Param, CPF_None,
                             FString(STR("/Script/REDExtend.CharaSelectPlayerParam")))
        FUNCTION_DEFINE(AREDGameState_CharaSelectRE, execSetPlayerParam,
                        FUNC_Public | FUNC_BlueprintCallable | FUNC_BlueprintPure,
                        FString(STR("/Script/REDExtend.REDGameState_CharaSelectRE:SetPlayerParam")), Params)
    }
    EVENT_DEFINE(FUNC_BlueprintEvent, FString(STR("/Script/REDExtend.REDGameState_CharaSelectRE:InitializeWidget")), {})
}

void AREDGameState_CharaSelectRE::InitializeVTable()
{
    CLONE_VTABLE(VTable, 0x105)

    ASSIGN_TO_VTABLE(VTable, 0x65, AREDGameState_CharaSelectRE, BeginPlay_Func, BeginPlay)
    ASSIGN_TO_VTABLE(VTable, 0xD8, AREDGameState_CharaSelectRE, HandleMatchIsWaitingToStart_Func,
                     HandleMatchIsWaitingToStart)
    ASSIGN_TO_VTABLE(VTable, 0xE3, AREDGameState_CharaSelectRE, SceneInitialize_Func, SceneInitialize)
}
