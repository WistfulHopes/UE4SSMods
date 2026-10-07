#pragma once

#include "Particles.hpp"
#include <Unreal/Rotator.hpp>

#include "struct_util.hpp"
#include "CXXBYTE.hpp"
#include "safetyhook.hpp"


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
    FIELD(0x8, int, ObjBaseSyncBegin);
    FIELD(0x10, ACTV_STATE, m_ActiveState);
    FIELD(0x14, bool, m_IsPlayerObj);
    FIELD(0x44, SIDE_ID, m_SideID);
    FIELD(0x48, EMemberID, m_MemberID);
    FIELD(0x1BC, int32_t, m_ActionTime);
    FIELD(0x260, OBJ_CBase*, m_pParentPly);
    FIELD(0x3B8, int32_t, m_PosX);
    FIELD(0x3BC, int32_t, m_PosY);
    FIELD(0xDFC, int32_t, m_HitPoint);
    FIELD(0xA080, CXXBYTE<32>, m_CurActionName);
    FIELD(0xA3FC, CCreateArg, m_CreateArg);
    FIELD(0xA780, class AREDPawn*, m_pPawn);
    FIELD(0xA788, struct CXXBYTE<32>, m_PawnName);
    FIELD(0xA7A8, class UParticleSystemComponent*, m_pLinkPSC);
    FIELD(0xA7DB, bool, m_StepAnimeMode);
};

static_assert(sizeof(OBJ_CBase) == 0xA940);

class OBJ_CCharBase : public OBJ_CBase {
    char ply_pad[0x31400];
};

static_assert(sizeof(OBJ_CCharBase) == 0x3BD40);

class OBJ_CStageBase : public OBJ_CBase {
    char stg_pad[0x738];
};

static_assert(sizeof(OBJ_CStageBase) == 0xB078);

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
    ARRAY_FIELD(0x11F6740, OBJ_CStageBase[10], m_StageVector);
    FIELD(0x12861c0, class AREDPawn*, m_pCommonPlayerPawn);
};

static_assert(sizeof(BATTLE_CObjectManager) == 0x1288800);

inline constexpr int32_t ObjCount = 400;
inline constexpr int32_t CharCount = 6;
inline constexpr int32_t StageCount = 10;
inline constexpr int32_t TotalObjCount = ObjCount + CharCount + StageCount;

inline constexpr int32_t InvalidSlot = -1;

inline constexpr size_t ObjManagerHeadOffset = 0x8;
inline constexpr size_t ObjManagerHeadSize = 0x83B8;
inline constexpr size_t ObjManagerTailOffset = 0x1264BF0;
inline constexpr size_t ObjManagerTailSize = 0x23C10;

inline constexpr size_t SyncSkipOffset = 0x8;

struct AA_Vector3
{
    float X = 0;
    float Y = 0;
    float Z = 0;
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

class OBJ_CBaseExt
{
public:
    struct SRollbackData
    {
        CXXBYTE<32> LinkParticleName;
        bool bLinkParticleSet;
        uint32_t LinkParticleStartFrame;
        CXXBYTE<32> LinkParticleActName;
        unsigned int PointLightId;
        
        CXXBYTE<32> LinkPawnName;
        bool LinkPawnSet;
        uint32_t LinkCommonPawn;
        bool LinkCommonPawnEffect;
        bool LinkCommonPawnSelf;
        bool LinkPawnPlayingCutSceneAnime;
        
        uint32_t LinkPawnStartFrame;
        CXXBYTE<32> LinkPawnActName;
        
        CXXBYTE<32> MeshSetName;
        CXXBYTE<32> MaterialSetName;
        
        CXXBYTE<32> PlayAnimeName;
        int32_t PlayAnimeStartFrame;
        bool bPlayAnimeSet;
        float PlayAnimeCurrentTime;
        int32_t PlayAnimCutSceneStepFramePawn;
        int32_t PlayAnimCutSceneStepFrameAnimInst;
    };

    SRollbackData m_RollbackData;
    int m_LinkParticleActiveFrame = -1;
};

class AA_CRandMT
{
public:
    struct FRollbackData
    {
        uint32_t m_State[624];
        int32_t m_Left;
        int32_t m_Initf;
        uint32_t* m_pNext;
    };
    
    uint32_t m_State[624];
    int32_t m_Left;
    int32_t m_Initf;
    uint32_t* m_pNext;

    void MakeRollbackData(FRollbackData& data)
    {
    }
};

extern SafetyHookInline DeleteLinkPSC_Detour;
extern SafetyHookInline UEParticleGet_Detour;
extern SafetyHookInline CreateParticleArg_Detour;
extern SafetyHookInline LinkParticleEx_Detour;
extern SafetyHookInline LinkParticleArg_Detour;
extern SafetyHookInline OBJ_CBase_Ctor_Detour;
extern SafetyHookInline ObjectConstructor_Detour;
extern SafetyHookInline AfterFrameStep_Detour;
extern SafetyHookInline UpdateBattle_Detour;
extern SafetyHookInline LinkCommonPawnEffect_Detour;

inline constexpr size_t MaxBattleEventSize = 0x178;
inline constexpr uint32_t BEMCount = 41;

size_t GetBattleEventSize(uint32_t BEMState);

struct RollbackData
{
    uint8_t ObjManagerHead[ObjManagerHeadSize];
    uint8_t ObjManagerTail[ObjManagerTailSize];
    uint8_t ScrManager[sizeof(BATTLE_CScreenManager)];
    uint8_t State[sizeof(BattleState) - SyncSkipOffset];
    uint8_t EvtManager[sizeof(BattleEventManager) - SyncSkipOffset];

    uint8_t BattleEvent[MaxBattleEventSize];
    uint32_t BattleEventSize;
    uint32_t BattleEventState;

    uint8_t Chara[CharCount][sizeof(OBJ_CCharBase) - SyncSkipOffset];
    uint8_t Stage[StageCount][sizeof(OBJ_CStageBase) - SyncSkipOffset];

    bool bObjActive[ObjCount];

    OBJ_CBaseExt ObjExt[TotalObjCount];

    AA_CCamera::FRollbackData Cam;
    BattleBGLocation::FRollbackData BG;
    AA_CRandMT::FRollbackData Rand;

    uint32_t SavedGameFrame;
    int32_t Inputs[2];

    uint8_t Obj[ObjCount][sizeof(OBJ_CBase) - SyncSkipOffset];

    void SaveState(struct RollbackContext& Ctx);
    void LoadState(RollbackContext& Ctx);

    size_t Compress(uint8_t* Dst, size_t DstSize) const;

    bool Decompress(const uint8_t* Src, size_t SrcSize);

    size_t PackedSize() const;
};

static_assert(sizeof(RollbackData) - (offsetof(RollbackData, Obj) + sizeof(RollbackData::Obj))
                  < alignof(RollbackData),
              "RollbackData::Obj must be the last member");

static_assert(std::is_trivially_copyable_v<RollbackData>,
              "RollbackData must be POD");