#pragma once

#include "AActor.hpp"
#include "Enums.h"
#include "REDGameState.h"
#include "struct_util.h"
#include "../../SuzieMod/include/Suzie.hpp"

using namespace RC::Unreal;

using CharType = wchar_t;
using StringType = std::basic_string<CharType>;

enum ECharaState
{
    CharaState_First,
    CharaState_Second,
    CharaState_Third,
    CharaState_ColorCostume,
    CharaState_Ready,
};

extern FDynamicEnum ECharaState_Data;
void ECharaState_InitializeEnum();

struct FDecideInfo
{
    TEnumAsByte<ECharaID> CharaID{};
    TEnumAsByte<EColorID> ColorID{};
    TEnumAsByte<ECostumeID> CostumeID{};
    TEnumAsByte<EBattleScript> ScriptID{};
    uint8 SpFlag{};
    int32 SkillSetIndex{};
    
    static void InitializeStruct();
    
    static inline FDynamicScriptStruct Data;
};

struct FSideInfo
{
    SIDE_ID SideID{};
    PAD_ID ControlPad{};
    FDecideInfo DecideInfo{};
    bool bSelectCPUChara{};
    int32 Page{};
};

struct FDecideInfoHistory
{
    FDecideInfo CharaHistory1P{};
    FDecideInfo CharaHistory2P{};
    bool IsValid{};
    SIDE_ID MainSide{};
};

struct FVector2D_Old
{
    float X;
    float Y;
};

struct FCharaSelectPlayerParam
{
    int32 Cursor{};
    FVector2D_Old CursorPos{};
    int32 RandomCounter{};
    bool bRandomSelect{};
    int32 PrevCursor{};
    FDecideInfo DecideInfo[3]{};
    int TeamSelectIndex{};
    int CustomCursor{};
    bool bSelectCPU{};
    TEnumAsByte<ECharaState> State{};
    TEnumAsByte<EBattleStage> StageID = BATTLE_STAGE_AUTO;
    EBGMID BGMID = EBGMID::BGMID_AUTO;
    bool bReady{};

    static void InitializeStruct();
    
    static inline FDynamicScriptStruct Data;
};

class AREDGameState_CharaSelectRE : public AREDGameState
{
    DECLARE_EXTERNAL_OBJECT_CLASS(AREDGameState_CharaSelectRE, REDExtend)
    
public:
    FCharaSelectPlayerParam PlayerParam[2];
    
    static void execGetPlayerParam(UObject* Context, FFrame& Stack, void* Z_Param__Result);
    FCharaSelectPlayerParam GetPlayerParam(const int32 Side) const;
    
    static void execSetPlayerParam(UObject* Context, FFrame& Stack, void* Z_Param__Result);
    void SetPlayerParam(const int32 Side, FCharaSelectPlayerParam Param);
    
    void InitializeWidget();

private:
    bool bStartedOnlineTraining;
    
public:
    void BeginPlay();
    void HandleMatchIsWaitingToStart();
    void SceneInitialize();
    
public:
    AActor* GetHUD();
    
    static void AREDGameState_CharaSelectRE_Ctor(const FObjectInitializer* ObjectInitializer);
    static void InitializeClass();
    void InitializeVTable();

    static inline RC::Function<void(UObject*)> OnStartedLobbyTraining_Func;
    
    static FDynamicClass Data;

private:
    static inline uintptr_t VTable[0x105];
};