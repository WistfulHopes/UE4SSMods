#include <polyhook2/Detour/x64Detour.hpp>
#include <polyhook2/Exceptions/AVehHook.hpp>
#include <PatternFinder.h>
#include <DynamicOutput/Output.hpp>
#include <Mod/CppUserModBase.hpp>
#include <UE4SSProgram.hpp>
#include "struct_util.h"

using namespace RC::Unreal;

// Mod

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

class CAtkParam
{
public:
    FIELD(0x0, int, atk_type);
    FIELD(0x4, int, atk_level);
    FIELD(0x8, int, atk_level_clash);
    FIELD(0xC, int, damage);
    FIELD(0x24, int*, hitstop_enemy_addition);
    FIELD(0x30, int, hitstop);
    FIELD(0x34, int, grab_wait_time);
    FIELD(0x38, int, guard_time);
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

class OBJ_CCharBase
{
public:
    FIELD(0x28, int, m_ObjTimer);
    FIELD(0x280, int, m_HitStopTime);
    FIELD(0x28C, int, m_HitStopTimeBySousai);
    FIELD(0x6F8, CAtkParam, m_AtkParam);
    FIELD(0xC78, CAtkParam, m_DmgParam);
    FIELD(0x1158, int, m_bIsCounterHit);
    FIELD(0x6084, unsigned int, m_Enable2Flag);
    FIELD(0x9868, int, m_StunCount);
    FIELD(0x9B8C, int, ply_Kizetsu);
    FIELD(0xC184, int, m_ComboCount);
    FIELD(0xC204, bool, m_IsImperfectCombo);
    FIELD(0xC206, bool, m_UkemiMiss);
    FIELD(0xC218, int, m_KizetsuPoint);
    FIELD(0xC21C, int, m_KizetsuMax);
    FIELD(0xC220, int, m_KizetsuTime);
    FIELD(0xC224, int, m_KizetsuTimeMax);
    FIELD(0xC26C, ID_CMNACT, m_CurCmnActionID);
    FIELD(0xC7FC, int, m_GuardBalance);
    char pad[0xC270];
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

bool (*UkemiCheck_Orig)(OBJ_CCharBase* pThis);
bool UkemiCheck_New(OBJ_CCharBase* pThis)
{
    if (!pThis->m_InpFlag[0][272] && !pThis->m_InpFlag[1][272])
    {
        if (pThis->m_StunCount <= 0 && (pThis->m_Enable2Flag & 0x100000) != 0
            && !pThis->m_HitStopTime && !pThis->m_HitStopTimeBySousai)
        {
            pThis->m_IsImperfectCombo = true;
            pThis->m_UkemiMiss = true;
        }
        return false;
    }
    return UkemiCheck_Orig(pThis);
}

void (*ExecuteDamage_Orig)(OBJ_CCharBase* pThis, OBJ_CCharBase* atObj, bool isAlreadyDamage);
void ExecuteDamage_New(OBJ_CCharBase* pThis, OBJ_CCharBase* atObj, bool isAlreadyDamage)
{
    ExecuteDamage_Orig(pThis, atObj, isAlreadyDamage);

    if (pThis->m_KizetsuTime) return;
    
    int ComboModifier;
    switch (pThis->m_ComboCount)
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
    if (pThis->m_bIsCounterHit)
        StateModifier = 200;
    if (pThis->IsKnockdown())
        StateModifier = 25;
    pThis->m_KizetsuPoint += (atObj->m_AtkParam.damage * StateModifier * ComboModifier / 100 / 100 - 5) * 15 * 115 / 100 / 100;
    if (pThis->m_KizetsuPoint > pThis->ply_Kizetsu)
    {
        pThis->m_KizetsuTime = 255;
        pThis->m_KizetsuTimeMax = 255;
        pThis->m_KizetsuPoint = 0;
    }
}

bool (*OnFrameStep_Old)(OBJ_CCharBase* pThis);
bool OnFrameStep_New(OBJ_CCharBase* pThis)
{
    if (pThis->m_KizetsuTime && pThis->m_CurCmnActionID == ID_CmnActKizetsu)
    {
        --pThis->m_KizetsuTime;
        if (pThis->m_InpFlag[0][2] || pThis->m_InpFlag[1][2])
        {
            pThis->m_KizetsuTime -= 1;
        }
        if (pThis->m_InpFlag[0][11] || pThis->m_InpFlag[1][11])
        {
            pThis->m_KizetsuTime -= 1;
        }
        if (pThis->m_InpFlag[0][20] || pThis->m_InpFlag[1][20])
        {
            pThis->m_KizetsuTime -= 1;
        }
        if (pThis->m_InpFlag[0][29] || pThis->m_InpFlag[1][29])
        {
            pThis->m_KizetsuTime -= 1;
        }
        if (pThis->m_InpFlag[0][71] || pThis->m_InpFlag[1][71])
        {
            pThis->m_KizetsuTime -= 1;
        }
        if (pThis->m_InpFlag[0][97] || pThis->m_InpFlag[1][97])
        {
            pThis->m_KizetsuTime -= 1;
        }
        if (pThis->m_InpFlag[0][123] || pThis->m_InpFlag[1][123])
        {
            pThis->m_KizetsuTime -= 1;
        }
        if (pThis->m_InpFlag[0][149] || pThis->m_InpFlag[1][149])
        {
            pThis->m_KizetsuTime -= 1;
        }
    }
    if (!pThis->IsStunned())
    {
        if (pThis->m_ObjTimer % 25 == 0)
        {
            pThis->m_KizetsuPoint -= 1;
        }
    }
    if (pThis->m_KizetsuTime <= 0) pThis->m_KizetsuTime = 0;
    if (pThis->m_KizetsuPoint <= 0) pThis->m_KizetsuPoint = 0;
    return OnFrameStep_Old(pThis);
};

bool (*IsCounterHitByGuardCounter_Old)(OBJ_CCharBase* pThis);
bool IsCounterHitByGuardCounter_New(OBJ_CCharBase* pThis)
{
    return pThis->m_GuardBalance >= 6400;
}

class DBMValueMod : public CppUserModBase
{
public:
    PLH::x64Detour* UkemiCheck_Detour;
    PLH::x64Detour* ExecuteDamage_Detour;
    PLH::x64Detour* OnFrameStep_Detour;
    PLH::x64Detour* IsCounterHitByGuardCounter_Detour;
    uintptr_t DBMTable;
    
    DBMValueMod()
    {
        UkemiCheck_Detour = nullptr;
        ExecuteDamage_Detour = nullptr;
        OnFrameStep_Detour = nullptr;
        IsCounterHitByGuardCounter_Detour = nullptr;
        DBMTable = 0;
        
        ModName = STR("DBMValueMod");
        ModVersion = STR("1.0");
        ModDescription = STR("Replaces DBM values");
        ModAuthors = STR("WistfulHopes");
        // Do not change this unless you want to target a UE4SS version
        // other than the one you're currently building with somehow.
        //ModIntendedSDKVersion = STR("2.6");
    }

    ~DBMValueMod() override
    {
        UkemiCheck_Detour->unHook();
        ExecuteDamage_Detour->unHook();
        OnFrameStep_Detour->unHook();
        IsCounterHitByGuardCounter_Detour->unHook();

        DWORD TechXFront;
        
        bool TechXFrontSuccess = VirtualProtect(reinterpret_cast<LPVOID>(DBMTable + 0x9F * 4), 0x4, PAGE_READWRITE, &TechXFront);

        if (TechXFrontSuccess)
        {
            *reinterpret_cast<int*>(DBMTable + 0x9F * 4) = 50000;

            VirtualProtect(reinterpret_cast<LPVOID>(DBMTable + 0x9F * 4), 0x4, TechXFront, &TechXFront);

            Output::send(STR("Successfully patched forward tech x!\n"));
        }
        else
        {
            Output::send(STR("Failed to unpatch forward tech x!\n"));
        }
        
        DWORD TechXBack;
        
        bool TechXBackSuccess = VirtualProtect(reinterpret_cast<LPVOID>(DBMTable + 0xA0 * 4), 0x4, PAGE_READWRITE, &TechXBack);

        if (TechXBackSuccess)
        {
            *reinterpret_cast<int*>(DBMTable + 0xA0 * 4) = 0;
            
            VirtualProtect(reinterpret_cast<LPVOID>(DBMTable + 0xA0 * 4), 0x4, TechXBack, &TechXBack);

            Output::send(STR("Successfully patched backward tech x!\n"));
        }
        else
        {
            Output::send(STR("Failed to unpatch backward tech x!\n"));
        }
        
        DWORD TechYBack;
        
        bool TechYBackSuccess = VirtualProtect(reinterpret_cast<LPVOID>(DBMTable + 0xA3 * 4), 0x4, PAGE_READWRITE, &TechYBack);

        if (TechYBackSuccess)
        {
            *reinterpret_cast<int*>(DBMTable + 0xA3 * 4) = 150;
            
            VirtualProtect(reinterpret_cast<LPVOID>(DBMTable + 0xA3 * 4), 0x4, TechYBack, &TechYBack);

            Output::send(STR("Successfully patched backward tech y!\n"));
        }
        else
        {
            Output::send(STR("Failed to unpatch backward tech y!\n"));
        }
        
        // old camera behavior
            
        DWORD ScreenZoomScale;
        
        bool ScreenZoomScaleSuccess = VirtualProtect(reinterpret_cast<LPVOID>(DBMTable + 0x1E3 * 4), 0x4, PAGE_READWRITE, &ScreenZoomScale);

        if (ScreenZoomScaleSuccess)
        {
            *reinterpret_cast<int*>(DBMTable + 0x1E3 * 4) = 1000;
            
            VirtualProtect(reinterpret_cast<LPVOID>(DBMTable + 0x1E3 * 4), 0x4, ScreenZoomScale, &ScreenZoomScale);
            
            Output::send(STR("Successfully patched screen zoom scale!\n"));
        }
        else
        {
            Output::send(STR("Failed to unpatch screen zoom scale!\n"));
        }
        
        // instant block window
            
        DWORD InstantBlockWindow;
        
        bool InstantBlockWindowSuccess = VirtualProtect(reinterpret_cast<LPVOID>(DBMTable + 0xAD * 4), 0x4, PAGE_READWRITE, &InstantBlockWindow);

        if (InstantBlockWindowSuccess)
        {
            *reinterpret_cast<int*>(DBMTable + 0xAD * 4) = 2;
            
            VirtualProtect(reinterpret_cast<LPVOID>(DBMTable + 0xAD * 4), 0x4, InstantBlockWindow, &InstantBlockWindow);
            
            Output::send(STR("Successfully patched instant block window!\n"));
        }
        else
        {
            Output::send(STR("Failed to unpatch instant block window!\n"));
        }
    }

    auto on_update() -> void override {}

    auto on_unreal_init() -> void override
    {
        HMODULE BaseModule = GetModuleHandleW(NULL);
        const uintptr_t Address = FindPattern(BaseModule, (PBYTE)"\x8B\x88\x98\x06\x00\x00\xB8\x56\x55\x55\x55", "xxxxxxxxxxx") + 0x1B;
        DBMTable = *reinterpret_cast<int*>(Address + 0x2) + Address + 6 - 103 * 4;
        
        // old tech
        const uintptr_t UkemiCheck_Address = FindPattern(BaseModule, (PBYTE)"\x40\x53\x56\x57\x41\x56\x48\x83\xEC\x38\x48\x8B\x05\x00\x00\x00\x00", "xxxxxxxxxxxxx????");
        
        UkemiCheck_Detour = new PLH::x64Detour(
            UkemiCheck_Address, reinterpret_cast<uint64_t>(&UkemiCheck_New),
            reinterpret_cast<uint64_t*>(&UkemiCheck_Orig));
        UkemiCheck_Detour->hook();

        const uintptr_t ExecuteDamage_Address = FindPattern(BaseModule, (PBYTE)"\x8D\x0C\xBF\xC1\xE1\x04", "xxxxxx")- 0x1EE;
        
        ExecuteDamage_Detour = new PLH::x64Detour(
            ExecuteDamage_Address, reinterpret_cast<uint64_t>(&ExecuteDamage_New),
            reinterpret_cast<uint64_t*>(&ExecuteDamage_Orig));
        ExecuteDamage_Detour->hook();

        const uintptr_t OnFrameStep_Address = FindPattern(BaseModule, (PBYTE)"\x75\x00\xE8\x00\x00\x00\x00\x44\x38\xA8\x84\x3C\x00\x00", "x?x????xxxxxxx")- 0x1247;
        
        OnFrameStep_Detour = new PLH::x64Detour(
            OnFrameStep_Address, reinterpret_cast<uint64_t>(&OnFrameStep_New),
            reinterpret_cast<uint64_t*>(&OnFrameStep_Old));
        OnFrameStep_Detour->hook();
        
        IsCounterHitByGuardCounter_Detour = new PLH::x64Detour(
            reinterpret_cast<uintptr_t>(BaseModule) + 0xBF4A80, reinterpret_cast<uint64_t>(&IsCounterHitByGuardCounter_New),
            reinterpret_cast<uint64_t*>(&IsCounterHitByGuardCounter_Old));
        IsCounterHitByGuardCounter_Detour->hook();
        
        DWORD TechXFront;
        
        bool TechXFrontSuccess = VirtualProtect(reinterpret_cast<LPVOID>(DBMTable + 0x9F * 4), 0x4, PAGE_READWRITE, &TechXFront);

        if (TechXFrontSuccess)
        {
            *reinterpret_cast<int*>(DBMTable + 0x9F * 4) = 25000;

            VirtualProtect(reinterpret_cast<LPVOID>(DBMTable + 0x9F * 4), 0x4, TechXFront, &TechXFront);

            Output::send(STR("Successfully patched forward tech x!\n"));
        }
        else
        {
            Output::send(STR("Failed to patch forward tech x!\n"));
        }
        
        DWORD TechXBack;
        
        bool TechXBackSuccess = VirtualProtect(reinterpret_cast<LPVOID>(DBMTable + 0xA0 * 4), 0x4, PAGE_READWRITE, &TechXBack);

        if (TechXBackSuccess)
        {
            *reinterpret_cast<int*>(DBMTable + 0xA0 * 4) = -25000;
            
            VirtualProtect(reinterpret_cast<LPVOID>(DBMTable + 0xA0 * 4), 0x4, TechXBack, &TechXBack);

            Output::send(STR("Successfully patched backward tech x!\n"));
        }
        else
        {
            Output::send(STR("Failed to patch backward tech x!\n"));
        }
        
        DWORD TechYBack;
        
        bool TechYBackSuccess = VirtualProtect(reinterpret_cast<LPVOID>(DBMTable + 0xA3 * 4), 0x4, PAGE_READWRITE, &TechYBack);

        if (TechYBackSuccess)
        {
            *reinterpret_cast<int*>(DBMTable + 0xA3 * 4) = 50;
            
            VirtualProtect(reinterpret_cast<LPVOID>(DBMTable + 0xA3 * 4), 0x4, TechYBack, &TechYBack);

            Output::send(STR("Successfully patched backward tech y!\n"));
        }
        else
        {
            Output::send(STR("Failed to patch backward tech y!\n"));
        }
        
        // old camera behavior
            
        DWORD ScreenZoomScale;
        
        bool ScreenZoomScaleSuccess = VirtualProtect(reinterpret_cast<LPVOID>(DBMTable + 0x1E3 * 4), 0x4, PAGE_READWRITE, &ScreenZoomScale);

        if (ScreenZoomScaleSuccess)
        {
            *reinterpret_cast<int*>(DBMTable + 0x1E3 * 4) = 1100;
            
            VirtualProtect(reinterpret_cast<LPVOID>(DBMTable + 0x1E3 * 4), 0x4, ScreenZoomScale, &ScreenZoomScale);
            
            Output::send(STR("Successfully patched screen zoom scale!\n"));
        }
        else
        {
            Output::send(STR("Failed to patch screen zoom scale!\n"));
        }
        
        // instant block window
            
        DWORD InstantBlockWindow;
        
        bool InstantBlockWindowSuccess = VirtualProtect(reinterpret_cast<LPVOID>(DBMTable + 0xAD * 4), 0x4, PAGE_READWRITE, &InstantBlockWindow);

        if (InstantBlockWindowSuccess)
        {
            *reinterpret_cast<int*>(DBMTable + 0xAD * 4) = 6;
            
            VirtualProtect(reinterpret_cast<LPVOID>(DBMTable + 0xAD * 4), 0x4, InstantBlockWindow, &InstantBlockWindow);
            
            Output::send(STR("Successfully patched instant block window!\n"));
        }
        else
        {
            Output::send(STR("Failed to patch instant block window!\n"));
        }
    }
};

#define DBMVALUEMOD_API __declspec(dllexport)
extern "C"
{
    DBMVALUEMOD_API CppUserModBase* start_mod()
    {
        return new DBMValueMod();
    }

    DBMVALUEMOD_API void uninstall_mod(CppUserModBase* mod)
    {
        delete mod;
    }
}