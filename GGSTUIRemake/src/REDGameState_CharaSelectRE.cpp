#include "REDGameState_CharaSelectRE.h"

#include "BPMacros.hpp"
#include "../../SuzieMod/include/ScriptMacros.hpp"
#include "../../SuzieMod/include/MiscMacros.hpp"
#include "Property/NumericPropertyTypes.hpp"

IMPLEMENT_EXTERNAL_OBJECT_CLASS(AGameStateBase);
IMPLEMENT_EXTERNAL_OBJECT_CLASS(AREDGameState);
IMPLEMENT_EXTERNAL_OBJECT_CLASS(AREDGameState_CharaSelectRE);

FDynamicEnum ECharaState_Data{};

void ECharaState_InitializeEnum()
{
    ECharaState_Data.CppType = FString(STR("Regular"));
    ECharaState_Data.Pairs.Add(FString(STR("CharaState_First")), CharaState_First);
    ECharaState_Data.Pairs.Add(FString(STR("CharaState_Second")), CharaState_Second);
    ECharaState_Data.Pairs.Add(FString(STR("CharaState_Third")), CharaState_Third);
    ECharaState_Data.Pairs.Add(FString(STR("CharaState_ColorCostume")), CharaState_ColorCostume);
    ECharaState_Data.Pairs.Add(FString(STR("CharaState_Ready")), CharaState_Ready);
}

void FDecideInfo::InitializeStruct()
{
    CREATE_ENUM_PROPERTY(ECharaID, FDecideInfo, CharaID,
                     CPF_BlueprintVisible | CPF_NativeAccessSpecifierPublic,
                     FString(STR("/Script/RED.ECharaID")), uint8)
    CREATE_ENUM_PROPERTY(EColorID, FDecideInfo, ColorID,
                     CPF_BlueprintVisible | CPF_NativeAccessSpecifierPublic,
                     FString(STR("/Script/RED.EColorID")), uint8)
    CREATE_ENUM_PROPERTY(ECostumeID, FDecideInfo, CostumeID,
                     CPF_BlueprintVisible | CPF_NativeAccessSpecifierPublic,
                     FString(STR("/Script/RED.ECostumeID")), uint8)
    CREATE_ENUM_PROPERTY(EBattleScript, FDecideInfo, ScriptID,
                     CPF_BlueprintVisible | CPF_NativeAccessSpecifierPublic,
                     FString(STR("/Script/RED.EBattleScript")), uint8)
    CREATE_PROPERTY(uint8, FDecideInfo, SpFlag,
                CPF_BlueprintVisible | CPF_NativeAccessSpecifierPublic)
    CREATE_PROPERTY(int32, FDecideInfo, SkillSetIndex,
                CPF_BlueprintVisible | CPF_NativeAccessSpecifierPublic)
}

void FCharaSelectPlayerParam::InitializeStruct()
{
    CREATE_PROPERTY(int32, FCharaSelectPlayerParam, Cursor,
                    CPF_BlueprintVisible | CPF_NativeAccessSpecifierPublic)
    CREATE_COMPLEX_PROPERTY(FVector, FCharaSelectPlayerParam, CursorPos,
                            CPF_BlueprintVisible | CPF_NativeAccessSpecifierPublic,
                            FString(STR("/Script/CoreUObject.Vector2D")))
    CREATE_PROPERTY(int32, FCharaSelectPlayerParam, RandomCounter,
                    CPF_BlueprintVisible | CPF_NativeAccessSpecifierPublic)
    CREATE_PROPERTY(bool, FCharaSelectPlayerParam, bRandomSelect,
                    CPF_BlueprintVisible | CPF_NativeAccessSpecifierPublic)
    CREATE_PROPERTY(int32, FCharaSelectPlayerParam, PrevCursor,
                    CPF_BlueprintVisible | CPF_NativeAccessSpecifierPublic)
    CREATE_COMPLEX_STATIC_ARRAY_PROPERTY(FDecideInfo, FCharaSelectPlayerParam, DecideInfo,
                            CPF_NativeAccessSpecifierPublic,
                            FString(STR("/Script/REDExtend.DecideInfo")), 3)
    CREATE_PROPERTY(int32, FCharaSelectPlayerParam, TeamSelectIndex,
                    CPF_BlueprintVisible | CPF_NativeAccessSpecifierPublic)
    CREATE_PROPERTY(int32, FCharaSelectPlayerParam, CustomCursor,
                    CPF_BlueprintVisible | CPF_NativeAccessSpecifierPublic)
    CREATE_PROPERTY(bool, FCharaSelectPlayerParam, bSelectCPU,
                    CPF_BlueprintVisible | CPF_NativeAccessSpecifierPublic)
    CREATE_ENUM_PROPERTY(ECharaState, FCharaSelectPlayerParam, StageID,
                         CPF_BlueprintVisible | CPF_NativeAccessSpecifierPublic,
                         FString(STR("/Script/REDExtend.ECharaState")), uint8)
    CREATE_ENUM_PROPERTY(EBattleStage, FCharaSelectPlayerParam, StageID,
                         CPF_BlueprintVisible | CPF_NativeAccessSpecifierPublic,
                         FString(STR("/Script/RED.EBattleStage")), uint8)
    CREATE_ENUM_PROPERTY(EBGMID, FCharaSelectPlayerParam, BGMID,
                         CPF_BlueprintVisible | CPF_NativeAccessSpecifierPublic,
                         FString(STR("/Script/RED.EBGMID")), int32)
    CREATE_PROPERTY(bool, FCharaSelectPlayerParam, bReady,
                    CPF_BlueprintVisible | CPF_NativeAccessSpecifierPublic)
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
    std::function<UFunction*(UClass*, FName)> FindFunctionByName;
    FindFunctionByName = [FindFunctionByName](UClass* Class, FName InName) -> UFunction*
    {
        auto Result = Class->GetFuncMap().FindRef(InName);
        if (Result == nullptr)
        {
            auto SuperClass = Class->GetSuperClass();
            if (SuperClass || Class->GetInterfaces().Num() > 0)
            {
                bool bFoundInSuperFuncMap = false;
                {
                    if (UFunction** SuperResult = Class->GetSuperFuncMap().Find(InName))
                    {
                        Result = *SuperResult;
                        bFoundInSuperFuncMap = true;
                    }
                }

                if (!bFoundInSuperFuncMap)
                {
                    for (const FImplementedInterface& Inter : Class->GetInterfaces())
                    {
                        Result = Inter.Class ? FindFunctionByName(Class, InName) : nullptr;
                        if (Result)
                        {
                            break;
                        }
                    }

                    if (SuperClass && Result == nullptr)
                    {
                        Result = FindFunctionByName(SuperClass, InName);
                    }

                    Class->GetSuperFuncMap().Add(InName, Result);
                }
            }
        }
        return Result;
    };
    ProcessEvent(FindFunctionByName(GetClassPrivate(), FName(STR("InitializeWidget"))), nullptr);
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
    CastContext->PlayerParam[0] = FCharaSelectPlayerParam();
    CastContext->PlayerParam[1] = FCharaSelectPlayerParam();
}

void AREDGameState_CharaSelectRE::InitializeClass()
{
    Data.ClassDefaultObjectPath = FString(STR("/Script/REDExtend.Default__REDGameState_CharaSelectRE"));
    Data.SuperStruct = FString(STR("/Script/RED.REDGameState"));

    CREATE_COMPLEX_STATIC_ARRAY_PROPERTY(FCharaSelectPlayerParam, AREDGameState_CharaSelectRE, PlayerParam,
                            CPF_NativeAccessSpecifierPublic,
                            FString(STR("/Script/REDExtend.CharaSelectPlayerParam")), 2)
    {
        PARAM_BEGIN()
        CREATE_PARAM(const int32, Side, CPF_None)
        CREATE_COMPLEX_PARAM(FCharaSelectPlayerParam, ReturnValue, CPF_ReturnParm,
                             FString(STR("/Script/REDExtend.CharaSelectPlayerParam")))
        CREATE_FUNCTION(AREDGameState_CharaSelectRE, execGetPlayerParam,
                        FUNC_Public | FUNC_BlueprintCallable | FUNC_BlueprintPure,
                        FString(STR("/Script/REDExtend.REDGameState_CharaSelectRE:GetPlayerParam")), Params)
    }
    {
        PARAM_BEGIN()
        CREATE_PARAM(const int32, Side, CPF_None)
        CREATE_COMPLEX_PARAM(FCharaSelectPlayerParam, Param, CPF_None,
                             FString(STR("/Script/REDExtend.CharaSelectPlayerParam")))
        CREATE_FUNCTION(AREDGameState_CharaSelectRE, execSetPlayerParam,
                        FUNC_Public | FUNC_BlueprintCallable | FUNC_BlueprintPure,
                        FString(STR("/Script/REDExtend.REDGameState_CharaSelectRE:SetPlayerParam")), Params)
    }
    CREATE_EVENT(FUNC_Public | FUNC_BlueprintEvent,
                 FString(STR("/Script/REDExtend.REDGameState_CharaSelectRE:InitializeWidget")), {})
}

void AREDGameState_CharaSelectRE::InitializeVTable()
{
    CLONE_VTABLE(VTable, 0x105)

    ASSIGN_TO_VTABLE(VTable, 0x65, AREDGameState_CharaSelectRE, BeginPlay_Func, BeginPlay)
    ASSIGN_TO_VTABLE(VTable, 0xD8, AREDGameState_CharaSelectRE, HandleMatchIsWaitingToStart_Func,
                     HandleMatchIsWaitingToStart)
    ASSIGN_TO_VTABLE(VTable, 0xE3, AREDGameState_CharaSelectRE, SceneInitialize_Func, SceneInitialize)
}
