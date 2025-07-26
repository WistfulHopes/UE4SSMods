#include <polyhook2/Detour/x64Detour.hpp>
#include <polyhook2/Exceptions/AVehHook.hpp>
#include <DynamicOutput/Output.hpp>
#include <Mod/CppUserModBase.hpp>
#include <UE4SSProgram.hpp>

#include "AActor.hpp"
#include "safetyhook.hpp"
#include "struct_util.h"
#include "UObject.hpp"
#include "SigScanner/SinglePassSigScanner.hpp"

using namespace RC::Unreal;

// Mod

auto patch_exe_bytes(uintptr_t address, PBYTE patch, int size) -> void
{
    DWORD old;
    bool success = VirtualProtect((LPVOID)address, size, PAGE_READWRITE, &old);
    if (success)
    {
        memcpy((void*)address, patch, size);

        VirtualProtect((LPVOID)address, size, old, &old);
    }
}

enum ID_CMNACT : uint32_t
{
    ID_CmnActStand = 0x0,
    ID_CmnActStandTurn = 0x1,
    ID_CmnActStand2Crouch = 0x2,
    ID_CmnActCrouch = 0x3,
    ID_CmnActCrouchTurn = 0x4,
    ID_CmnActCrouch2Stand = 0x5,
    ID_CmnActJumpPre = 0x6,
    ID_CmnActJump = 0x7,
    ID_CmnActJumpLanding = 0x8,
    ID_CmnActLandingStiff = 0x9,
    ID_CmnActFWalk = 0xA,
    ID_CmnActBWalk = 0xB,
    ID_CmnActFDash = 0xC,
    ID_CmnActFDashStop = 0xD,
    ID_CmnActBDash = 0xE,
    ID_CmnActBDashStop = 0xF,
    ID_CmnActAirFDash = 0x10,
    ID_CmnActAirBDash = 0x11,
    ID_CmnActNokezoriHighLv1 = 0x12,
    ID_CmnActNokezoriHighLv2 = 0x13,
    ID_CmnActNokezoriHighLv3 = 0x14,
    ID_CmnActNokezoriHighLv4 = 0x15,
    ID_CmnActNokezoriHighLv5 = 0x16,
    ID_CmnActNokezoriLowLv1 = 0x17,
    ID_CmnActNokezoriLowLv2 = 0x18,
    ID_CmnActNokezoriLowLv3 = 0x19,
    ID_CmnActNokezoriLowLv4 = 0x1A,
    ID_CmnActNokezoriLowLv5 = 0x1B,
    ID_CmnActNokezoriCrouchLv1 = 0x1C,
    ID_CmnActNokezoriCrouchLv2 = 0x1D,
    ID_CmnActNokezoriCrouchLv3 = 0x1E,
    ID_CmnActNokezoriCrouchLv4 = 0x1F,
    ID_CmnActNokezoriCrouchLv5 = 0x20,
    ID_CmnActBDownUpper = 0x21,
    ID_CmnActBDownUpperEnd = 0x22,
    ID_CmnActBDownDown = 0x23,
    ID_CmnActBDownBound = 0x24,
    ID_CmnActBDownLoop = 0x25,
    ID_CmnActBDown2Stand = 0x26,
    ID_CmnActFDownUpper = 0x27,
    ID_CmnActFDownUpperEnd = 0x28,
    ID_CmnActFDownDown = 0x29,
    ID_CmnActFDownBound = 0x2A,
    ID_CmnActFDownLoop = 0x2B,
    ID_CmnActFDown2Stand = 0x2C,
    ID_CmnActVDownUpper = 0x2D,
    ID_CmnActVDownUpperEnd = 0x2E,
    ID_CmnActVDownDown = 0x2F,
    ID_CmnActVDownBound = 0x30,
    ID_CmnActVDownLoop = 0x31,
    ID_CmnActBlowoff = 0x32,
    ID_CmnActBlowoffUpper90 = 0x33,
    ID_CmnActBlowoffUpper60 = 0x34,
    ID_CmnActBlowoffUpper30 = 0x35,
    ID_CmnActBlowoffDown30 = 0x36,
    ID_CmnActBlowoffDown60 = 0x37,
    ID_CmnActBlowoffDown90 = 0x38,
    ID_CmnActKirimomiUpper = 0x39,
    ID_CmnActWallBound = 0x3A,
    ID_CmnActWallBoundDown = 0x3B,
    ID_CmnActWallHaritsuki = 0x3C,
    ID_CmnActWallHaritsukiLand = 0x3D,
    ID_CmnActWallHaritsukiGetUp = 0x3E,
    ID_CmnActJitabataLoop = 0x3F,
    ID_CmnActKizetsu = 0x40,
    ID_CmnActHizakuzure = 0x41,
    ID_CmnActKorogari = 0x42,
    ID_CmnActZSpin = 0x43,
    ID_CmnActFuttobiFinish = 0x44,
    ID_CmnActFuttobiBGTrans = 0x45,
    ID_CmnActUkemi = 0x46,
    ID_CmnActLandUkemi = 0x47,
    ID_CmnActVUkemi = 0x48,
    ID_CmnActFUkemi = 0x49,
    ID_CmnActBUkemi = 0x4A,
    ID_CmnActKirimomiLand = 0x4B,
    ID_CmnActKirimomiLandEnd = 0x4C,
    ID_CmnActSlideDown = 0x4D,
    ID_CmnActRushFinishDown = 0x4E,
    ID_CmnActKirimomiVert = 0x4F,
    ID_CmnActKirimomiVertEnd = 0x50,
    ID_CmnActKirimomiSide = 0x51,
    ID_CmnActMidGuardPre = 0x52,
    ID_CmnActMidGuardLoop = 0x53,
    ID_CmnActMidGuardEnd = 0x54,
    ID_CmnActHighGuardPre = 0x55,
    ID_CmnActHighGuardLoop = 0x56,
    ID_CmnActHighGuardEnd = 0x57,
    ID_CmnActCrouchGuardPre = 0x58,
    ID_CmnActCrouchGuardLoop = 0x59,
    ID_CmnActCrouchGuardEnd = 0x5A,
    ID_CmnActAirGuardPre = 0x5B,
    ID_CmnActAirGuardLoop = 0x5C,
    ID_CmnActAirGuardEnd = 0x5D,
    ID_CmnActHajikareStand = 0x5E,
    ID_CmnActHajikareCrouch = 0x5F,
    ID_CmnActHajikareAir = 0x60,
    ID_CmnActAirTurn = 0x61,
    ID_CmnActLockWait = 0x62,
    ID_CmnActLockReject = 0x63,
    ID_CmnActAirLockWait = 0x64,
    ID_CmnActAirLockReject = 0x65,
    ID_CmnActItemUse = 0x66,
    ID_CmnActBurst = 0x67,
    ID_CmnActRomanCancel = 0x68,
    ID_CmnActEntry = 0x69,
    ID_CmnActRoundWin = 0x6A,
    ID_CmnActMatchWin = 0x6B,
    ID_CmnActLose = 0x6C,
    ID_CmnActResultWin = 0x6D,
    ID_CmnActResultLose = 0x6E,
    ID_CmnActEntryWait = 0x6F,
    ID_CmnActSubEntry = 0x70,
    ID_CmnActSpecialFinishWait = 0x71,
    ID_CmnActExDamage = 0x72,
    ID_CmnActExDamageLand = 0x73,
    ID_CmnActHide = 0x74,
    ID_CmnActChangeEnter = 0x75,
    ID_CmnActChangeEnterCutscene = 0x76,
    ID_CmnActChangeEnterCutsceneRecv = 0x77,
    ID_CmnActChangeEnterAttack = 0x78,
    ID_CmnActChangeEnterStiff = 0x79,
    ID_CmnActChangeLeave = 0x7A,
    ID_CmnActEnterAfterDestruction = 0x7B,
    ID_CmnActEnterAfterBGTransLeftIn = 0x7C,
    ID_CmnActEnterAfterBGTransRightIn = 0x7D,
    ID_CmnActRushStart = 0x7E,
    ID_CmnActRushRush = 0x7F,
    ID_CmnActRushFinish = 0x80,
    ID_CmnActRushFinishChaseLand = 0x81,
    ID_CmnActRushFinishChaseAir = 0x82,
    ID_CmnActRushFinishChaseEnd = 0x83,
    ID_CmnActRushFinishChange = 0x84,
    ID_CmnActRushSousai = 0x85,
    ID_CmnActRushSousaiPrimary = 0x86,
    ID_CmnActHomingDash = 0x87,
    ID_CmnActHomingDashCurve = 0x88,
    ID_CmnActHomingDashBrake = 0x89,
    ID_CmnActMikiwameMove = 0x8A,
    ID_CmnActShotHajiki = 0x8B,
    ID_CmnActGuardCancelAttack = 0x8C,
    ID_CmnActLimitBurst = 0x8D,
    ID_CmnActSparkingBurst = 0x8E,
    ID_CmnActRushRejectWait = 0x8F,
    ID_CmnActRushFinishDamage = 0x90,
    ID_CmnActQuickDown = 0x91,
    ID_CmnActQuickDown2Stand = 0x92,
    ID_CmnActNokezoriBottomLv1 = 0x93,
    ID_CmnActNokezoriBottomLv2 = 0x94,
    ID_CmnActNokezoriBottomLv3 = 0x95,
    ID_CmnActNokezoriBottomLv4 = 0x96,
    ID_CmnActNokezoriBottomLv5 = 0x97,
    ID_CmnActFloatDamage = 0x98,
    ID_CmnActEntryToStand1P = 0x99,
    ID_CmnActEntryToStand2P = 0x9A,
    ID_CmnActGuardLanding = 0x9B,
    ID_CmnAct_NUM = 0x9C,
    ID_CmnAct_NULL = 0xFFFFFFFF,
};

class BATTLE_TeamManager
{
    char pad[0x190];

public:
    FIELD(0x8, class OBJ_CCharBase*, m_MainPlayer);
};

class BATTLE_CObjectManager
{
public:
    ARRAY_FIELD(0x0, BATTLE_TeamManager[2], m_TeamManager);
};

class C32BYTE
{
    char str[0x20]{};

public:
    void set(const char* other)
    {
        strncpy(str, other, 0x20);
    }
};

class CAtkParam
{
    char pad[0x410];

public:
    FIELD(0x0, int, atk_type);
    FIELD(0x4, int, atk_level);
    FIELD(0x8, int, atk_level_clash);
    FIELD(0xC, int, damage);
    FIELD(0x14, int, m_AtkFlag2);
    FIELD(0x18, int, m_AtkFlag3);
    FIELD(0x1C, int, m_AtkFlag4);
    FIELD(0x24, int*, hitstop_enemy_addition);
    FIELD(0x30, int, hitstop);
    FIELD(0x34, int, grab_wait_time);
    FIELD(0x38, int, guard_time);
    FIELD(0x54, C32BYTE, m_AtkLockAction);
    FIELD(0x8C, C32BYTE, m_AtkNextHitActionName);
    FIELD(0x108, int, stun_damage);
    FIELD(0x12C, int, pushback);
    FIELD(0x130, int, fd_min_pushback);
    FIELD(0x134, int, guard_break_pushback);
    FIELD(0x138, int, hit_pushback);
    FIELD(0x13C, int, wall_pushback);
    FIELD(0x14C, int, stagger_time);
    FIELD(0x254, int, atk_level_guard);
    FIELD(0x258, int, risc);
    FIELD(0x25C, int, proration);
    FIELD(0x260, int, proration_rate_first);
    FIELD(0x264, int, proration_rate_once);
    FIELD(0x268, int, proration_first);
    FIELD(0x26C, int, proration_once);
    FIELD(0x270, int, chip_damage);
    FIELD(0x274, int, chip_damage_rate);
    FIELD(0x278, int, unburst_time);
    FIELD(0x304, int, guard_crush_time);
};

class CAtkParamEx
{
    char pad[0xC0];

public:
    FIELD(0x14, int, m_AtkLandStunTime);
    FIELD(0x18, int, m_AtkAirStunTime);
};

class CInterruptInfo
{
public:
    char* m_BeginAddr;
    char m_ActionName[0x20];
    int m_IIParam0;
    int m_IIParam1;
    char m_GotoLabel[0x20];
    uint32 m_IntFlg;
};

class CSkillInfo
{
public:
    FIELD(0x1FC, uint8, m_SkillCategory);
};

class CActionRequestInfo
{
    char pad[0x50];

public:
    FIELD(0x20, int, m_RequestFlag);
};

class OBJ_CCharBase
{
public:
    FIELD(0x28, int, m_ObjTimer);
    FIELD(0x4C, int, m_SideID);
    FIELD(0x1A8, int, m_ObjFlag);
    FIELD(0x1B0, int, m_ObjFlag3);
    FIELD(0x1C4, int, m_ActionTime);
    FIELD(0x280, int, m_HitStopTime);
    FIELD(0x28C, int, m_HitStopTimeBySousai);
    FIELD(0x2C0, OBJ_CCharBase*, m_TargetObj);
    FIELD(0x3AC, int, m_PosY);
    FIELD(0x4C8, int, m_SpeedX);
    FIELD(0x4CC, int, m_SpeedY);
    FIELD(0x4D0, int, m_Gravity);
    FIELD(0x750, CAtkParam, m_AtkParam);
    FIELD(0xB60, CAtkParamEx, m_AtkParamNH);
    FIELD(0xC20, CAtkParamEx, m_AtkParamCH);
    FIELD(0xD00, CAtkParam, m_DmgParam);
    FIELD(0x1110, CAtkParamEx, m_DmgParamEx);
    FIELD(0x11DC, int, m_bIsCounterHit);
    FIELD(0x11EC, int, m_MutekiDagekiTime);
    ARRAY_FIELD(0x13D8, CInterruptInfo[106], m_InterruptInfo);
    FIELD(0x38B4, CActionRequestInfo, m_ActionRequestInfoReg);
    FIELD(0x62D8, unsigned int, m_PlayerFlag2);
    FIELD(0x62EC, unsigned int, m_CtrlDir);
    FIELD(0x62F0, unsigned int, m_EnableFlag);
    FIELD(0x62F4, unsigned int, m_Enable2Flag);
    FIELD(0x6310, unsigned int, m_GuardStopCount);
    FIELD(0x9AD8, int, m_StunCount);
    FIELD(0x9B10, int, ply_JumpSpeed);
    FIELD(0x9B4C, int, ply_Kizetsu);
    FIELD(0xC3F4, int, m_ComboCount);
    FIELD(0xC41C, int, m_ComboTimeWithOutAnten);
    FIELD(0xC474, bool, m_IsImperfectCombo);
    FIELD(0xC476, bool, m_UkemiMiss);
    FIELD(0xC488, int, m_KizetsuPoint);
    FIELD(0xC48C, int, m_KizetsuMax);
    FIELD(0xC490, int, m_KizetsuTime);
    FIELD(0xC494, int, m_KizetsuTimeMax);
    FIELD(0xC4DC, ID_CMNACT, m_CurCmnActionID);
    FIELD(0xCA00, int, m_ExKizetsu);
    FIELD(0xCA20, int, m_DustHomingTime);
    FIELD(0xCA24, int, m_DustHomingSubTime);
    FIELD(0xCA6C, int, m_GuardBalance);
    FIELD(0xF030, CAtkParam, m_AtkParamForGGThrowStack);
    FIELD(0xF440, CAtkParamEx, m_AtkParamCHForGGThrowStack);
    FIELD(0xF520, CAtkParamEx, m_AtkParamNHForGGThrowStack);
    FIELD(0x107F8, int, m_CPUUkemi);
    char pad[0xC4E0];
    uint8_t m_InpFlag[2][333];

    bool IsInHitstun();
    bool IsInBlockstun();
    bool IsDownBound();
    bool IsQuickDown1();
    bool IsQuickDown2();
    bool IsDownLoop();
    bool IsDown2Stand();
    bool IsKnockdown();
    bool IsRoll();
    bool IsStagger();
    bool IsGuardCrush();
    bool IsStunned();
};

class CSceneChangeCharaInfo
{
public:
    int m_CharaID{};
    int m_ColorID{};
    int m_ScriptType{};
    int m_CostumeID{};
    int m_Handicap{};
};

class CSceneChange
{
public:
    ARRAY_FIELD(0xC, CSceneChangeCharaInfo[3], m_scCharInfoP1);
    ARRAY_FIELD(0x48, CSceneChangeCharaInfo[3], m_scCharInfoP2);
};

bool OBJ_CCharBase::IsInHitstun()
{
    switch (m_CurCmnActionID)
    {
    case ID_CmnActNokezoriHighLv1:
    case ID_CmnActNokezoriHighLv2:
    case ID_CmnActNokezoriHighLv3:
    case ID_CmnActNokezoriHighLv4:
    case ID_CmnActNokezoriHighLv5:
    case ID_CmnActNokezoriLowLv1:
    case ID_CmnActNokezoriLowLv2:
    case ID_CmnActNokezoriLowLv3:
    case ID_CmnActNokezoriLowLv4:
    case ID_CmnActNokezoriLowLv5:
    case ID_CmnActNokezoriCrouchLv1:
    case ID_CmnActNokezoriCrouchLv2:
    case ID_CmnActNokezoriCrouchLv3:
    case ID_CmnActNokezoriCrouchLv4:
    case ID_CmnActNokezoriCrouchLv5:
    case ID_CmnActBDownUpper:
    case ID_CmnActBDownUpperEnd:
    case ID_CmnActBDownDown:
    case ID_CmnActBDownLoop:
    case ID_CmnActFDownUpper:
    case ID_CmnActFDownUpperEnd:
    case ID_CmnActFDownDown:
    case ID_CmnActFDownLoop:
    case ID_CmnActVDownUpper:
    case ID_CmnActVDownUpperEnd:
    case ID_CmnActVDownDown:
    case ID_CmnActVDownLoop:
    case ID_CmnActBlowoff:
    case ID_CmnActBlowoffUpper90:
    case ID_CmnActBlowoffUpper60:
    case ID_CmnActBlowoffUpper30:
    case ID_CmnActBlowoffDown30:
    case ID_CmnActBlowoffDown60:
    case ID_CmnActBlowoffDown90:
    case ID_CmnActKirimomiUpper:
    case ID_CmnActWallBound:
    case ID_CmnActWallBoundDown:
    case ID_CmnActWallHaritsuki:
    case ID_CmnActWallHaritsukiLand:
    case ID_CmnActWallHaritsukiGetUp:
    case ID_CmnActJitabataLoop:
    case ID_CmnActKizetsu:
    case ID_CmnActHizakuzure:
    case ID_CmnActKorogari:
    case ID_CmnActZSpin:
    case ID_CmnActFuttobiFinish:
    case ID_CmnActFuttobiBGTrans:
    case ID_CmnActUkemi:
    case ID_CmnActLandUkemi:
    case ID_CmnActVUkemi:
    case ID_CmnActFUkemi:
    case ID_CmnActBUkemi:
    case ID_CmnActKirimomiLand:
    case ID_CmnActKirimomiLandEnd:
    case ID_CmnActSlideDown:
    case ID_CmnActRushFinishDown:
    case ID_CmnActKirimomiVert:
    case ID_CmnActKirimomiVertEnd:
    case ID_CmnActKirimomiSide:
    case ID_CmnActLockWait:
    case ID_CmnActLockReject:
    case ID_CmnActAirLockWait:
    case ID_CmnActAirLockReject:
    case ID_CmnActExDamage:
    case ID_CmnActExDamageLand:
    case ID_CmnActRushRejectWait:
    case ID_CmnActNokezoriBottomLv1:
    case ID_CmnActNokezoriBottomLv2:
    case ID_CmnActNokezoriBottomLv3:
    case ID_CmnActNokezoriBottomLv4:
    case ID_CmnActNokezoriBottomLv5:
    case ID_CmnActFloatDamage:
    case ID_CmnActGuardLanding: return true;
    default: return false;
    }
}

bool OBJ_CCharBase::IsInBlockstun()
{
    switch (m_CurCmnActionID)
    {
    case ID_CmnActMidGuardPre:
    case ID_CmnActMidGuardLoop:
    case ID_CmnActMidGuardEnd:
    case ID_CmnActHighGuardPre:
    case ID_CmnActHighGuardLoop:
    case ID_CmnActHighGuardEnd:
    case ID_CmnActCrouchGuardPre:
    case ID_CmnActCrouchGuardLoop:
    case ID_CmnActCrouchGuardEnd:
    case ID_CmnActAirGuardPre:
    case ID_CmnActAirGuardLoop:
    case ID_CmnActAirGuardEnd: return true;
    default: return false;
    }
}

bool OBJ_CCharBase::IsDownBound()
{
    switch (m_CurCmnActionID)
    {
    case ID_CmnActBDownBound:
    case ID_CmnActFDownBound:
    case ID_CmnActVDownBound: return true;
    default: return false;
    }
}

bool OBJ_CCharBase::IsQuickDown1()
{
    return m_CurCmnActionID == ID_CmnActQuickDown;
}

bool OBJ_CCharBase::IsQuickDown2()
{
    return m_CurCmnActionID == ID_CmnActQuickDown2Stand;
}

bool OBJ_CCharBase::IsDownLoop()
{
    switch (m_CurCmnActionID)
    {
    case ID_CmnActBDownLoop:
    case ID_CmnActFDownLoop:
    case ID_CmnActVDownLoop: return true;
    default: return false;
    }
}

bool OBJ_CCharBase::IsDown2Stand()
{
    switch (m_CurCmnActionID)
    {
    case ID_CmnActBDown2Stand:
    case ID_CmnActFDown2Stand: return true;
    default: return false;
    }
}

bool OBJ_CCharBase::IsKnockdown()
{
    return IsDownBound() || IsQuickDown1() || IsQuickDown2() || IsDownLoop() || IsDown2Stand();
}

bool OBJ_CCharBase::IsRoll()
{
    return m_CurCmnActionID == ID_CmnActKorogari;
}

bool OBJ_CCharBase::IsStagger()
{
    return m_CurCmnActionID == ID_CmnActJitabataLoop;
}

bool OBJ_CCharBase::IsGuardCrush()
{
    switch (m_CurCmnActionID)
    {
    case ID_CmnActHajikareStand:
    case ID_CmnActHajikareCrouch:
    case ID_CmnActHajikareAir: return true;
    default: return false;
    }
}

bool OBJ_CCharBase::IsStunned()
{
    return IsInHitstun() || IsInBlockstun() || IsKnockdown() || IsRoll() || IsStagger() || IsGuardCrush();
}

struct FMemberCharaInfo
{
    bool bEnable;
    char CharaID;
    char ColorID;
    char ScriptType;
    char CostumeID;
    bool bAnotherStyle;
    bool bCPU;
    int CPULv;
};

class UREDBattlePlayerInfo : public UObject
{
public:
    ARRAY_FIELD(0x28, FMemberCharaInfo[3], m_MemberCharaInfoList);
};

class UREDGameCommon : public UObject
{
public:
    ARRAY_FIELD(0x4C8, UREDBattlePlayerInfo*[2], BattlePlayerInfo);
};

struct FSpawnPlayerInfo
{
    int SideID;
    int MemberID;
    FString ChaaraID;
    TArray<FString> SelfTeamMemberCharaID;
    TArray<FString> EnemyTeamMemberCharaID;
    TArray<FString> BGLayerNames;
    int CostumeID;
    int ColorID;
    int ScriptType;
    FString VoiceLoc;
    bool bREV2ModelFlag;
    const void* CostumeIDConvTable;
    const void* ColorIDConvTable;
    int CostumeIDConvTableNum;
    int ColorIDConvTableNum;
};

class Coda : public CppUserModBase
{
private:
    static inline SafetyHookInline UkemiCheck;
    static inline SafetyHookInline ExecuteDamage;
    static inline SafetyHookInline OnFrameStep;
    static inline SafetyHookInline IsCounterHitByGuardCounter;
    static inline SafetyHookInline GetAirStunTime;
    static inline SafetyHookInline GetSpawnPlayerInfoList;
    static inline SafetyHookInline SetDamageParam;
    static inline SafetyHookInline CalcAtkAirStunTime;
    static inline SafetyHookInline GetEnemyHitStopTime;
    static inline SafetyHookMid CmnActUkemi;
    static inline SafetyHookMid SetGuardBackSpeed_JG;
    static inline SafetyHookMid PlayerInitializeOnEasyReset;
    static inline SafetyHookMid HitCollision;
    static inline SafetyHookMid OnFrameStepForPlayer;
    static inline UREDGameCommon* GameCommon;
    static inline uintptr_t DBMTable;
    static inline uintptr_t CmnActUkemi_HookAddr;
    static inline uintptr_t PlayerInitializeOnEasyReset_HookAddr;
    static inline Function<bool(OBJ_CCharBase*, int, int)> IsTrgBtnX;
    static inline Function<void*()> TrainingMenu;
    static inline Function<bool(void*, int)> IsTrainingDummy;
    static inline Function<bool(OBJ_CCharBase*)> IsActionRequested;
    static inline Function<void(OBJ_CCharBase*, const char*)> ActionRequest;
    static inline Function<bool(OBJ_CCharBase*)> IsAir;
    static inline Function<void(OBJ_CCharBase*)> AtkAllDefault;
    static inline Function<void(OBJ_CCharBase*, const char*)> FuncCall;
    static inline Function<CSkillInfo*(OBJ_CCharBase*)> GetCurSkill;
    static inline Function<int(OBJ_CCharBase*)> GetPosX;
    static inline Function<uint32_t(OBJ_CCharBase*)> DirSign;

public:
    Coda()
    {
        DBMTable = 0;

        ModName = STR("Coda");
        ModVersion = STR("1.0");
        ModDescription = STR("Replaces DBM values");
        ModAuthors = STR("WistfulHopes");
        // Do not change this unless you want to target a UE4SS version
        // other than the one you're currently building with somehow.
        //ModIntendedSDKVersion = STR("2.6");
    }

    ~Coda() override
    {
        int WorldWidthScale = 1000;
        int ScreenZoomScale = 1000;
        int InstantBlockXPushback = 0;
        int InstantBlockXAirPushback = 0;
        int InstantBlockWindow = 2;

        patch_exe_bytes(DBMTable + 501 * 4, (PBYTE)&WorldWidthScale, 4);
        patch_exe_bytes(DBMTable + 505 * 4, (PBYTE)&ScreenZoomScale, 4);
        patch_exe_bytes(DBMTable + 174 * 4, (PBYTE)&InstantBlockXPushback, 4);
        patch_exe_bytes(DBMTable + 178 * 4, (PBYTE)&InstantBlockXAirPushback, 4);
        patch_exe_bytes(DBMTable + 176 * 4, (PBYTE)&InstantBlockWindow, 4);
    }

    auto on_update() -> void override
    {
    }

    static void get_spawn_player_info_list(TArray<FSpawnPlayerInfo>* SpawnPlayerInfoList, const void* pEasyCharaSelect)
    {
        if (!GameCommon)
        {
            GameCommon = reinterpret_cast<UREDGameCommon*>(UObjectGlobals::FindFirstOf(FName(STR("REDGameCommon"))));
        }

        GameCommon->BattlePlayerInfo[0]->m_MemberCharaInfoList[1].bEnable = true;
        GameCommon->BattlePlayerInfo[0]->m_MemberCharaInfoList[1].bCPU = true;
        GameCommon->BattlePlayerInfo[0]->m_MemberCharaInfoList[1].CharaID = 1;
        GameCommon->BattlePlayerInfo[0]->m_MemberCharaInfoList[2].bEnable = true;
        GameCommon->BattlePlayerInfo[0]->m_MemberCharaInfoList[2].bCPU = true;
        GameCommon->BattlePlayerInfo[0]->m_MemberCharaInfoList[2].CharaID = 2;
        GameCommon->BattlePlayerInfo[1]->m_MemberCharaInfoList[1].bEnable = true;
        GameCommon->BattlePlayerInfo[1]->m_MemberCharaInfoList[1].bCPU = true;
        GameCommon->BattlePlayerInfo[1]->m_MemberCharaInfoList[1].CharaID = 4;
        GameCommon->BattlePlayerInfo[1]->m_MemberCharaInfoList[2].bEnable = true;
        GameCommon->BattlePlayerInfo[1]->m_MemberCharaInfoList[2].bCPU = true;
        GameCommon->BattlePlayerInfo[1]->m_MemberCharaInfoList[2].CharaID = 5;

        GetSpawnPlayerInfoList.call(SpawnPlayerInfoList, pEasyCharaSelect);
    }

    static bool ukemi_check(OBJ_CCharBase* ctx)
    {
        //bool is_button_pressed = IsTrgBtnX(ctx, 3, 0) || IsTrgBtnX(ctx, 3, 1) || IsTrgBtnX(ctx, 3, 2) ||
        //    IsTrgBtnX(ctx, 3, 3);
        bool is_button_pressed = ctx->m_InpFlag[ctx->m_CtrlDir][1] || ctx->m_InpFlag[ctx->m_CtrlDir][10] || ctx->
            m_InpFlag[ctx->m_CtrlDir][19] || ctx->m_InpFlag[ctx->m_CtrlDir][28];
        bool can_ukemi = ctx->m_StunCount <= 0 && (ctx->m_Enable2Flag & 0x100000) != 0
            && !ctx->m_HitStopTime && !ctx->m_HitStopTimeBySousai;

        int32 ukemi_direction = 0;

        if (ctx->m_InpFlag[0][120])
        {
            ukemi_direction = 1;
        }
        else if (ctx->m_InpFlag[0][94])
        {
            ukemi_direction = -1;
        }

        if (GetPosX(ctx) > GetPosX(ctx->m_TargetObj))
        {
            ukemi_direction *= -1;
        }

        if (ctx->m_PlayerFlag2 & 0x1000)
        {
            is_button_pressed = std::rand() % 1000 < ctx->m_CPUUkemi;
            ukemi_direction = std::rand() % 4;
        }

        if (IsTrainingDummy(TrainingMenu(), ctx->m_SideID))
        {
            is_button_pressed = true;
        }

        if (can_ukemi)
        {
            if (is_button_pressed && !IsActionRequested(ctx))
            {
                if (ukemi_direction == 0)
                {
                    if (ctx->m_SpeedY < -8750 && ctx->m_PosY < 175000)
                    {
                        ctx->m_Gravity = 1925;
                        ctx->m_SpeedY = ctx->ply_JumpSpeed * 8 / 10;
                        ctx->m_MutekiDagekiTime = 17;
                        ActionRequest(ctx, "CmnActUkemi");
                    }
                    else
                    {
                        ctx->m_Gravity = 1925;
                        ctx->m_SpeedY = ctx->ply_JumpSpeed / 2;
                        ctx->m_MutekiDagekiTime = 17;
                        ActionRequest(ctx, "CmnActUkemi");
                    }
                }
                else if (ukemi_direction > 0)
                {
                    ctx->m_Gravity = 1925;
                    ctx->m_SpeedY = ctx->ply_JumpSpeed / 2;
                    ctx->m_SpeedX /= 2;
                    if (ctx->m_ObjFlag & 0x400000 || ctx->m_ObjFlag & 0x800000)
                    {
                        ctx->m_SpeedX += DirSign(ctx) * 22750;
                    }
                    else
                    {
                        ctx->m_SpeedX += DirSign(ctx) * 14000;
                    }
                    ctx->m_MutekiDagekiTime = 17;
                    ActionRequest(ctx, "CmnActUkemi");
                }
                else
                {
                    ctx->m_Gravity = 1925;
                    ctx->m_SpeedY = ctx->ply_JumpSpeed / 2;
                    ctx->m_SpeedX /= 2;
                    ctx->m_SpeedX -= DirSign(ctx) * 10500;
                    ctx->m_MutekiDagekiTime = 15;
                    ActionRequest(ctx, "CmnActUkemi");
                }
                return true;
            }

            ctx->m_UkemiMiss = true;
            ctx->m_IsImperfectCombo = true;
        }

        return false;
    }

    static void execute_damage(OBJ_CCharBase* ctx, OBJ_CCharBase* atObj, bool isAlreadyDamage)
    {
        ExecuteDamage.call<
            void, OBJ_CCharBase*, OBJ_CCharBase*, bool>(
            ctx, atObj, isAlreadyDamage);

        if (ctx->m_KizetsuTime) return;

        int ComboModifier;
        switch (ctx->m_ComboCount)
        {
        case 0:
        case 1:
            ComboModifier = 100;
            break;
        case 2:
            ComboModifier = 75;
            break;
        case 3:
            ComboModifier = 69;
            break;
        case 4:
            ComboModifier = 63;
            break;
        case 5:
            ComboModifier = 56;
            break;
        case 6:
            ComboModifier = 50;
            break;
        case 7:
            ComboModifier = 44;
            break;
        case 8:
            ComboModifier = 38;
            break;
        case 9:
            ComboModifier = 31;
            break;
        case 10:
            ComboModifier = 25;
            break;
        case 11:
            ComboModifier = 19;
            break;
        default:
            ComboModifier = 13;
            break;
        }
        int StateModifier = 100;
        if (ctx->m_bIsCounterHit)
            StateModifier = 200;
        if (ctx->IsKnockdown())
            StateModifier = 25;
        ctx->m_KizetsuPoint += (atObj->m_AtkParam.damage *
                StateModifier * ComboModifier / 100 / 100 - 5) *
            15 * 115 / 100
            / 100;
        if (ctx->m_KizetsuPoint > ctx->ply_Kizetsu)
        {
            ctx->m_KizetsuTime = 255;
            ctx->m_KizetsuTimeMax = 255;
            ctx->m_KizetsuPoint = 0;
        }
    }

    static void on_frame_step(OBJ_CCharBase* ctx)
    {
        if (ctx->m_KizetsuTime && ctx->m_CurCmnActionID == ID_CmnActKizetsu)
        {
            --ctx->m_KizetsuTime;
            if (ctx->m_InpFlag[0][2] || ctx->m_InpFlag[1][2])
            {
                ctx->m_KizetsuTime -= 1;
            }
            if (ctx->m_InpFlag[0][11] || ctx->m_InpFlag[1][11])
            {
                ctx->m_KizetsuTime -= 1;
            }
            if (ctx->m_InpFlag[0][20] || ctx->m_InpFlag[1][20])
            {
                ctx->m_KizetsuTime -= 1;
            }
            if (ctx->m_InpFlag[0][29] || ctx->m_InpFlag[1][29])
            {
                ctx->m_KizetsuTime -= 1;
            }
            if (ctx->m_InpFlag[0][71] || ctx->m_InpFlag[1][71])
            {
                ctx->m_KizetsuTime -= 1;
            }
            if (ctx->m_InpFlag[0][97] || ctx->m_InpFlag[1][97])
            {
                ctx->m_KizetsuTime -= 1;
            }
            if (ctx->m_InpFlag[0][123] || ctx->m_InpFlag[1][123])
            {
                ctx->m_KizetsuTime -= 1;
            }
            if (ctx->m_InpFlag[0][149] || ctx->m_InpFlag[1][149])
            {
                ctx->m_KizetsuTime -= 1;
            }
        }
        if (!ctx->IsStunned())
        {
            if (ctx->m_ObjTimer % 25 == 0)
            {
                ctx->m_KizetsuPoint -= 1;
            }
        }
        if (ctx->m_KizetsuTime <= 0) ctx->m_KizetsuTime = 0;
        if (ctx->m_KizetsuPoint <= 0) ctx->m_KizetsuPoint = 0;

        if (ctx->m_DustHomingTime)
        {
            ctx->m_DustHomingTime--;
        }
        if (ctx->m_DustHomingSubTime)
        {
            ctx->m_DustHomingSubTime--;
        }

        return OnFrameStep.call<void, OBJ_CCharBase*>(ctx);
    }

    static bool risc_counter(OBJ_CCharBase* ctx)
    {
        return ctx->m_GuardBalance >= 6400;
    }

    static int get_air_stun_time(int level)
    {
        const int AirStunTimeTable[5] = {10, 12, 14, 16, 18};

        if (level < 0) return AirStunTimeTable[0];
        if (level > 4) return AirStunTimeTable[4];
        return AirStunTimeTable[level];
    }

    static void set_damage_param(OBJ_CCharBase* ctx, OBJ_CCharBase* atObj)
    {
        SetDamageParam.call(ctx, atObj);

        if (ctx->m_DmgParam.atk_type != 3)
        {
            if ((ctx->m_DmgParam.m_AtkFlag2 & 0x10) == 0)
            {
                if (ctx->m_ComboTimeWithOutAnten > 1080)
                {
                    ctx->m_DmgParamEx.m_AtkLandStunTime = ctx->m_DmgParamEx.m_AtkLandStunTime * 50 / 100;
                    ctx->m_DmgParamEx.m_AtkAirStunTime = ctx->m_DmgParamEx.m_AtkAirStunTime * 10 / 100;
                }
                else if (ctx->m_ComboTimeWithOutAnten > 840)
                {
                    ctx->m_DmgParamEx.m_AtkAirStunTime = ctx->m_DmgParamEx.m_AtkAirStunTime * 60 / 100;
                }
                else if (ctx->m_ComboTimeWithOutAnten > 600)
                {
                    ctx->m_DmgParamEx.m_AtkAirStunTime = ctx->m_DmgParamEx.m_AtkAirStunTime * 70 / 100;
                }
                else if (ctx->m_ComboTimeWithOutAnten > 420)
                {
                    ctx->m_DmgParamEx.m_AtkAirStunTime = ctx->m_DmgParamEx.m_AtkAirStunTime * 80 / 100;
                }
                else if (ctx->m_ComboTimeWithOutAnten > 300)
                {
                    ctx->m_DmgParamEx.m_AtkAirStunTime = ctx->m_DmgParamEx.m_AtkAirStunTime * 90 / 100;
                }
                else if (ctx->m_ComboTimeWithOutAnten > 180)
                {
                    ctx->m_DmgParamEx.m_AtkAirStunTime = ctx->m_DmgParamEx.m_AtkAirStunTime * 95 / 100;
                }

                if (ctx->m_DmgParamEx.m_AtkLandStunTime < 1) ctx->m_DmgParamEx.m_AtkAirStunTime = 1;
                if (ctx->m_DmgParamEx.m_AtkAirStunTime < 1) ctx->m_DmgParamEx.m_AtkAirStunTime = 1;
            }
        }
    }

    static int calc_atk_air_stun_time(CAtkParamEx* ctx, uint32_t flag, CAtkParam* pAtkParam, CAtkParamEx* pCounterHit,
                                      int defVal)
    {
        auto result = ctx->m_AtkAirStunTime;
        if (result == INT_MAX) result = defVal;
        if ((flag & 1) == 0) return result;
        if (pCounterHit->m_AtkAirStunTime == INT_MAX) result *= 2;
        else result = pCounterHit->m_AtkAirStunTime;
        return result;
    }

    static int get_enemy_hit_stop_time(int level)
    {
        level = FMath::Clamp(level, 0, 5);
        switch (level)
        {
        case 0:
        default:
            return 0;
        case 1:
            return 2;
        case 2:
            return 4;
        case 3:
            return 8;
        case 4:
            return 12;
        case 5:
            return 14;
        }
    }

    auto on_unreal_init() -> void override
    {
        const SignatureContainer dbm_table_sig{
            {{.signature = "8B 0D ? ? ? ? 0F AF CF"}},
            [&](const SignatureContainer& self)
            {
                DBMTable = *reinterpret_cast<int*>(self.get_match_address() + 0x2) + (uintptr_t)self.get_match_address()
                    + 6 - 283 * 4;

                // Data modification
                int WorldWidthScale = 1000;
                int ScreenZoomScale = 1100;
                int InstantBlockXPushback = 500;
                int InstantBlockXAirPushback = 750;
                int InstantBlockWindow = 8;

                patch_exe_bytes(DBMTable + 501 * 4, (PBYTE)&WorldWidthScale, 4);
                patch_exe_bytes(DBMTable + 505 * 4, (PBYTE)&ScreenZoomScale, 4);
                patch_exe_bytes(DBMTable + 174 * 4, (PBYTE)&InstantBlockXPushback, 4);
                patch_exe_bytes(DBMTable + 178 * 4, (PBYTE)&InstantBlockXAirPushback, 4);
                patch_exe_bytes(DBMTable + 176 * 4, (PBYTE)&InstantBlockWindow, 4);

                return true;
            },
            [](SignatureContainer& self)
            {
            },
        };

        // Old Tech Mod
        const SignatureContainer ukemi_check_sig{
            {{.signature = "40 53 56 57 41 56 48 83 EC ? 48 8B 05 ? ? ? ? 48 8B F1"}},
            [&](const SignatureContainer& self)
            {
                UkemiCheck = safetyhook::create_inline(self.get_match_address(), &ukemi_check);
                return true;
            },
            [](SignatureContainer& self)
            {
            },
        };

        const SignatureContainer cmn_act_ukemi_sig{
            {{.signature = "83 B9 ? ? ? ? ? 76 ? 48 8D 15 ? ? ? ? E8"}},
            [&](const SignatureContainer& self)
            {
                CmnActUkemi_HookAddr = reinterpret_cast<uintptr_t>(self.get_match_address());
                CmnActUkemi = safetyhook::create_mid(self.get_match_address(), [](SafetyHookContext& ctx)
                {
                    if (reinterpret_cast<OBJ_CCharBase*>(ctx.rcx)->m_ActionTime < 21)
                        ctx.rip = CmnActUkemi_HookAddr +
                            0x15;
                });
                return true;
            },
            [](SignatureContainer& self)
            {
            },
        };

        // Stun Mod
        const SignatureContainer execute_damage_sig{
            {
                {
                    .signature =
                    "48 89 5C 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 ? ? ? ? 8B A9"
                }
            },
            [&](const SignatureContainer& self)
            {
                ExecuteDamage = safetyhook::create_inline(self.get_match_address(), &execute_damage);
                return true;
            },
            [](SignatureContainer& self)
            {
            },
        };

        const SignatureContainer on_frame_step_sig{
            {
                {
                    .signature =
                    "48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 55 41 54 41 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 ? 4C 8B F1 E8 ? ? ? ? 41 FF 86"
                }
            },
            [&](const SignatureContainer& self)
            {
                OnFrameStep = safetyhook::create_inline(self.get_match_address(), on_frame_step);
                return true;
            },
            [](SignatureContainer& self)
            {
            },
        };

        // RISC Counter Mod
        const SignatureContainer risc_counter_sig{
            {{.signature = "81 B9 ? ? ? ? ? ? ? ? 0F 9D C0"}},
            [&](const SignatureContainer& self)
            {
                IsCounterHitByGuardCounter = safetyhook::create_inline(self.get_match_address(), risc_counter);
                return true;
            },
            [](SignatureContainer& self)
            {
            },
        };

        // RISC Counter Mod
        const SignatureContainer get_air_stun_time_sig{
            {
                {
                    .signature =
                    "85 C9 79 ? 33 C0 48 8D 0D ? ? ? ? 48 98 8B 04 81 C3 B8 ? ? ? ? 3B C8 0F 4C C1 48 8D 0D ? ? ? ? 48 98 8B 04 81 C3 CC CC CC CC CC CC 85 C9 79 ? 33 C0 48 8D 0D ? ? ? ? 48 98 8B 04 81 C3 B8 ? ? ? ? 3B C8 0F 4C C1 48 8D 0D ? ? ? ? 48 98 8B 04 81 C3 CC CC CC CC CC CC 48 89 5C 24"
                }
            },
            [&](const SignatureContainer& self)
            {
                GetAirStunTime = safetyhook::create_inline(self.get_match_address(), get_air_stun_time);
                return true;
            },
            [](SignatureContainer& self)
            {
            },
        };

        // Instant Block Stun Mod
        const SignatureContainer set_guard_back_speed_jg_sig{
            {{.signature = "89 91 ? ? ? ? 40 84 ED"}},
            [&](const SignatureContainer& self)
            {
                SetGuardBackSpeed_JG = safetyhook::create_mid(self.get_match_address(), [](SafetyHookContext& ctx)
                {
                    int groundJG[5] = {2, 3, 3, 4, 4};
                    int airJG[5] = {6, 6, 6, 7, 8};
                    auto obj = reinterpret_cast<OBJ_CCharBase*>(ctx.rcx);

                    int* jg = *(bool*)&ctx.rbp ? airJG : groundJG;

                    if (obj->m_DmgParam.atk_level <= 0) obj->m_GuardStopCount -= jg[0];
                    else if (obj->m_DmgParam.atk_level >= 4) obj->m_GuardStopCount -= jg[4];
                    else obj->m_GuardStopCount -= jg[obj->m_DmgParam.atk_level];
                });
                return true;
            },
            [](SignatureContainer& self)
            {
            },
        };

        // Proximity Throw
        const SignatureContainer hit_collision_sig{
            {
                {
                    .signature = "33 D2 48 8B CE E8 ? ? ? ? 8D 55"
                }
            },
            [&](const SignatureContainer& self)
            {
                HitCollision = safetyhook::create_mid(self.get_match_address(), [](SafetyHookContext& ctx)
                {
                    for (int i = 0; i < 2; i++)
                    {
                        auto objMan = reinterpret_cast<BATTLE_CObjectManager*>(ctx.rsi);
                        auto player = objMan->m_TeamManager[i].m_MainPlayer;

                        player->m_PlayerFlag2 &= ~0x80;

                        if (player->m_PlayerFlag2 & 0x40 && (player->m_PlayerFlag2 & 1) == IsAir(player))
                        {
                            player->m_PlayerFlag2 |= 0x80;
                            player->m_AtkParamForGGThrowStack = player->m_AtkParam;
                            player->m_AtkParamCHForGGThrowStack = player->m_AtkParamCH;
                            player->m_AtkParamNHForGGThrowStack = player->m_AtkParamNH;
                            AtkAllDefault(player);

                            if (IsAir(player))
                            {
                                FuncCall(player, "cmnAtkTemplNageAir");
                            }
                            else
                            {
                                FuncCall(player, "cmnAtkTemplNageLand");
                            }

                            player->m_AtkParam.m_AtkFlag3 |= 1;
                            player->m_AtkParam.m_AtkNextHitActionName.set("");
                            player->m_AtkParam.m_AtkFlag4 |= 0x10;
                            player->m_AtkParam.atk_type = 1;

                            if (IsAir(player))
                            {
                                FuncCall(player, "_ThrowGripParamAir");
                            }
                            else
                            {
                                FuncCall(player, "_ThrowGripParamLand");
                            }

                            player->m_InterruptInfo[8].m_IntFlg |= 1;
                            player->m_InterruptInfo[9].m_IntFlg |= 1;
                            player->m_InterruptInfo[10].m_IntFlg |= 1;
                            player->m_InterruptInfo[52].m_IntFlg |= 1;
                            player->m_InterruptInfo[60].m_IntFlg |= 1;
                        }
                    }
                });
                return true;
            },
            [](SignatureContainer& self)
            {
            },
        };

        const SignatureContainer on_frame_step_for_player_sig{
            {
                {
                    .signature = "45 33 ED 45 39 AE ? ? ? ? 0F 8D"
                }
            },
            [&](const SignatureContainer& self)
            {
                OnFrameStepForPlayer = safetyhook::create_mid(self.get_match_address(), [](SafetyHookContext& ctx)
                {
                    auto player = reinterpret_cast<OBJ_CCharBase*>(ctx.r14);
                    player->m_PlayerFlag2 &= ~0x40;
                    player->m_PlayerFlag2 &= ~0x1;
                    player->m_PlayerFlag2 &= ~0x800;

                    bool input_success = player->m_InpFlag[player->m_CtrlDir][31] && (player->m_InpFlag[player->
                        m_CtrlDir][120] || player->m_InpFlag[player->m_CtrlDir][94]);

                    if (player->m_PlayerFlag2 & 0x1000)
                    {
                        input_success = !(std::rand() % 8);
                    }

                    if (input_success)
                    {
                        auto success = false;
                        if (player->m_ActionTime <= 1 && GetCurSkill(player) && GetCurSkill(player)->m_SkillCategory ==
                            17)
                            success = true;
                        if (player->m_ExKizetsu) success = false;
                        if (player->m_PlayerFlag2 & 0x10000 || player->m_PlayerFlag2 & 0x20000 || player->m_ObjFlag3 &
                            0x10000)
                            success = false;
                        if (player->m_HitStopTime || player->m_HitStopTimeBySousai) success = false;
                        if (IsAir(player) && player->m_PlayerFlag2 & 0x400000) success = false;

                        if (success)
                        {
                            player->m_PlayerFlag2 |= 0x40;
                            player->m_PlayerFlag2 |= 0x80000000;
                            if (IsAir(player))
                            {
                                player->m_PlayerFlag2 |= 1;
                            }

                            if (player->m_InpFlag[player->m_CtrlDir][94])
                            {
                                player->m_PlayerFlag2 |= 0x100;
                                player->m_PlayerFlag2 &= ~0x200;
                            }
                            else
                            {
                                player->m_PlayerFlag2 &= ~0x100;
                                player->m_PlayerFlag2 |= 0x200;
                            }
                        }
                    }
                });
                return true;
            },
            [](SignatureContainer& self)
            {
            },
        };

        // Multiple Characters Mod
        const SignatureContainer multiple_characters_sig{
            {
                {
                    .signature =
                    "48 89 5C 24 ? 48 89 54 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 33 DB 4C 8B E9 48 89 5C 24"
                }
            },
            [&](const SignatureContainer& self)
            {
                // GetSpawnPlayerInfoList = safetyhook::create_inline(self.get_match_address(), get_spawn_player_info_list);
                return true;
            },
            [](SignatureContainer& self)
            {
            },
        };

        const SignatureContainer multiple_characters_2_sig{
            {
                {
                    .signature =
                    "0F 84 ? ? ? ? 48 8B 05 ? ? ? ? 48 85 C0 74 ? 48 8B B8 ? ? ? ? 48 85 FF 74 ? E8 ? ? ? ? 48 8B 57 ? 4C 8D 40 ? 48 63 40 ? 3B 42 ? 7F ? 48 8B C8 48 8B 42 ? 4C 39 04 C8 74 ? 33 FF 48 8B 87 ? ? ? ? 80 78"
                }
            },
            [&](const SignatureContainer& self)
            {
                PlayerInitializeOnEasyReset_HookAddr = (uintptr_t)self.get_match_address();
                PlayerInitializeOnEasyReset = safetyhook::create_mid(self.get_match_address(),
                                                                     [](SafetyHookContext& ctx)
                                                                     {
                                                                         if (*(char*)(ctx.rax + 0x4E) >= 35) return;
                                                                         ctx.rip = PlayerInitializeOnEasyReset_HookAddr
                                                                             + 0xE0;
                                                                     });
                return true;
            },
            [](SignatureContainer& self)
            {
            },
        };

        // Hitstun Mod
        const SignatureContainer set_damage_param_sig{
            {
                {
                    .signature =
                    "48 89 5C 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 44 24 ? 8B 81"
                }
            },
            [&](const SignatureContainer& self)
            {
                SetDamageParam = safetyhook::create_inline(self.get_match_address(), set_damage_param);
                return true;
            },
            [](SignatureContainer& self)
            {
            },
        };

        const SignatureContainer calc_atk_air_stun_time_sig{
            {
                {
                    .signature =
                    "44 8B 41 ? 41 81 F8 ? ? ? ? 44 0F 44 44 24 ? F6 C2 ? 74 ? 41 8B 41 ? 3D ? ? ? ? 41 0F 44 C0 C3 41 8B C0 C3 CC CC CC CC CC CC CC CC 8B 81"
                }
            },
            [&](const SignatureContainer& self)
            {
                CalcAtkAirStunTime = safetyhook::create_inline(self.get_match_address(), calc_atk_air_stun_time);
                return true;
            },
            [](SignatureContainer& self)
            {
            },
        };

        const SignatureContainer get_enemy_hit_stop_time_sig{
            {
                {
                    .signature =
                    "85 C9 79 ? 33 C0 48 8D 0D ? ? ? ? 48 98 8B 04 81 C3 B8 ? ? ? ? 3B C8 0F 4C C1 48 8D 0D ? ? ? ? 48 98 8B 04 81 C3 CC CC CC CC CC CC 85 C9 79 ? 33 C0 48 8D 0D ? ? ? ? 48 98 8B 04 81 C3 B8 ? ? ? ? 3B C8 0F 4C C1 48 8D 0D ? ? ? ? 48 98 8B 04 81 C3 CC CC CC CC CC CC 85 C9 79 ? 33 C0 48 8D 0D ? ? ? ? 48 98 8B 04 81 C3 B8 ? ? ? ? 3B C8 0F 4C C1 48 8D 0D ? ? ? ? 48 98 8B 04 81 C3 CC CC CC CC CC CC 85 C9 79 ? 33 C0 48 8D 0D ? ? ? ? 48 98 8B 04 81 C3 B8 ? ? ? ? 3B C8 0F 4C C1 48 8D 0D ? ? ? ? 48 98 8B 04 81 C3 CC CC CC CC CC CC 85 C9 79 ? 33 C0 48 8D 0D ? ? ? ? 48 98 8B 04 81 C3 B8 ? ? ? ? 3B C8 0F 4C C1 48 8D 0D ? ? ? ? 48 98 8B 04 81 C3 CC CC CC CC CC CC 85 C9 79 ? 33 C0 48 8D 0D ? ? ? ? 48 98 8B 04 81 C3 B8 ? ? ? ? 3B C8 0F 4C C1 48 8D 0D ? ? ? ? 48 98 8B 04 81 C3 CC CC CC CC CC CC 40 53"
                }
            },
            [&](const SignatureContainer& self)
            {
                GetEnemyHitStopTime = safetyhook::create_inline(self.get_match_address(), get_enemy_hit_stop_time);
                return true;
            },
            [](SignatureContainer& self)
            {
            },
        };

        // Helpers
        const SignatureContainer is_trg_btn_x_sig{
            {
                {
                    .signature =
                    "4C 8B 89 ? ? ? ? 4D 85 C9 74 ? 49 63 C0 41 39 94 81 ? ? ? ? 7F ? B0 ? C3 32 C0 C3 CC 4C 8B 89"
                }
            },
            [&](const SignatureContainer& self)
            {
                IsTrgBtnX.assign_address(self.get_match_address());
                return true;
            },
            [](SignatureContainer& self)
            {
            },
        };

        const SignatureContainer training_menu_sig{
            {
                {
                    .signature =
                    "48 8D 05 ? ? ? ? C3 CC CC CC CC CC CC CC CC 48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4"
                }
            },
            [&](const SignatureContainer& self)
            {
                TrainingMenu.assign_address(self.get_match_address());
                return true;
            },
            [](SignatureContainer& self)
            {
            },
        };

        const SignatureContainer is_training_dummy_sig{
            {
                {
                    .signature = "48 89 5C 24 ? 57 48 83 EC ? 48 8B F9 8B DA 48 8D 0D ? ? ? ? E8"
                }
            },
            [&](const SignatureContainer& self)
            {
                IsTrainingDummy.assign_address(self.get_match_address());
                return true;
            },
            [](SignatureContainer& self)
            {
            },
        };

        const SignatureContainer is_action_requested_sig{
            {
                {
                    .signature = "48 83 EC ? 8B 81 ? ? ? ? 84 C0"
                }
            },
            [&](const SignatureContainer& self)
            {
                IsActionRequested.assign_address(self.get_match_address());
                return true;
            },
            [](SignatureContainer& self)
            {
            },
        };

        const SignatureContainer action_request_sig{
            {
                {
                    .signature = "48 89 5C 24 ? 57 48 83 EC ? 80 B9 ? ? ? ? ? 48 8B FA 48 8B D9 74 ? F7 81"
                }
            },
            [&](const SignatureContainer& self)
            {
                ActionRequest.assign_address(self.get_match_address());
                return true;
            },
            [](SignatureContainer& self)
            {
            },
        };

        const SignatureContainer is_air_sig{
            {
                {
                    .signature = "48 83 EC ? F6 81 ? ? ? ? ? 75 ? E8 ? ? ? ? 85 C0 7F"
                }
            },
            [&](const SignatureContainer& self)
            {
                IsAir.assign_address(self.get_match_address());
                return true;
            },
            [](SignatureContainer& self)
            {
            },
        };

        const SignatureContainer atk_all_default_sig{
            {
                {
                    .signature =
                    "40 53 48 83 EC ? 48 8B D9 48 81 C1 ? ? ? ? E8 ? ? ? ? 48 8D 8B ? ? ? ? E8 ? ? ? ? 48 8D 8B ? ? ? ? 48 83 C4"
                }
            },
            [&](const SignatureContainer& self)
            {
                AtkAllDefault.assign_address(self.get_match_address());
                return true;
            },
            [](SignatureContainer& self)
            {
            },
        };

        const SignatureContainer func_call_sig{
            {
                {
                    .signature =
                    "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8B 05 ? ? ? ? 33 FF 48 8B EA"
                }
            },
            [&](const SignatureContainer& self)
            {
                FuncCall.assign_address(self.get_match_address());
                return true;
            },
            [](SignatureContainer& self)
            {
            },
        };

        const SignatureContainer get_cur_skill_sig{
            {
                {
                    .signature = "48 63 81 ? ? ? ? 83 F8 ? 74 ? 48 69 D0"
                }
            },
            [&](const SignatureContainer& self)
            {
                GetCurSkill.assign_address(self.get_match_address());
                return true;
            },
            [](SignatureContainer& self)
            {
            },
        };

        const SignatureContainer get_pos_x_sig{
            {
                {
                    .signature =
                    "48 8B 5C 24 ? 48 83 C4 ? 5F C3 48 8B 5C 24 ? 8B C7 48 83 C4 ? 5F C3 CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC E9"
                }
            },
            [&](const SignatureContainer& self)
            {
                GetPosX.assign_address(self.get_match_address() - 0x1A9);
                return true;
            },
            [](SignatureContainer& self)
            {
            },
        };

        const SignatureContainer dir_sign_sig{
            {
                {
                    .signature = "48 83 EC ? 48 8B 81 ? ? ? ? 48 85 C0 74 ? 48 8B C8 E8 ? ? ? ? 8B C8"
                }
            },
            [&](const SignatureContainer& self)
            {
                DirSign.assign_address(self.get_match_address());
                return true;
            },
            [](SignatureContainer& self)
            {
            },
        };

        std::vector<SignatureContainer> signature_containers;
        signature_containers.push_back(dbm_table_sig);
        signature_containers.push_back(ukemi_check_sig);
        signature_containers.push_back(cmn_act_ukemi_sig);
        // signature_containers.push_back(execute_damage_sig);
        signature_containers.push_back(on_frame_step_sig);
        signature_containers.push_back(risc_counter_sig);
        signature_containers.push_back(get_air_stun_time_sig);
        signature_containers.push_back(get_enemy_hit_stop_time_sig);
        signature_containers.push_back(set_damage_param_sig);
        signature_containers.push_back(calc_atk_air_stun_time_sig);
        signature_containers.push_back(set_guard_back_speed_jg_sig);
        // signature_containers.push_back(hit_collision_sig);
        // signature_containers.push_back(on_frame_step_for_player_sig);
        signature_containers.push_back(is_trg_btn_x_sig);
        signature_containers.push_back(training_menu_sig);
        signature_containers.push_back(is_training_dummy_sig);
        signature_containers.push_back(is_action_requested_sig);
        signature_containers.push_back(action_request_sig);
        signature_containers.push_back(is_air_sig);
        signature_containers.push_back(atk_all_default_sig);
        signature_containers.push_back(func_call_sig);
        signature_containers.push_back(get_cur_skill_sig);
        signature_containers.push_back(get_pos_x_sig);
        signature_containers.push_back(dir_sign_sig);
        // signature_containers.push_back(multiple_characters_sig);
        // signature_containers.push_back(multiple_characters_2_sig);

        SinglePassScanner::SignatureContainerMap signature_containers_map;
        signature_containers_map.emplace(ScanTarget::MainExe, signature_containers);

        SinglePassScanner::start_scan(signature_containers_map);
    }
};

#define CODA_API __declspec(dllexport)

extern "C" {
CODA_API CppUserModBase* start_mod()
{
    return new Coda();
}

CODA_API void uninstall_mod(CppUserModBase* mod)
{
    delete mod;
}
}
