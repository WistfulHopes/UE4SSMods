#pragma once

#include <Particles.h>
#include <Unreal/Rotator.hpp>

#include "struct_util.h"
#include "CXXBYTE.h"

enum ACTV_STATE
{
    ACTV_NOT_ACTIVE = 0x0,
    ACTV_ACTIVE = 0x1,
    ACTV_REQ_ACTIVE = 0x2,
    ACTV_REQ_NO_ACTIVE = 0x3,
    ACTV_WAITING_BEGIN = 0x4,
    ACTV_WAITING_0 = 0x4,
    ACTV_WAITING_1 = 0x5,
    ACTV_WAITING_2 = 0x6,
};

enum SIDE_ID : uint8_t
{
    SIDE_BEGIN = 0x0,
    SIDE_1P = 0x0,
    SIDE_2P = 0x1,
    SIDE_ID_NUM = 0x2,
    SIDE_COMMON = 0x2,
    SIDE_ID_NUM_WITH_COMMON = 0x3,
    SIDE_ID_INVALID = 0x4,
};

enum EMemberID : uint8_t
{
    MemberID_Begin = 0x0,
    MemberID_01 = 0x0,
    MemberID_02 = 0x1,
    MemberID_03 = 0x2,
    MemberID_MAX = 0x3,
    MemberID_INVALID = 0x4,
};

class OBJ_CBase {
    char pad[0xA940];
public:
    FIELD(0x0, ACTV_STATE, m_ActiveState);
    FIELD(0x44, SIDE_ID, m_SideID);
    FIELD(0x48, EMemberID, m_MemberID);
    FIELD(0xA080, CXXBYTE<32>, m_CurActionName);
    FIELD(0xA3FC, CCreateArg, m_CreateArg);
    FIELD(0xA7A8, class UParticleSystemComponent*, m_pLinkPSC);
};

static_assert(sizeof(OBJ_CBase) == 0xA940);

class OBJ_CCharBase : public OBJ_CBase {
    char ply_pad[0x31400];
};

static_assert(sizeof(OBJ_CCharBase) == 0x3BD40);

class OBJ_CRPGBase : public OBJ_CBase {
    char rpg_pad[0x738];
};

static_assert(sizeof(OBJ_CRPGBase) == 0xB078);

class BATTLE_TeamManager {
    char pad[0x78];

public:
    FIELD(0x0, SIDE_ID, m_SideID);
    FIELD(0x8, OBJ_CCharBase*, m_pMainPlayerObject);
    ARRAY_FIELD(0x10, OBJ_CCharBase[3], m_pMemberObjects);
    FIELD(0x28, OBJ_CCharBase*, m_pPrevMainPlayerObject);
    FIELD(0x3C, int, m_TensionVal);
    FIELD(0x44, int, m_BurstStockMax);
    FIELD(0x54, int, m_AssistActInvalidTime);
    FIELD(0x5C, bool, m_bDoingUltimateChange);
};

static_assert(sizeof(BATTLE_TeamManager) == 0x78);

class BATTLE_CObjectManager {
    char pad[0x1288800];
    
public:
    ARRAY_FIELD(0x10, BATTLE_TeamManager[2], m_TeamManager);
    FIELD(0xEA8, int, m_ActiveObjectCount);
    ARRAY_FIELD(0x1BC0, OBJ_CBase*[416], m_SortedObjPtrVector);
    ARRAY_FIELD(0x83C0, OBJ_CBase[400], m_ObjVector);
    ARRAY_FIELD(0x108F7C0, OBJ_CCharBase[6], m_CharVector);
    ARRAY_FIELD(0x11F6740, OBJ_CRPGBase[10], m_RpgVector);
};

static_assert(sizeof(BATTLE_CObjectManager) == 0x1288800);

struct AA_Vector3
{
    AA_Vector3()
    {
        X = 0;
        Y = 0;
        Z = 0;
    }
    
    AA_Vector3(const AA_Vector3& orig)
    {
        X = orig.X;
        Y = orig.Y;
        Z = orig.Z;
    }
    
    float X;
    float Y;
    float Z;
};

class AA_CCamera
{    
    char pad[0x1F0];

public:
    struct FRollbackData
    {
        AA_Vector3 m_Pos;
        AA_Vector3 m_Up;
        AA_Vector3 m_At;
    };
    
    FIELD(0x190, AA_Vector3, m_Pos);
    FIELD(0x19C, AA_Vector3, m_Up);
    FIELD(0x1A8, AA_Vector3, m_At);
};

static_assert(sizeof(AA_CCamera) == 0x1F0);

class BATTLE_CScreenManager
{
    char pad[0x1A0];

public:
    FIELD(0x38, AA_CCamera*, m_pCamera);
};

class BattleState
{
    char pad[0x128];
};

class BattleEventManager
{
    char pad[0xF0];

public:
    FIELD(0xB0, void*, m_pBattleEvent);
    FIELD(0xDC, int, m_CurrentBEMState);
};

struct FMatrix
{
    float M[4][4];
};

template<typename T>
struct TLinearInterp
{
    T Val;
    T StartVal;
    T EndVal;
    int MaxFrame;
    int CurrentFrame;
};

class BattleBGLocation
{
    char pad[0x230];

public:
    struct FRollbackData
    {
        FMatrix m_OffsetMatrix;
        TLinearInterp<AA_Vector3> m_OffsetMove_Location;
        TLinearInterp<RC::Unreal::FRotator> m_OffsetMove_Rotation;
    };

    FIELD(0x10, FMatrix, m_OffsetMatrix);
    FIELD(0x12C, TLinearInterp<AA_Vector3>, m_OffsetMove_Location);
    FIELD(0x158, TLinearInterp<RC::Unreal::FRotator>, m_OffsetMove_Rotation);
};

static_assert(sizeof(BattleBGLocation) == 0x230);

struct RollbackData {    
    BATTLE_CObjectManager* ObjManager = nullptr;
    BATTLE_CScreenManager* ScrManager = nullptr;
    BattleState* State = nullptr;
    BattleEventManager* EvtManager = nullptr;
    void* BtlEvent = nullptr;
    AA_CCamera::FRollbackData CamRollbackData;
    BattleBGLocation::FRollbackData BGRollbackData;
    
    void SaveObj(BATTLE_CObjectManager* InObjManager);
    void SaveChara(BATTLE_CObjectManager* InObjManager);
    void SaveRPG(BATTLE_CObjectManager* InObjManager);
    void SaveState(class AREDGameState_Battle* GameState);
    void LoadObj(BATTLE_CObjectManager* InObjManager);
    void LoadChara(BATTLE_CObjectManager* InObjManager);
    void LoadRPG(BATTLE_CObjectManager* InObjManager);
    void LoadState(class AREDGameState_Battle* GameState);
};