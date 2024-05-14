#pragma once

#include "HAL/Platform.hpp"

using namespace RC::Unreal;

struct FActionParameter {
    int32 PreActionID;
    int32 ActionID;
    int32 ActionFrame;
    bool isActionEnd;
    bool isActionFollow;
    bool isActionMargin;
    int32 LoopCountNum;
    int32 ActionChangedNum;
    bool IsCommonAction;
    bool IsHitStop;
    int32 AttackCollisionListIndex;
    int32 HitCollisionListIndex;
    int32 BodyCollisionListIndex;
};

enum class EDamageVoice {
    NO_VOICE = 0,
    VO_DMG_L = 1,
    VO_DMG_M = 2,
    VO_DMG_H = 3,
    DAMAGE_VOICE_MAX = 4,
    EDamageVoice_MAX = 5,
};

enum class EHitSound {
    NO_SOUND = 0,
    HIT_L = 1,
    HIT_M = 2,
    HIT_H_PUNCH = 3,
    HIT_H_KICK = 4,
    HIT_SLASH_L = 5,
    HIT_SLASH_M = 6,
    HIT_SLASH_H_PUNCH = 7,
    HIT_SLASH_H_KICK = 8,
    HIT_GRAB = 9,
    HIT_STAB = 10,
    HIT_BONE = 11,
    HIT_BITE = 12,
    HIT_TIGHTEN = 13,
    HIT_SHELL = 14,
    HIT_BULLET = 15,
    HIT_THROW = 16,
    HIT_SFX_FIRE = 17,
    HIT_SFX_ELEC = 18,
    HIT_SFX_ICE = 19,
    HIT_SFX_ICEBREAK = 20,
    HIT_SFX_POISON = 21,
    HIT_SFX_EXP_L = 22,
    HIT_SFX_EXP_H = 23,
    HIT_SFX_SP = 24,
    GUARD = 25,
    CONC_DOWN = 26,
    JUMP_NORMAL = 27,
    JUMP_AIR = 28,
    JUMP_SP = 29,
    STEP_FRONT = 30,
    STEP_BACK = 31,
    STEP_SWOOP = 32,
    STUN_IN = 33,
    SLIDING_L = 34,
    SLIDING_M = 35,
    SLIDING_H = 36,
    WALL_LAND_NORMAL = 37,
    WALL_LAND_METAL = 38,
    WALL_LAND_BIG = 39,
    WALL_JUMP_NORMAL = 40,
    WALL_JUMP_METAL = 41,
    WALL_JUMP_BIG = 42,
    STUN_CAP = 43,
    STUN_MAR = 44,
    SHELL_OFFSET = 45,
    ATEMI_SUCCESS = 46,
    ADV_GUARD = 47,
    THROW_OUT = 48,
    CHANGE_ATK = 49,
    SC_START = 50,
    SC_DELAY = 51,
    FIN_DOWN = 52,
    FIN_KO = 53,
    FIN_AKEBONO = 54,
    NA_READY = 55,
    NA_FIGHT = 56,
    NA_KO = 57,
    NA_TIMEOVER = 58,
    NA_GEM_MIND = 59,
    NA_GEM_POWER = 60,
    NA_GEM_REALITY = 61,
    NA_GEM_SOUL = 62,
    NA_GEM_SPACE = 63,
    NA_GEM_TIME = 64,
    HIT_SOUND_MAX = 65,
    EHitSound_MAX = 66,
};

enum class ECharacterAssetVfxCategory {
    COMMON = 0,
    FIELD = 1,
    CHARA = 2,
    FIELD_COMMON = 3,
    ECharacterAssetVfxCategory_MAX = 4,
};

struct FStageVfxData
{
    int32 VfxID;                                                                      // 0x0000 (size: 0x4)
    ECharacterAssetVfxCategory Category;                                              // 0x0004 (size: 0x1)
    int32 StageID;                                                                    // 0x0008 (size: 0x4)

}; // Size: 0xC

enum class EMaterialType {
    DEFAULT = 0,
    BODY = 1,
    METAL = 2,
    NO_SOUND = 3,
    EMaterialType_MAX = 4,
};

enum class ECharacterAssetShotReflect {
    NOT_REFLECT = 0,
    HIT_DELETE = 1,
    DAMAGE_DELETE = 2,
    NATURAL_DELETE = 3,
    INSTANT_DELETE = 4,
    HIT_COUNT_ZERO = 5,
    AXIS_NONE = 6,
    AXIS_X = 7,
    AXIS_Y = 8,
    AXIS_Y_G = 9,
    AXIS_XY = 10,
    AXIS_XY_G = 11,
    AXIS_Y_G_FLIP = 12,
    AXIS_XY_G_FLIP = 13,
    ECharacterAssetShotReflect_MAX = 14,
};

enum class ECharacterAssetShotHitProperty {
    HIT_ENERGY = 0,
    HIT_OBJECT = 1,
    HIT_MAN = 2,
    ECharacterAssetShotHitProperty_MAX = 3,
};

enum class ECharacterAssetShotCategory {
    CATEGORY_0 = 0,
    CATEGORY_1 = 1,
    CATEGORY_2 = 2,
    CATEGORY_3 = 3,
    CATEGORY_4 = 4,
    CATEGORY_5 = 5,
    CATEGORY_6 = 6,
    CATEGORY_7 = 7,
    CATEGORY_GEM_LP = 8,
    CATEGORY_GEM = 9,
    CATEGORY_GEM_SURGE = 10,
    CATEGORY_NONE = 11,
    ECharacterAssetShotCategory_MAX = 12,
};

struct FProjectileData
{
    int32 TargetScatterZ;                                                             // 0x0000 (size: 0x4)
    int32 TargetScatterY;                                                             // 0x0004 (size: 0x4)
    int32 TargetScatterX;                                                             // 0x0008 (size: 0x4)
    int8 HitSpan;                                                                     // 0x000C (size: 0x1)
    int8 HitCount;                                                                    // 0x000D (size: 0x1)
    bool IsBullet;                                                                    // 0x000E (size: 0x1)
    bool ErasedByDifferentStyle;                                                      // 0x000F (size: 0x1)
    bool ErasedByPlayerDamage;                                                        // 0x0010 (size: 0x1)
    bool NoHitIfPlayerDamageEx;                                                       // 0x0011 (size: 0x1)
    bool NoHitIfPlayerDamage;                                                         // 0x0012 (size: 0x1)
    bool HitToPlayer;                                                                 // 0x0013 (size: 0x1)
    EMaterialType Made_of;                                                            // 0x0014 (size: 0x1)
    ECharacterAssetShotReflect Reflect;                                               // 0x0015 (size: 0x1)
    ECharacterAssetShotHitProperty HitKind;                                           // 0x0016 (size: 0x1)
    ECharacterAssetShotCategory Category;                                             // 0x0017 (size: 0x1)
    bool NoDeathVfxLanding;                                                           // 0x0018 (size: 0x1)
    bool NotDieIfHitCountZero;                                                        // 0x0019 (size: 0x1)
    bool DeadPriorityLow;                                                             // 0x001A (size: 0x1)
    bool IsMotionCameraVisible;                                                       // 0x001B (size: 0x1)
    bool IgnoreCalcDeathEffectRot;                                                    // 0x001C (size: 0x1)
    bool IsRotateMirroring;                                                           // 0x001D (size: 0x1)
    bool AdjustLightChanelGround;                                                     // 0x001E (size: 0x1)
    bool AdjustLightChanelDeath;                                                      // 0x001F (size: 0x1)
    bool AdjustLightChanelCounteract;                                                 // 0x0020 (size: 0x1)
    bool AdjustLightChanelAura;                                                       // 0x0021 (size: 0x1)
    bool AdjustLightChanelBeam;                                                       // 0x0022 (size: 0x1)
    bool AdjustLightChanelCore;                                                       // 0x0023 (size: 0x1)
    bool IsCastShadowGround;                                                          // 0x0024 (size: 0x1)
    bool IsCastShadowDeath;                                                           // 0x0025 (size: 0x1)
    bool IsCastShadowCounteract;                                                      // 0x0026 (size: 0x1)
    bool IsCastShadowAura;                                                            // 0x0027 (size: 0x1)
    bool IsCastShadowBeam;                                                            // 0x0028 (size: 0x1)
    bool IsCastShadowCore;                                                            // 0x0029 (size: 0x1)
    int32 RangeB;                                                                     // 0x002C (size: 0x4)
    int32 RangeF;                                                                     // 0x0030 (size: 0x4)
    int32 RangeD;                                                                     // 0x0034 (size: 0x4)
    int32 RangeU;                                                                     // 0x0038 (size: 0x4)
    int32 DeathBoneID;                                                                // 0x003C (size: 0x4)
    int32 Scale;                                                                      // 0x0040 (size: 0x4)
    int32 AuraDeleteFrame;                                                            // 0x0044 (size: 0x4)
    int32 ZScalingFrame;                                                              // 0x0048 (size: 0x4)
    int32 MeshID;                                                                     // 0x004C (size: 0x4)
    Unreal::TArray<FStageVfxData> GroundStageData;                                            // 0x0050 (size: 0x10)
    int32 GroundVfxID;                                                                // 0x0060 (size: 0x4)
    ECharacterAssetVfxCategory GroundCategory;                                        // 0x0064 (size: 0x1)
    Unreal::TArray<FStageVfxData> NaturalDeathStageData;                                      // 0x0068 (size: 0x10)
    int32 NaturalDeathVfxID;                                                          // 0x0078 (size: 0x4)
    ECharacterAssetVfxCategory NaturalDeathCategory;                                  // 0x007C (size: 0x1)
    Unreal::TArray<FStageVfxData> DeathStageData;                                             // 0x0080 (size: 0x10)
    int32 DeathVfxID;                                                                 // 0x0090 (size: 0x4)
    ECharacterAssetVfxCategory DeathCategory;                                         // 0x0094 (size: 0x1)
    Unreal::TArray<FStageVfxData> CounteractStageData;                                        // 0x0098 (size: 0x10)
    int32 CounteractVfxID;                                                            // 0x00A8 (size: 0x4)
    ECharacterAssetVfxCategory CounteractCategory;                                    // 0x00AC (size: 0x1)
    Unreal::TArray<FStageVfxData> AuraStageData;                                              // 0x00B0 (size: 0x10)
    int32 AuraVfxID;                                                                  // 0x00C0 (size: 0x4)
    ECharacterAssetVfxCategory AuraCategory;                                          // 0x00C4 (size: 0x1)
    Unreal::TArray<FStageVfxData> BeamStageData;                                              // 0x00C8 (size: 0x10)
    int32 BeamVfxID;                                                                  // 0x00D8 (size: 0x4)
    ECharacterAssetVfxCategory BeamCategory;                                          // 0x00DC (size: 0x1)
    Unreal::TArray<FStageVfxData> CoreStageData;                                              // 0x00E0 (size: 0x10)
    int32 CoreVfxID;                                                                  // 0x00F0 (size: 0x4)
    ECharacterAssetVfxCategory CoreCategory;                                          // 0x00F4 (size: 0x1)

}; // Size: 0xF8

struct FAttackData
{
    int32 Vital;                                                                      // 0x0000 (size: 0x4)
    bool StartAdj_H;                                                                  // 0x0004 (size: 0x1)
    bool StartAdj_M;                                                                  // 0x0005 (size: 0x1)
    bool StartAdj_L;                                                                  // 0x0006 (size: 0x1)
    uint8 StartAdjust;                                                                // 0x0007 (size: 0x1)
    uint8 DamageType;                                                                 // 0x0008 (size: 0x1)
    bool NoZuri;                                                                      // 0x0009 (size: 0x1)
    bool ChoButtobi;                                                                  // 0x000A (size: 0x1)
    bool FloorBound;                                                                  // 0x000B (size: 0x1)
    bool WallBound;                                                                   // 0x000C (size: 0x1)
    bool KezuReserve;                                                                 // 0x000D (size: 0x1)
    bool KezuHoldDmg;                                                                 // 0x000E (size: 0x1)
    bool KezuReflect;                                                                 // 0x000F (size: 0x1)
    bool KezuAtemiSH;                                                                 // 0x0010 (size: 0x1)
    bool KezuAtemiPL;                                                                 // 0x0011 (size: 0x1)
    bool KezuDown;                                                                    // 0x0012 (size: 0x1)
    bool KezuStand;                                                                   // 0x0013 (size: 0x1)
    uint8 AttackKind;                                                                 // 0x0014 (size: 0x1)
    uint8 DamageKind;                                                                 // 0x0015 (size: 0x1)
    uint8 DamageChara;                                                                // 0x0016 (size: 0x1)
    bool ExZuri;                                                                      // 0x0017 (size: 0x1)
    uint8 BoundRatioWall;                                                             // 0x0018 (size: 0x1)
    uint8 BoundRatioFloor;                                                            // 0x0019 (size: 0x1)
    uint8 PiyoPoint;                                                                  // 0x001A (size: 0x1)
    int8 HitStopDmg;                                                                  // 0x001B (size: 0x1)
    uint8 HitStop;                                                                    // 0x001C (size: 0x1)
    uint8 SandbagAdjust;                                                              // 0x001D (size: 0x1)
    uint8 SandbagTime;                                                                // 0x001E (size: 0x1)
    uint8 DownTime;                                                                   // 0x001F (size: 0x1)
    uint8 CharaFlip;                                                                  // 0x0020 (size: 0x1)
    uint8 DmDir;                                                                      // 0x0021 (size: 0x1)
    uint8 ButtVector;                                                                 // 0x0022 (size: 0x1)
    uint8 ButtPower;                                                                  // 0x0023 (size: 0x1)
    int8 ZuriPow;                                                                     // 0x0024 (size: 0x1)
    uint8 ScreenVib;                                                                  // 0x0025 (size: 0x1)
    uint8 HitMarkFloor;                                                               // 0x0026 (size: 0x1)
    uint8 HitMarkDir;                                                                 // 0x0027 (size: 0x1)
    uint8 HitMark;                                                                    // 0x0028 (size: 0x1)
    EDamageVoice DamageVoice;                                                         // 0x0029 (size: 0x1)
    EHitSound HitSound3;                                                              // 0x002A (size: 0x1)
    EHitSound HitSound2;                                                              // 0x002B (size: 0x1)
    EHitSound HitSound;                                                               // 0x002C (size: 0x1)
    int32 GaugeAdd;                                                                   // 0x0030 (size: 0x4)
    bool NoGaugeDmg;                                                                  // 0x0034 (size: 0x1)
    bool NoGaugeAtk;                                                                  // 0x0035 (size: 0x1)
    uint8 ArmorPoint;                                                                 // 0x0036 (size: 0x1)
    uint8 Etc;                                                                        // 0x0037 (size: 0x1)
    bool GuardMarkNoDisp;                                                             // 0x0038 (size: 0x1)
    bool DoublePrio;                                                                  // 0x0039 (size: 0x1)
    bool WeakAttack;                                                                  // 0x003A (size: 0x1)
    bool HyperComboLv3;                                                               // 0x003B (size: 0x1)
    bool ShockBite;                                                                   // 0x003C (size: 0x1)
    bool LifeSteal;                                                                   // 0x003D (size: 0x1)
    bool NoRolling;                                                                   // 0x003E (size: 0x1)
    bool SnapShot;                                                                    // 0x003F (size: 0x1)
    bool AssistHitToGuard;                                                            // 0x0040 (size: 0x1)
    bool ApplyVelocity;                                                               // 0x0041 (size: 0x1)
    bool ApplyHitPos;                                                                 // 0x0042 (size: 0x1)
    bool ApplyMasterDir;                                                              // 0x0043 (size: 0x1)
    bool DrawAttack;                                                                  // 0x0044 (size: 0x1)
    bool NoComboAdjust;                                                               // 0x0045 (size: 0x1)
    bool NoComboCount;                                                                // 0x0046 (size: 0x1)
    bool NoDeath;                                                                     // 0x0047 (size: 0x1)
    bool AerialBegin;                                                                 // 0x0048 (size: 0x1)
    bool AerialFinish;                                                                // 0x0049 (size: 0x1)
    bool NoKezuriDeath;                                                               // 0x004A (size: 0x1)
    bool AtemiBreak;                                                                  // 0x004B (size: 0x1)
    uint8 GuardStop;                                                                  // 0x004C (size: 0x1)
    uint8 MutekiTime;                                                                 // 0x004D (size: 0x1)
    bool IgnoreHitStop;                                                               // 0x004E (size: 0x1)
    bool NormalCatch;                                                                 // 0x004F (size: 0x1)
    bool HitCatch;                                                                    // 0x0050 (size: 0x1)
    bool GuardCatch;                                                                  // 0x0051 (size: 0x1)
    bool ShellCatch;                                                                  // 0x0052 (size: 0x1)
    bool NotAdjustPosition;                                                           // 0x0053 (size: 0x1)
    bool NoEscape;                                                                    // 0x0054 (size: 0x1)
    bool VsDown;                                                                      // 0x0055 (size: 0x1)
    bool VsJump;                                                                      // 0x0056 (size: 0x1)
    bool VsCrouch;                                                                    // 0x0057 (size: 0x1)
    bool VsStand;                                                                     // 0x0058 (size: 0x1)
    uint8 NagerareType;                                                               // 0x0059 (size: 0x1)
    uint8 KezuriType;                                                                 // 0x005A (size: 0x1)
    uint8 SkillKind;                                                                  // 0x005B (size: 0x1)
    uint8 GuardType;                                                                  // 0x005C (size: 0x1)
    uint8 ComboAdd;                                                                   // 0x005D (size: 0x1)

}; // Size: 0x60

struct FHitRect16
{
    int16 SizeY;                                                                      // 0x0000 (size: 0x2)
    int16 SizeX;                                                                      // 0x0002 (size: 0x2)
    int16 OffsetY;                                                                    // 0x0004 (size: 0x2)
    int16 OffsetX;                                                                    // 0x0006 (size: 0x2)

}; // Size: 0x8

struct FHitRectData
{
    FHitRect16 RectData;                                                              // 0x0000 (size: 0x8)

}; // Size: 0x8

struct FCharacterAssetCmdGrpSelTrg
{
    int32 No;                                                                         // 0x0000 (size: 0x4)

}; // Size: 0x4

struct FCharacterAssetCommandGroup
{
    Unreal::TArray<FCharacterAssetCmdGrpSelTrg> SelectTrigger;                                // 0x0000 (size: 0x10)
    int32 No;                                                                         // 0x0010 (size: 0x4)
    FString Name;                                                                     // 0x0018 (size: 0x10)

}; // Size: 0x28

struct FCharacterAssetTrgGrpSelTrg
{
    int32 No;                                                                         // 0x0000 (size: 0x4)

}; // Size: 0x4

struct FCharacterAssetTriggerGroup
{
    Unreal::TArray<FCharacterAssetTrgGrpSelTrg> SelectTrigger;                                // 0x0000 (size: 0x10)
    int32 No;                                                                         // 0x0010 (size: 0x4)
    FString Name;                                                                     // 0x0018 (size: 0x10)

}; // Size: 0x28

struct FCharacterAssetTriggerFreeParam
{
    int32 Param;                                                                      // 0x0000 (size: 0x10)

}; // Size: 0x10

struct FCharacterAssetTriggerGauge
{
    int32 VitalConsume;                                                               // 0x0000 (size: 0x4)
    int32 VitalNeed;                                                                  // 0x0004 (size: 0x4)
    int32 SHC_GaugeConsume;                                                           // 0x0008 (size: 0x4)
    int32 SHC_GaugeNeed;                                                              // 0x000C (size: 0x4)

}; // Size: 0x10

struct FCharacterAssetTriggerState
{
    bool Sleep;                                                                       // 0x0000 (size: 0x1)
    bool Air;                                                                         // 0x0001 (size: 0x1)
    bool Crouch;                                                                      // 0x0002 (size: 0x1)
    bool Stand;                                                                       // 0x0003 (size: 0x1)

}; // Size: 0x4

enum class ECharacterAssetCompCond {
    EQ = 0,
    NE = 1,
    LT = 2,
    LE = 3,
    GT = 4,
    GE = 5,
    ECharacterAssetCompCond_MAX = 6,
};

enum class ECharacterAssetUParamType {
    NONE = 0,
    PARAM_0 = 1,
    PARAM_1 = 2,
    PARAM_2 = 3,
    PARAM_3 = 4,
    ECharacterAssetUParamType_MAX = 5,
};

enum class ECharacterAssetTriggerCondType {
    NONE = 0,
    LESS_THAN = 1,
    GREATER_THAN = 2,
    ECharacterAssetTriggerCondType_MAX = 3,
};

enum class ECharacterAssetRangeCond {
    NONE = 0,
    X_GE = 1,
    X_LE = 2,
    Y_GE = 3,
    Y_LE = 4,
    Z_GE = 5,
    Z_LE = 6,
    ECharacterAssetRangeCond_MAX = 7,
};

struct FCharacterAssetTriggerCondition
{
    int8 UParamValue;                                                                 // 0x0000 (size: 0x1)
    ECharacterAssetCompCond UParamCond;                                               // 0x0001 (size: 0x1)
    ECharacterAssetUParamType UParamType;                                             // 0x0002 (size: 0x1)
    int32 ShotCategoryFlag;                                                           // 0x0004 (size: 0x4)
    int32 GemKind;                                                                    // 0x0008 (size: 0x4)
    int32 EffectLimit;                                                                // 0x000C (size: 0x4)
    int32 MultiJumpLimit;                                                             // 0x0010 (size: 0x4)
    int32 JumpCommandLimit;                                                           // 0x0014 (size: 0x4)
    ECharacterAssetTriggerCondType VitalRatioCondition;                               // 0x0018 (size: 0x1)
    int32 VitalRatio;                                                                 // 0x001C (size: 0x4)
    ECharacterAssetRangeCond RangeConditionType;                                      // 0x0020 (size: 0x1)
    int32 RangeFix;                                                                   // 0x0024 (size: 0x4)
    int32 SelectFightStyle;                                                           // 0x0028 (size: 0x4)

}; // Size: 0x2C

struct FCharacterAssetTriggerCategory
{
    bool Fly;                                                                         // 0x0000 (size: 0x1)
    bool EasyHyper;                                                                   // 0x0001 (size: 0x1)
    bool Kick;                                                                        // 0x0002 (size: 0x1)
    bool Punch;                                                                       // 0x0003 (size: 0x1)
    bool Heavy;                                                                       // 0x0004 (size: 0x1)
    bool Middle;                                                                      // 0x0005 (size: 0x1)
    bool Light;                                                                       // 0x0006 (size: 0x1)

}; // Size: 0x7

enum class ECharacterAssetFunctionType {
    NORMAL = 0,
    HYPER = 1,
    SPECIAL = 2,
    DASH = 3,
    AIR_DASH = 4,
    NORMAL_JUMP = 5,
    AIR_JUMP = 6,
    SUPER_JUMP = 7,
    TOUCH = 8,
    ASSIST = 9,
    GEM = 10,
    GEM_HYPER = 11,
    GEM_ASSIST = 12,
    NO_EFFECT = 13,
    MINION = 14,
    FUNC_TYPE_NUM = 15,
    ECharacterAssetFunctionType_MAX = 16,
};

enum class ECharacterAssetCommandDir {
    SIDE = 0,
    DIRECTION = 1,
    ECharacterAssetCommandDir_MAX = 2,
};

enum class ECharacterAssetCommandType {
    NONE = 0,
    CMD_236 = 1,
    CMD_214 = 2,
    CMD_623 = 3,
    CMD_421 = 4,
    CMD_632 = 5,
    CMD_412 = 6,
    CMD_66 = 7,
    CMD_44 = 8,
    CMD_22 = 9,
    CMD_88 = 10,
    CMD_46 = 11,
    CMD_28 = 12,
    CMD_41236 = 13,
    CMD_63214 = 14,
    CMD_XX6AY = 15,
    ROUND_1 = 16,
    RAPID_P_L = 17,
    RAPID_P_H = 18,
    RAPID_K_L = 19,
    RAPID_K_H = 20,
    FREE_21 = 21,
    FREE_22 = 22,
    FREE_23 = 23,
    FREE_24 = 24,
    FREE_25 = 25,
    FREE_26 = 26,
    FREE_27 = 27,
    FREE_28 = 28,
    FREE_29 = 29,
    FREE_30 = 30,
    FREE_31 = 31,
    CHG_SJUMP = 32,
    CHG_TOUCH = 33,
    CHG_GEM = 34,
    ECharacterAssetCommandType_MAX = 35,
};

enum class ECharacterAssetCondType {
    NONE = 0,
    INCLUSIVE = 1,
    EXCLUSIVE = 2,
    PARALLEL = 3,
    EQUAL = 4,
    ECharacterAssetCondType_MAX = 5,
};

struct FCharacterAssetTriggerLevBut
{
    bool Release;                                                                     // 0x0000 (size: 0x1)
    bool Direct;                                                                      // 0x0001 (size: 0x1)
    bool Press;                                                                       // 0x0002 (size: 0x1)
    ECharacterAssetCondType Condition;                                                // 0x0003 (size: 0x1)

}; // Size: 0x4

struct FCharacterAssetTrigger
{
    int32 CommandUI;                                                                  // 0x0000 (size: 0x4)
    FCharacterAssetTriggerFreeParam FreeParam;                                        // 0x0004 (size: 0x10)
    FCharacterAssetTriggerGauge Gauge;                                                // 0x0014 (size: 0x10)
    FCharacterAssetTriggerState State;                                                // 0x0024 (size: 0x4)
    FCharacterAssetTriggerCondition Condition;                                        // 0x0028 (size: 0x2C)
    FCharacterAssetTriggerCategory Category;                                          // 0x0054 (size: 0x7)
    ECharacterAssetFunctionType Function;                                             // 0x005B (size: 0x1)
    int32 CommandGroup;                                                               // 0x005C (size: 0x4)
    int32 Action;                                                                     // 0x0060 (size: 0x4)
    int32 ActionList;                                                                 // 0x0064 (size: 0x4)
    ECharacterAssetCommandDir CommandDir;                                             // 0x0068 (size: 0x1)
    ECharacterAssetCommandType Command;                                               // 0x0069 (size: 0x1)
    int32 NGKey;                                                                      // 0x006C (size: 0x4)
    FCharacterAssetTriggerLevBut Button;                                              // 0x0070 (size: 0x4)
    FCharacterAssetTriggerLevBut Lever;                                               // 0x0074 (size: 0x4)
    int32 Key;                                                                        // 0x0078 (size: 0x4)
    bool Disable;                                                                     // 0x007C (size: 0x1)
    FString Name;                                                                     // 0x0080 (size: 0x10)

}; // Size: 0x90

struct FStyleData
{
    int32 IntActionExit;                                                              // 0x0000 (size: 0x4)
    int32 IntStyleExit;                                                               // 0x0004 (size: 0x4)
    int32 IntActionInit;                                                              // 0x0008 (size: 0x4)
    int32 IntStyleInit;                                                               // 0x000C (size: 0x4)
    int32 IntActionRvk;                                                               // 0x0010 (size: 0x4)
    int32 IntStyleRvk;                                                                // 0x0014 (size: 0x4)
    int32 IntActionTime;                                                              // 0x0018 (size: 0x4)
    int32 IntStyleTime;                                                               // 0x001C (size: 0x4)
    int32 LimitCancelST1;                                                             // 0x0020 (size: 0x4)
    int32 LimitCancelST0;                                                             // 0x0024 (size: 0x4)
    int32 TimeLimit;                                                                  // 0x0028 (size: 0x4)
    int32 EndActST1;                                                                  // 0x002C (size: 0x4)
    int32 EndActST0;                                                                  // 0x0030 (size: 0x4)
    int32 EndAction;                                                                  // 0x0034 (size: 0x4)
    int32 EndStyle;                                                                   // 0x0038 (size: 0x4)
    bool CantTouch;                                                                   // 0x003C (size: 0x1)
    bool CantAirGuard;                                                                // 0x003D (size: 0x1)
    bool CantCrouchGuard;                                                             // 0x003E (size: 0x1)
    bool CantStandGuard;                                                              // 0x003F (size: 0x1)
    bool CantAirJump;                                                                 // 0x0040 (size: 0x1)
    bool CantTriangleJump;                                                            // 0x0041 (size: 0x1)
    bool CantJump;                                                                    // 0x0042 (size: 0x1)
    bool CantCrouch;                                                                  // 0x0043 (size: 0x1)
    bool CantTurn;                                                                    // 0x0044 (size: 0x1)
    bool CantWalk;                                                                    // 0x0045 (size: 0x1)
    bool Immortal;                                                                    // 0x0046 (size: 0x1)
    bool AttackResist;                                                                // 0x0047 (size: 0x1)
    bool DamageResist;                                                                // 0x0048 (size: 0x1)
    bool SuperArmor;                                                                  // 0x0049 (size: 0x1)
    int32 GainRatioHC;                                                                // 0x004C (size: 0x4)
    int32 GemGaugeAdd;                                                                // 0x0050 (size: 0x4)
    int32 VitalAddAndSub;                                                             // 0x0054 (size: 0x4)
    int32 MoveSpeedAdjust;                                                            // 0x0058 (size: 0x4)
    int32 DefensiveAdjust;                                                            // 0x005C (size: 0x4)
    int32 OffensiveAdjust;                                                            // 0x0060 (size: 0x4)
    int32 BaseStyleNo;                                                                // 0x0064 (size: 0x4)

}; // Size: 0x68

enum class ECharacterAssetActionGroup {
    BASIC = 0,
    GUARD = 1,
    DAMAGE = 2,
    NORMAL_ATTACK = 3,
    CATCH = 4,
    SPECIAL = 5,
    HYPER = 6,
    PERFORM = 7,
    DEMO = 8,
    INTERRUPT = 9,
    SHOT = 10,
    DEBUG = 11,
    ALL = 12,
    ECharacterAssetActionGroup_MAX = 13,
};

struct FCharacterAssetActionProjectile
{
    int32 DataIndex;                                                                  // 0x0000 (size: 0x4)

}; // Size: 0x4

struct FCharacterAssetActionInherit
{
    bool BasePositionZ;                                                               // 0x0000 (size: 0x1)
    bool BasePositionY;                                                               // 0x0001 (size: 0x1)
    bool BasePositionX;                                                               // 0x0002 (size: 0x1)
    float AccelalerationZ;                                                            // 0x0004 (size: 0x4)
    float AccelalerationY;                                                            // 0x0008 (size: 0x4)
    float AccelalerationX;                                                            // 0x000C (size: 0x4)
    float VelocityRateZ;                                                              // 0x0010 (size: 0x4)
    float VelocityRateY;                                                              // 0x0014 (size: 0x4)
    float VelocityRateX;                                                              // 0x0018 (size: 0x4)

}; // Size: 0x1C

struct FCharacterAssetActionState
{
    int32 ObjectPriority;                                                             // 0x0000 (size: 0x4)
    int32 EndStateParam;                                                              // 0x0004 (size: 0x4)
    int32 LoopCount;                                                                  // 0x0008 (size: 0x4)

}; // Size: 0xC

struct FCharacterAssetActionFrame
{
    int32 MarginFrame;                                                                // 0x0000 (size: 0x4)
    int32 FollowFrame;                                                                // 0x0004 (size: 0x4)
    int32 MainFrame;                                                                  // 0x0008 (size: 0x4)

}; // Size: 0xC

struct FCharacterAssetActionRange
{
    int16 BACK;                                                                       // 0x0000 (size: 0x2)
    int16 Front;                                                                      // 0x0002 (size: 0x2)
    int16 Bottom;                                                                     // 0x0004 (size: 0x2)
    int16 Top;                                                                        // 0x0006 (size: 0x2)

}; // Size: 0x8

struct FCharacterAssetKeyDetails
{
    bool Disable;                                                                     // 0x0000 (size: 0x1)
    int32 Slot;                                                                       // 0x0004 (size: 0x4)
    int32 Range;                                                                      // 0x0008 (size: 0x4)
    int32 EndFrame;                                                                   // 0x000C (size: 0x4)
    int32 StartFrame;                                                                 // 0x0010 (size: 0x4)

}; // Size: 0x14

struct FCharacterAssetSoundKey : public FCharacterAssetKeyDetails
{
    bool OnlyOnce;                                                                    // 0x0014 (size: 0x1)
    int32 ID;                                                                         // 0x0018 (size: 0x4)

}; // Size: 0x1C

enum class ECharacterAssetGimmickDeadOperation {
    OP_DELETE = 0,
    OP_CANCEL = 1,
    OP_CANCEL2DELETE = 2,
    ECharacterAssetGimmickDeadOperation_MAX = 3,
};

enum class ECharacterAssetGimmickKind {
    VFX = 0,
    MATERIAL = 1,
    ECharacterAssetGimmickKind_MAX = 2,
};

struct FCharacterAssetGimmickKey : public FCharacterAssetKeyDetails
{
    bool IgnoreSpawn;                                                                 // 0x0014 (size: 0x1)
    ECharacterAssetGimmickDeadOperation DeadOperation;                                // 0x0015 (size: 0x1)
    Unreal::TArray<int32> RandGimmickID;                                                      // 0x0018 (size: 0x10)
    int32 GimmickIDMirror;                                                            // 0x0028 (size: 0x4)
    int32 GimmickID;                                                                  // 0x002C (size: 0x4)
    ECharacterAssetGimmickKind GimmickKind;                                           // 0x0030 (size: 0x1)

}; // Size: 0x38

enum class ECharacterAssetPhysicsOverride {
    DEFAULT = 0,
    FORCE_APPLY = 1,
    FORCE_NON_APPLY = 2,
    ECharacterAssetPhysicsOverride_MAX = 3,
};

enum class ECharacterAssetMirrorAxisSetting {
    NONE = 0,
    X_YZ = 1,
    Y_ZX = 2,
    Z_XY = 3,
    ECharacterAssetMirrorAxisSetting_MAX = 4,
};

enum class ECharacterAssetPartsOffsetSetting {
    WORLD = 0,
    LOCAL = 1,
    ECharacterAssetPartsOffsetSetting_MAX = 2,
};

enum class ECharacterAssetPartsAttachSetting {
    BOTH = 0,
    POS_ONLY = 1,
    ROT_ONLY = 2,
    ECharacterAssetPartsAttachSetting_MAX = 3,
};

enum class ECharacterAssetPartsAttachTarget {
    OWN = 0,
    TARGET = 1,
    ECharacterAssetPartsAttachTarget_MAX = 2,
};

struct FCharacterAssetModelPartsKey : public FCharacterAssetKeyDetails
{
    ECharacterAssetPhysicsOverride PhysicsOverride;                                   // 0x0014 (size: 0x1)
    bool IsApplyPhysics;                                                              // 0x0015 (size: 0x1)
    bool Visible;                                                                     // 0x0016 (size: 0x1)
    int32 ValidStyle;                                                                 // 0x0018 (size: 0x4)
    int32 MirrorSettingOverride;                                                      // 0x001C (size: 0x4)
    int32 MotionEndFrame;                                                             // 0x0020 (size: 0x4)
    int32 MotionStartFrame;                                                           // 0x0024 (size: 0x4)
    int32 MotionID;                                                                   // 0x0028 (size: 0x4)
    float RotateOffsetZ;                                                              // 0x002C (size: 0x4)
    float RotateOffsetY;                                                              // 0x0030 (size: 0x4)
    float RotateOffsetX;                                                              // 0x0034 (size: 0x4)
    float PosOffsetZ;                                                                 // 0x0038 (size: 0x4)
    float PosOffsetY;                                                                 // 0x003C (size: 0x4)
    float PosOffsetX;                                                                 // 0x0040 (size: 0x4)
    int32 AttachedJoint;                                                              // 0x0044 (size: 0x4)
    int32 AttachJoint;                                                                // 0x0048 (size: 0x4)
    int32 PartsArrayIdx;                                                              // 0x004C (size: 0x4)
    int32 AttachPartsIndex;                                                           // 0x0050 (size: 0x4)
    ECharacterAssetMirrorAxisSetting LocalMirrorSetting;                              // 0x0054 (size: 0x1)
    ECharacterAssetPartsOffsetSetting OffsetSetting;                                  // 0x0055 (size: 0x1)
    ECharacterAssetPartsAttachSetting AttachSetting;                                  // 0x0056 (size: 0x1)
    ECharacterAssetPartsAttachTarget AttachTarget;                                    // 0x0057 (size: 0x1)

}; // Size: 0x58

enum class ECharacterAssetShotMatSetting {
    NO_INHERT = 0,
    INHERIT_SAME_COLOR = 1,
    USE_SAME_MATERIAL = 2,
    INHERIT_PARENT_SHOT = 3,
    ECharacterAssetShotMatSetting_MAX = 4,
};

enum class ECharacterAssetShotType {
    NORMAL = 0,
    BEAM = 1,
    MODEL = 2,
    DOUBLE = 3,
    OUROBOROS = 4,
    REALITY_GEM = 5,
    REALITY_BEAM = 6,
    REALITY_SURGE = 7,
    PARTNER = 8,
    ECharacterAssetShotType_MAX = 9,
};

enum class ECharacterAssetShotOperation {
    SPAWN = 0,
    CANCEL = 1,
    DELETE_ = 2,
    ECharacterAssetShotOperation_MAX = 3,
};

struct FCharacterAssetShotKey : public FCharacterAssetKeyDetails
{
    int32 ValidStyle;                                                                 // 0x0014 (size: 0x4)
    int32 FollowJointID;                                                              // 0x0018 (size: 0x4)
    ECharacterAssetShotMatSetting MaterialColorSetting;                               // 0x001C (size: 0x1)
    bool InheritHitData;                                                              // 0x001D (size: 0x1)
    bool ForceParentShot;                                                             // 0x001E (size: 0x1)
    bool IsFollowJoint;                                                               // 0x001F (size: 0x1)
    bool IsCoreCancel;                                                                // 0x0020 (size: 0x1)
    bool DemoCamera;                                                                  // 0x0021 (size: 0x1)
    bool IgnoreWorld;                                                                 // 0x0022 (size: 0x1)
    bool FollowOnDelete;                                                              // 0x0023 (size: 0x1)
    bool FollowOnSpawn;                                                               // 0x0024 (size: 0x1)
    bool IsDeleteActionChange;                                                        // 0x0025 (size: 0x1)
    bool IsDeleteDamage;                                                              // 0x0026 (size: 0x1)
    bool IgnoreVisibleMirror;                                                         // 0x0027 (size: 0x1)
    bool IsRotateDirectBase;                                                          // 0x0028 (size: 0x1)
    bool IgnoreCalcScale;                                                             // 0x0029 (size: 0x1)
    bool IgnoreCalcRotation;                                                          // 0x002A (size: 0x1)
    bool IgnoreCalcPosition;                                                          // 0x002B (size: 0x1)
    bool InheritParentPosZ;                                                           // 0x002C (size: 0x1)
    bool InheritParentPosY;                                                           // 0x002D (size: 0x1)
    bool InheritParentPosX;                                                           // 0x002E (size: 0x1)
    int32 BoneOffSetZ;                                                                // 0x0030 (size: 0x4)
    int32 BoneOffSetY;                                                                // 0x0034 (size: 0x4)
    int32 BoneOffSetX;                                                                // 0x0038 (size: 0x4)
    int32 RotOffSetZ;                                                                 // 0x003C (size: 0x4)
    int32 RotOffSetY;                                                                 // 0x0040 (size: 0x4)
    int32 RotOffSetX;                                                                 // 0x0044 (size: 0x4)
    int32 VisualOffSetZ;                                                              // 0x0048 (size: 0x4)
    int32 VisualOffSetY;                                                              // 0x004C (size: 0x4)
    int32 VisualOffSetX;                                                              // 0x0050 (size: 0x4)
    int32 ScatterZ;                                                                   // 0x0054 (size: 0x4)
    int32 ScatterY;                                                                   // 0x0058 (size: 0x4)
    int32 ScatterX;                                                                   // 0x005C (size: 0x4)
    int32 OffsetZ;                                                                    // 0x0060 (size: 0x4)
    int32 OffsetY;                                                                    // 0x0064 (size: 0x4)
    int32 OffsetX;                                                                    // 0x0068 (size: 0x4)
    bool IsMirrorMove;                                                                // 0x006C (size: 0x1)
    int32 DelayFrame;                                                                 // 0x0070 (size: 0x4)
    int32 StartMoveFrame;                                                             // 0x0074 (size: 0x4)
    int32 LifeTimeFrame;                                                              // 0x0078 (size: 0x4)
    int32 PartnerChara;                                                               // 0x007C (size: 0x4)
    ECharacterAssetShotType ShotType;                                                 // 0x0080 (size: 0x1)
    ECharacterAssetShotOperation Operation;                                           // 0x0081 (size: 0x1)
    int32 StyleIdx;                                                                   // 0x0084 (size: 0x4)
    int32 ActionId;                                                                   // 0x0088 (size: 0x4)

}; // Size: 0x8C

enum class ECharacterAssetLockType {
    NORMAL = 0,
    JOINT = 1,
    ECharacterAssetLockType_MAX = 2,
};

enum class ECharacterAssetPlaceAddPosAxis {
    NONE = 0,
    X = 1,
    Y = 2,
    Z = 3,
    XY = 4,
    YZ = 5,
    XZ = 6,
    XYZ = 7,
    ECharacterAssetPlaceAddPosAxis_MAX = 8,
};

struct FCharacterAssetBonePlaceInfo
{
    ECharacterAssetLockType LockType;                                                 // 0x0000 (size: 0x1)
    ECharacterAssetPlaceAddPosAxis IgnoreAxis;                                        // 0x0001 (size: 0x1)
    int32 TargetFrame;                                                                // 0x0004 (size: 0x4)
    int32 RotateBaseJoint;                                                            // 0x0008 (size: 0x4)
    int32 OffsetBaseJoint;                                                            // 0x000C (size: 0x4)
    int32 RotateZ;                                                                    // 0x0010 (size: 0x4)
    int32 RotateY;                                                                    // 0x0014 (size: 0x4)
    int32 RotateX;                                                                    // 0x0018 (size: 0x4)
    int32 OffsetZ;                                                                    // 0x001C (size: 0x4)
    int32 OffsetY;                                                                    // 0x0020 (size: 0x4)
    int32 OffsetX;                                                                    // 0x0024 (size: 0x4)
    int32 TargetJoint;                                                                // 0x0028 (size: 0x4)
    int32 AttachJoint;                                                                // 0x002C (size: 0x4)
    int32 LockActionNo;                                                               // 0x0030 (size: 0x4)

}; // Size: 0x34

struct FCharacterAssetPlaceAddPos
{
    int32 Value;                                                                      // 0x0000 (size: 0x4)
    ECharacterAssetPlaceAddPosAxis Axis;                                              // 0x0004 (size: 0x1)
    int32 Frame;                                                                      // 0x0008 (size: 0x4)

}; // Size: 0xC

struct FCharacterAssetBonePlaceKey : public FCharacterAssetKeyDetails
{
    FCharacterAssetBonePlaceInfo Info;                                                // 0x0014 (size: 0x34)
    Unreal::TArray<FCharacterAssetPlaceAddPos> AddPos;                                        // 0x0048 (size: 0x10)

}; // Size: 0x58

struct FCharacterAssetPlaceKey : public FCharacterAssetKeyDetails
{
    int32 PlaceRatioZ;                                                                // 0x0014 (size: 0x4)
    int32 PlaceRatioY;                                                                // 0x0018 (size: 0x4)
    int32 PlaceRatioX;                                                                // 0x001C (size: 0x4)
    Unreal::TArray<FCharacterAssetPlaceAddPos> AddPos;                                        // 0x0020 (size: 0x10)
    bool AddTargetSteerInJointLock;                                                   // 0x0030 (size: 0x1)
    bool VisualOffset;                                                                // 0x0031 (size: 0x1)

}; // Size: 0x38

enum class ECharacterAssetEventType {
    OWNER = 0,
    SYSTEM = 1,
    CHARA = 2,
    FIELD = 3,
    HUD = 4,
    CAMERA = 5,
    SOUND = 6,
    UNIQUE = 7,
    SHOT = 8,
    STORY = 9,
    ECharacterAssetEventType_MAX = 10,
};

struct FCharacterAssetLightKey : public FCharacterAssetKeyDetails
{
    bool OnlyOnce;                                                                    // 0x0014 (size: 0x1)
    ECharacterAssetEventType Type;                                                    // 0x0015 (size: 0x1)
    int32 ID;                                                                         // 0x0018 (size: 0x4)
    int32 SEID;                                                                       // 0x001C (size: 0x4)
    int32 Style;                                                                      // 0x0020 (size: 0x4)
    int32 Param01;                                                                    // 0x0024 (size: 0x4)
    int32 Param02;                                                                    // 0x0028 (size: 0x4)
    int32 Param03;                                                                    // 0x002C (size: 0x4)
    int32 Param04;                                                                    // 0x0030 (size: 0x4)
    int32 Param05;                                                                    // 0x0034 (size: 0x4)
    int32 Param06;                                                                    // 0x0038 (size: 0x4)
    int32 Param07;                                                                    // 0x003C (size: 0x4)
    int32 Param08;                                                                    // 0x0040 (size: 0x4)
    int32 Param09;                                                                    // 0x0044 (size: 0x4)
    int32 Param10;                                                                    // 0x0048 (size: 0x4)
    int32 Param11;                                                                    // 0x004C (size: 0x4)
    int32 Param12;                                                                    // 0x0050 (size: 0x4)
    int32 Param13;                                                                    // 0x0054 (size: 0x4)
    int32 Param14;                                                                    // 0x0058 (size: 0x4)

}; // Size: 0x5C

struct FCharacterAssetEventKey : public FCharacterAssetKeyDetails
{
    bool OnlyOnce;                                                                    // 0x0014 (size: 0x1)
    ECharacterAssetEventType Type;                                                    // 0x0015 (size: 0x1)
    int32 ID;                                                                         // 0x0018 (size: 0x4)
    int32 SEID;                                                                       // 0x001C (size: 0x4)
    int32 Style;                                                                      // 0x0020 (size: 0x4)
    int32 Param01;                                                                    // 0x0024 (size: 0x4)
    int32 Param02;                                                                    // 0x0028 (size: 0x4)
    int32 Param03;                                                                    // 0x002C (size: 0x4)
    int32 Param04;                                                                    // 0x0030 (size: 0x4)
    int32 Param05;                                                                    // 0x0034 (size: 0x4)
    int32 Param06;                                                                    // 0x0038 (size: 0x4)
    int32 Param07;                                                                    // 0x003C (size: 0x4)
    int32 Param08;                                                                    // 0x0040 (size: 0x4)
    int32 Param09;                                                                    // 0x0044 (size: 0x4)
    int32 Param10;                                                                    // 0x0048 (size: 0x4)
    int32 Param11;                                                                    // 0x004C (size: 0x4)
    int32 Param12;                                                                    // 0x0050 (size: 0x4)
    int32 Param13;                                                                    // 0x0054 (size: 0x4)
    int32 Param14;                                                                    // 0x0058 (size: 0x4)

}; // Size: 0x5C

enum class ECharacterAssetSteerTargetType {
    SELF = 0,
    PARENT = 1,
    RIVAL = 2,
    SCREEN = 3,
    NEAR_SHOT = 4,
    HIT_TARGET = 5,
    LOCK_TARGET = 6,
    WORLD = 7,
    RIVAL_COLLISION = 8,
    RIVAL_MASTER = 9,
    RIVAL_PARTNER = 10,
    OWNER_MASTER = 11,
    OWNER_PARTNER = 12,
    WORLD_MIRROR = 13,
    HIT_TARGET_COLLISION = 14,
    ECharacterAssetSteerTargetType_MAX = 15,
};

enum class ECharacterAssetSteerValType {
    VELOCITY_X = 0,
    VELOCITY_Y = 1,
    VELOCITY_Z = 2,
    ACCELERATION_X = 3,
    ACCELERATION_Y = 4,
    ACCELERATION_Z = 5,
    ECharacterAssetSteerValType_MAX = 6,
};

enum class ECharacterAssetSteerOpeType {
    NOP = 0,
    SET = 1,
    ADD = 2,
    MULTIPLY = 3,
    SET_SIGN = 4,
    ADD_SIGN = 5,
    SET_NEGATIVE_X = 6,
    SET_NEGATIVE_Y = 7,
    SET_NEGATIVE_Z = 8,
    SET_LIMIT_MIN = 9,
    SET_LIMIT_MAX = 10,
    SET_IGNORE = 11,
    SET_INHERIT = 12,
    SET_TARGET = 13,
    HOMING_VALUE = 14,
    HOMING_TIME = 15,
    ECharacterAssetSteerOpeType_MAX = 16,
};

struct FCharacterAssetSteerKey : public FCharacterAssetKeyDetails
{
    int32 Param;                                                                      // 0x0014 (size: 0x4)
    bool OnlyOnce;                                                                    // 0x0018 (size: 0x1)
    int8 MultiValueType;                                                              // 0x0019 (size: 0x1)
    ECharacterAssetSteerTargetType TargetType;                                        // 0x001A (size: 0x1)
    int32 LimitAngle;                                                                 // 0x001C (size: 0x4)
    int32 FixTargetOffsetZ;                                                           // 0x0020 (size: 0x4)
    int32 FixTargetOffsetY;                                                           // 0x0024 (size: 0x4)
    int32 FixTargetOffsetX;                                                           // 0x0028 (size: 0x4)
    int32 CalcValueFrame;                                                             // 0x002C (size: 0x4)
    int32 FixValue;                                                                   // 0x0030 (size: 0x4)
    ECharacterAssetSteerValType ValueType;                                            // 0x0034 (size: 0x1)
    ECharacterAssetSteerOpeType OperationType;                                        // 0x0035 (size: 0x1)

}; // Size: 0x38

enum class ECharacterAssetSwitchDoubleOperation {
    OPE_NONE = 0,
    DELETE_ = 1,
    DISP_ON = 2,
    DISP_OFF = 3,
    ECharacterAssetSwitchDoubleOperation_MAX = 4,
};

enum class ECharacterAssetSwitchSideOperation {
    OPE_NONE = 0,
    SET_SIDE = 1,
    SET_SIDE_INNER = 2,
    REVERSE_SIDE = 3,
    REVERSE_SIDE_INNER = 4,
    TARGET_SIDE = 5,
    TARGET_SIDE_INNER = 6,
    REVERSE_TARGET_SIDE = 7,
    REVERSE_TARGET_SIDE_INNER = 8,
    SET_SIDE_LEFT = 9,
    SET_SIDE_RIGHT = 10,
    PARENT_SIDE = 11,
    SCREEN_CENTER = 12,
    STAGE_CENTER = 13,
    VELOCITY_X = 14,
    REVERSE_VELOCITY_X = 15,
    HIT_TARGET_SIDE = 16,
    HIT_TARGET_SIDE_INNER = 17,
    REVERSE_HIT_TARGET_SIDE = 18,
    REVERSE_HIT_TARGET_SIDE_INNER = 19,
    ECharacterAssetSwitchSideOperation_MAX = 20,
};

enum class ECharacterAssetActionStatus {
    N_FOOTWORK = 0,
    N_WALK = 1,
    N_JUMP = 2,
    N_JUMP_NORM = 3,
    N_JUMP_LAND = 4,
    N_JUMP_RET = 5,
    N_SITD = 6,
    N_SIT = 7,
    N_STAND = 8,
    N_STAND_TURN = 9,
    N_SIT_TURN = 10,
    N_DASH = 11,
    N_NOKI = 12,
    N_PIYO = 13,
    N_SJUMP = 14,
    N_SJUMP_NORM = 15,
    N_SJUMP_DMG = 16,
    N_SJUMP_RET = 17,
    N_FALL = 18,
    N_GETUP = 19,
    N_DEF = 20,
    N_JDEF = 21,
    N_ATCK = 22,
    N_SPECIAL = 23,
    N_WIN = 24,
    N_SLEEP = 25,
    N_FLYING = 26,
    N_WJUMP = 27,
    N_WSJUMP = 28,
    N_TJUMP = 29,
    N_KDASH = 30,
    N_UKEMI = 31,
    N_TOUCH = 32,
    N_HYPER = 33,
    N_CATCH = 34,
    N_HOLD = 35,
    N_DAMAGE = 36,
    N_FRAME_IN = 37,
    N_GEM = 38,
    N_WITHDRAW = 39,
    N_NONE = 40,
    ECharacterAssetActionStatus_MAX = 41,
};

enum class ECharacterAssetSwitchJump {
    JUMP_ST_NONE = 0,
    JUMP_ST_NORMAL = 1,
    JUMP_ST_SJUMP_UP = 2,
    JUMP_ST_SJUMP_DOWN = 3,
    ECharacterAssetSwitchJump_MAX = 4,
};

enum class ECharacterAssetSwitchPose {
    POSE_ST_NONE = 0,
    POSE_ST_STAND = 1,
    POSE_ST_CROUCH = 2,
    POSE_ST_JUMP = 3,
    POSE_ST_DOWN = 4,
    ECharacterAssetSwitchPose_MAX = 5,
};

struct FCharacterAssetSwitchGrdTouch
{
    bool RightFootOff;                                                                // 0x0000 (size: 0x1)
    bool LeftFootOff;                                                                 // 0x0001 (size: 0x1)
    bool RightFoot;                                                                   // 0x0002 (size: 0x1)
    bool LeftFoot;                                                                    // 0x0003 (size: 0x1)

}; // Size: 0x4

struct FCharacterAssetSwitchLookAt
{
    bool HeadReset;                                                                   // 0x0000 (size: 0x1)
    bool EyesReset;                                                                   // 0x0001 (size: 0x1)
    bool HeadOFF;                                                                     // 0x0002 (size: 0x1)
    bool EyesOFF;                                                                     // 0x0003 (size: 0x1)
    bool HeadON;                                                                      // 0x0004 (size: 0x1)
    bool EyesON;                                                                      // 0x0005 (size: 0x1)

}; // Size: 0x6

struct FCharacterAssetSwitchInvisible
{
    bool HUD;                                                                         // 0x0000 (size: 0x1)
    bool Target;                                                                      // 0x0001 (size: 0x1)
    bool Body;                                                                        // 0x0002 (size: 0x1)
    bool Owner;                                                                       // 0x0003 (size: 0x1)

}; // Size: 0x4

struct FCharacterAssetSwitchKey : public FCharacterAssetKeyDetails
{
    bool IgnoreTargetChange;                                                          // 0x0014 (size: 0x1)
    bool TouchDisable;                                                                // 0x0015 (size: 0x1)
    bool IgnoreTurbo;                                                                 // 0x0016 (size: 0x1)
    bool IgnoreTouchWait;                                                             // 0x0017 (size: 0x1)
    bool ThrowEscape;                                                                 // 0x0018 (size: 0x1)
    bool WaitLanding;                                                                 // 0x0019 (size: 0x1)
    bool DashJumpEnable;                                                              // 0x001A (size: 0x1)
    bool IgnoreInertia;                                                               // 0x001B (size: 0x1)
    bool DamageAbsorb;                                                                // 0x001C (size: 0x1)
    bool ShotInvincible;                                                              // 0x001D (size: 0x1)
    bool SuperArmor;                                                                  // 0x001E (size: 0x1)
    bool DamageResistance;                                                            // 0x001F (size: 0x1)
    bool SpaceSurge;                                                                  // 0x0020 (size: 0x1)
    bool IgnoreInput;                                                                 // 0x0021 (size: 0x1)
    bool IgnoreBodyPush;                                                              // 0x0022 (size: 0x1)
    bool TakeBehind;                                                                  // 0x0023 (size: 0x1)
    bool NotToTarget;                                                                 // 0x0024 (size: 0x1)
    bool NonSHCAdd;                                                                   // 0x0025 (size: 0x1)
    bool ApplyRuthlessWall;                                                           // 0x0026 (size: 0x1)
    bool NonScrollPush;                                                               // 0x0027 (size: 0x1)
    bool NonBodyPush;                                                                 // 0x0028 (size: 0x1)
    bool NonShotCheck;                                                                // 0x0029 (size: 0x1)
    bool NonHitCheck;                                                                 // 0x002A (size: 0x1)
    bool WorldCancel;                                                                 // 0x002B (size: 0x1)
    bool HitStopCancel;                                                               // 0x002C (size: 0x1)
    bool Landing;                                                                     // 0x002D (size: 0x1)
    bool ToNormal;                                                                    // 0x002E (size: 0x1)
    bool LandToNormal;                                                                // 0x002F (size: 0x1)
    bool FollowParent;                                                                // 0x0030 (size: 0x1)
    bool UpdateScrollFlag;                                                            // 0x0031 (size: 0x1)
    ECharacterAssetSwitchDoubleOperation Double;                                      // 0x0032 (size: 0x1)
    ECharacterAssetSwitchSideOperation Side;                                          // 0x0033 (size: 0x1)
    ECharacterAssetActionStatus ActionStatus;                                         // 0x0034 (size: 0x1)
    ECharacterAssetSwitchJump JumpStatus;                                             // 0x0035 (size: 0x1)
    ECharacterAssetSwitchPose PoseStatus;                                             // 0x0036 (size: 0x1)
    FCharacterAssetSwitchGrdTouch GroundTouch;                                        // 0x0037 (size: 0x4)
    FCharacterAssetSwitchLookAt LookAt;                                               // 0x003B (size: 0x6)
    FCharacterAssetSwitchInvisible Invisible;                                         // 0x0041 (size: 0x4)

}; // Size: 0x48

enum class ECharacterAssetBranchType {
    ALWAYS = 0,
    ELSE = 1,
    HIT = 2,
    HIT_CHARA_ID = 3,
    GUARD = 4,
    SWING = 5,
    HIT_ATEMI = 6,
    BODY_PUSH = 7,
    POSE_STATUS = 8,
    ACTION_STATUS = 9,
    ACTION_CATEGORY = 10,
    ACTION_ID = 11,
    HEIGHT = 12,
    LANDING = 13,
    WALL_F = 14,
    WALL_B = 15,
    OBSTACLE = 16,
    OBSTACLE_Y = 17,
    SIDE = 18,
    RANGE = 19,
    STEER = 20,
    TRIGGER = 21,
    COMMAND = 22,
    ARMOR = 23,
    ATEMI = 24,
    TARGET_LOSS = 25,
    CHARA_ID = 26,
    PARTNER_NO = 27,
    FIGHT_STYLE = 28,
    UNIQUE_PARAM = 29,
    UNIQUE_TIMER = 30,
    LOOP_COUNT = 31,
    VITAL = 32,
    GEM_GAUGE = 33,
    GEM_KIND = 34,
    FINISH = 35,
    DURABILITY = 36,
    GEM_SPACE = 37,
    RANDOM = 38,
    CATCH_F = 39,
    CATCH_B = 40,
    CATCH_N = 41,
    HIT_CATCH = 42,
    NEGATIVE_X = 43,
    NEGATIVE_Y = 44,
    AREA_PARENT = 45,
    AREA_TARGET = 46,
    AREA_RIVAL_TEAM = 47,
    SHOT_HIT = 48,
    SHOT_DAMAGE = 49,
    SHOT_REFLECT = 50,
    SHOT_HIT_COUNT_0 = 51,
    ECharacterAssetBranchType_MAX = 52,
};

struct FCharacterAssetBranchKey : public FCharacterAssetKeyDetails
{
    int32 Param03;                                                                    // 0x0014 (size: 0x4)
    int32 Param02;                                                                    // 0x0018 (size: 0x4)
    int32 Param01;                                                                    // 0x001C (size: 0x4)
    int32 Param00;                                                                    // 0x0020 (size: 0x4)
    int32 ActionFrame;                                                                // 0x0024 (size: 0x4)
    int32 Action;                                                                     // 0x0028 (size: 0x4)
    int32 ActionList;                                                                 // 0x002C (size: 0x4)
    bool IsChangeStyle;                                                               // 0x0030 (size: 0x1)
    bool InheritFrame;                                                                // 0x0031 (size: 0x1)
    bool InheritLoopCount;                                                            // 0x0032 (size: 0x1)
    ECharacterAssetBranchType Type;                                                   // 0x0033 (size: 0x1)

}; // Size: 0x34

enum class ECharacterAssetSoundAisacType {
    PAN = 0,
    PITCH = 1,
    VOLUME = 2,
    PAN3D = 3,
    EQUALIZER = 4,
    REVERB = 5,
    PAN_HUD = 6,
    ECharacterAssetSoundAisacType_MAX = 7,
};

enum class ECharacterAssetSoundGroupID {
    DEFAULT = 0,
    CHANGESTATE = 1,
    CHANGEACTION = 2,
    STUN = 3,
    ECharacterAssetSoundGroupID_MAX = 4,
};

enum class ECharacterAssetSoundType {
    SYSTEM = 0,
    COMMON = 1,
    CHARA_SE = 2,
    CHARA_VOICE = 3,
    FOOT = 4,
    ECharacterAssetSoundType_MAX = 5,
};

enum class ECharacterAssetSoundOpeType {
    PLAYCUE = 0,
    STOPCUE = 1,
    VOLUMECONTROL = 2,
    PITCHCONTROL = 3,
    AISACCONTROL = 4,
    ECharacterAssetSoundOpeType_MAX = 5,
};

struct FCharacterAssetSEKey : public FCharacterAssetKeyDetails
{
    bool OnlyOnce;                                                                    // 0x0014 (size: 0x1)
    float ControlValue;                                                               // 0x0018 (size: 0x4)
    ECharacterAssetSoundAisacType AISACType;                                          // 0x001C (size: 0x1)
    ECharacterAssetSoundGroupID GroupID;                                              // 0x001D (size: 0x1)
    int32 CueID;                                                                      // 0x0020 (size: 0x4)
    ECharacterAssetSoundType Type;                                                    // 0x0024 (size: 0x1)
    ECharacterAssetSoundOpeType OperationType;                                        // 0x0025 (size: 0x1)

}; // Size: 0x28

struct FCharacterAssetVoiceKey : public FCharacterAssetKeyDetails
{
    bool OnlyOnce;                                                                    // 0x0014 (size: 0x1)
    float ControlValue;                                                               // 0x0018 (size: 0x4)
    ECharacterAssetSoundAisacType AISACType;                                          // 0x001C (size: 0x1)
    ECharacterAssetSoundGroupID GroupID;                                              // 0x001D (size: 0x1)
    int32 CueID;                                                                      // 0x0020 (size: 0x4)
    ECharacterAssetSoundType Type;                                                    // 0x0024 (size: 0x1)
    ECharacterAssetSoundOpeType OperationType;                                        // 0x0025 (size: 0x1)

}; // Size: 0x28

struct FCharacterAssetAutoTriggerKey : public FCharacterAssetKeyDetails
{
    int32 TriggerID;                                                                  // 0x0014 (size: 0x4)

}; // Size: 0x18

struct FCharacterAssetTriggerKey : public FCharacterAssetKeyDetails
{
    bool GemAbility;                                                                  // 0x0014 (size: 0x1)
    bool Wall;                                                                        // 0x0015 (size: 0x1)
    bool Fly;                                                                         // 0x0016 (size: 0x1)
    bool Defer;                                                                       // 0x0017 (size: 0x1)
    bool Master;                                                                      // 0x0018 (size: 0x1)
    bool SuperJump;                                                                   // 0x0019 (size: 0x1)
    bool NormalJump;                                                                  // 0x001A (size: 0x1)
    bool Armor;                                                                       // 0x001B (size: 0x1)
    bool Swing;                                                                       // 0x001C (size: 0x1)
    bool Guard;                                                                       // 0x001D (size: 0x1)
    bool Hit;                                                                         // 0x001E (size: 0x1)
    int32 TriggerGroup;                                                               // 0x0020 (size: 0x4)

}; // Size: 0x24

enum class ECharacterAssetMergeType {
    PREVIOUS = 0,
    NEXT = 1,
    STARTSTATE = 2,
    ENDSTATE = 3,
    SMOOTH = 4,
    ECharacterAssetMergeType_MAX = 5,
};

struct FCharacterAssetFacialMergeKey : public FCharacterAssetKeyDetails
{
    float DeltaEnd;                                                                   // 0x0014 (size: 0x4)
    float Delta;                                                                      // 0x0018 (size: 0x4)
    bool isPoseMerge;                                                                 // 0x001C (size: 0x1)
    bool isPlaceMerge;                                                                // 0x001D (size: 0x1)
    ECharacterAssetMergeType Type;                                                    // 0x001E (size: 0x1)

}; // Size: 0x20

enum class ECharacterAssetMergeTarget {
    OBJECT = 0,
    CAMERA = 1,
    FACIAL = 2,
    ECharacterAssetMergeTarget_MAX = 3,
};

struct FCharacterAssetMergeKey : public FCharacterAssetKeyDetails
{
    float DeltaEnd;                                                                   // 0x0014 (size: 0x4)
    float Delta;                                                                      // 0x0018 (size: 0x4)
    bool isPoseMerge;                                                                 // 0x001C (size: 0x1)
    bool isPlaceMerge;                                                                // 0x001D (size: 0x1)
    ECharacterAssetMergeTarget Target;                                                // 0x001E (size: 0x1)
    ECharacterAssetMergeType Type;                                                    // 0x001F (size: 0x1)

}; // Size: 0x20

enum class ECamTargetType {
    NONE = 0,
    OWNER = 1,
    OPPONENT = 2,
    WORLD = 3,
    BATTLE_CAMERA = 4,
    ECamTargetType_MAX = 5,
};

struct FCharacterAssetCameraKey : public FCharacterAssetKeyDetails
{
    float EaseCurveP1;                                                                // 0x0014 (size: 0x4)
    float EaseCurveP0;                                                                // 0x0018 (size: 0x4)
    bool IsIntLocal;                                                                  // 0x001C (size: 0x1)
    ECamTargetType IntTargetType;                                                     // 0x001D (size: 0x1)
    int32 IntJointNo;                                                                 // 0x0020 (size: 0x4)
    int32 IntOffsetZ;                                                                 // 0x0024 (size: 0x4)
    int32 IntOffsetY;                                                                 // 0x0028 (size: 0x4)
    int32 IntOffsetX;                                                                 // 0x002C (size: 0x4)
    bool IsPosLocal;                                                                  // 0x0030 (size: 0x1)
    ECamTargetType PosTargetType;                                                     // 0x0031 (size: 0x1)
    int32 PosJointNo;                                                                 // 0x0034 (size: 0x4)
    int32 PosOffsetZ;                                                                 // 0x0038 (size: 0x4)
    int32 PosOffsetY;                                                                 // 0x003C (size: 0x4)
    int32 PosOffsetX;                                                                 // 0x0040 (size: 0x4)
    bool VisibleOther;                                                                // 0x0044 (size: 0x1)
    bool VisibleTarget;                                                               // 0x0045 (size: 0x1)
    bool IsDemoStop;                                                                  // 0x0046 (size: 0x1)
    bool IsMainPlayerOnly;                                                            // 0x0047 (size: 0x1)
    float Roll;                                                                       // 0x0048 (size: 0x4)
    float FOV;                                                                        // 0x004C (size: 0x4)
    int32 EndHokan;                                                                   // 0x0050 (size: 0x4)
    int32 HokanFrame;                                                                 // 0x0054 (size: 0x4)

}; // Size: 0x58

enum class ECharacterAssetEyeOffsetType {
    LEFT = 0,
    RIGHT = 1,
    BOTH = 2,
    ECharacterAssetEyeOffsetType_MAX = 3,
};

enum class ECharacterAssetFacialType {
    FACIAL_MOTION = 0,
    EYE_UV_OFFSET = 1,
    SOUND_FACIAL = 2,
    ECharacterAssetFacialType_MAX = 3,
};

struct FCharacterAssetFacialKey : public FCharacterAssetKeyDetails
{
    int32 UVInterpolateFrame;                                                         // 0x0014 (size: 0x4)
    float UVOffsetV;                                                                  // 0x0018 (size: 0x4)
    float UVOffsetU;                                                                  // 0x001C (size: 0x4)
    ECharacterAssetEyeOffsetType UVTargetType;                                        // 0x0020 (size: 0x1)
    ECharacterAssetFacialType FacialType;                                             // 0x0021 (size: 0x1)
    bool GetAppearSlot;                                                               // 0x0022 (size: 0x1)
    bool SameBodyFrame;                                                               // 0x0023 (size: 0x1)
    bool Mirr0000;                                                                    // 0x0024 (size: 0x1)
    int32 MotionEndFrame;                                                             // 0x0028 (size: 0x4)
    int32 MotionStartFrame;                                                           // 0x002C (size: 0x4)
    int32 MotionID;                                                                   // 0x0030 (size: 0x4)

}; // Size: 0x34

enum class ECharacterAssetMotType {
    COMMONOBJECT = 0,
    UNIQUEOBJECT = 1,
    UNIQUECAMERA = 2,
    UNIQUEFACIAL = 3,
    TARGETOBJECT = 4,
    OPTIONOBJECT = 5,
    COMMONCAMERA = 6,
    ECharacterAssetMotType_MAX = 7,
};

enum class ECharacterAssetMotOperation {
    SOURCE_MOTION = 0,
    BLEND_MOTION = 1,
    ECharacterAssetMotOperation_MAX = 2,
};

struct FCharacterAssetMotionKey : public FCharacterAssetKeyDetails
{
    Unreal::TArray<int32> BlendBoneSet;                                                       // 0x0018 (size: 0x10)
    ECharacterAssetPhysicsOverride PhysicsOverride;                                   // 0x0028 (size: 0x1)
    bool IsApplyPhysics;                                                              // 0x0029 (size: 0x1)
    bool IsMirrorParts;                                                               // 0x002A (size: 0x1)
    bool IsContainFacial;                                                             // 0x002B (size: 0x1)
    bool IsVisibleAllChara;                                                           // 0x002C (size: 0x1)
    bool IsVisibleOhterChara;                                                         // 0x002D (size: 0x1)
    bool MoveValueZ;                                                                  // 0x002E (size: 0x1)
    bool MoveValueY;                                                                  // 0x002F (size: 0x1)
    bool MoveValueX;                                                                  // 0x0030 (size: 0x1)
    bool IsTurnAround;                                                                // 0x0031 (size: 0x1)
    bool Mirr0000;                                                                    // 0x0032 (size: 0x1)
    int32 MotionEndFrame;                                                             // 0x0034 (size: 0x4)
    int32 MotionStartFrame;                                                           // 0x0038 (size: 0x4)
    int32 MotionID;                                                                   // 0x003C (size: 0x4)
    ECharacterAssetMotType MotionType;                                                // 0x0040 (size: 0x1)
    ECharacterAssetMotOperation Operation;                                            // 0x0041 (size: 0x1)

}; // Size: 0x48

struct FAttackDataKey : public FCharacterAssetKeyDetails
{
    bool Swing;                                                                       // 0x0014 (size: 0x1)
    bool Guard;                                                                       // 0x0015 (size: 0x1)
    bool Hit;                                                                         // 0x0016 (size: 0x1)
    int32 ValidStyle;                                                                 // 0x0018 (size: 0x4)
    int32 AttackDataListIndex;                                                        // 0x001C (size: 0x4)

}; // Size: 0x20

struct FUniqueCollisionKey : public FCharacterAssetKeyDetails
{
    int8 Value;                                                                       // 0x0014 (size: 0x1)
    uint8 Operation;                                                                  // 0x0015 (size: 0x1)
    uint8 ParamIndex;                                                                 // 0x0016 (size: 0x1)
    uint8 CheckType;                                                                  // 0x0017 (size: 0x1)
    int32 CheckFlags;                                                                 // 0x0018 (size: 0x4)
    int32 CollisionListIndex;                                                         // 0x001C (size: 0x4)

}; // Size: 0x20

struct FAttackCollisionKey : public FCharacterAssetKeyDetails
{
    int32 CollisionListIndex;                                                         // 0x0014 (size: 0x4)

}; // Size: 0x18

struct FPushCollisionKey : public FCharacterAssetKeyDetails
{
    int32 CollisionListIndex;                                                         // 0x0014 (size: 0x4)

}; // Size: 0x18

struct FDamageCollisionKey : public FCharacterAssetKeyDetails
{
    int32 CollisionListIndex;                                                         // 0x0014 (size: 0x4)

}; // Size: 0x18

struct FCharacterAssetAction
{
    int32 Frame;                                                                      // 0x0000 (size: 0x4)
    int32 No;                                                                         // 0x0004 (size: 0x4)
    ECharacterAssetActionGroup Group;                                                 // 0x0008 (size: 0x1)
    FCharacterAssetActionProjectile Projectile;                                       // 0x000C (size: 0x4)
    FCharacterAssetActionInherit Inherit;                                             // 0x0010 (size: 0x1C)
    FCharacterAssetActionState State;                                                 // 0x002C (size: 0xC)
    FCharacterAssetActionFrame ActionFrame;                                           // 0x0038 (size: 0xC)
    FCharacterAssetActionRange Range;                                                 // 0x0044 (size: 0x8)
    Unreal::TArray<FCharacterAssetSoundKey> SoundKey;                                         // 0x0050 (size: 0x10)
    Unreal::TArray<FCharacterAssetGimmickKey> GimmickKey;                                     // 0x0060 (size: 0x10)
    Unreal::TArray<FCharacterAssetModelPartsKey> ModelPartsKey;                               // 0x0070 (size: 0x10)
    Unreal::TArray<FCharacterAssetShotKey> ShotKey;                                           // 0x0080 (size: 0x10)
    Unreal::TArray<FCharacterAssetBonePlaceKey> BonePlaceKey;                                 // 0x0090 (size: 0x10)
    Unreal::TArray<FCharacterAssetPlaceKey> PlaceKey;                                         // 0x00A0 (size: 0x10)
    Unreal::TArray<FCharacterAssetLightKey> LightKey;                                         // 0x00B0 (size: 0x10)
    Unreal::TArray<FCharacterAssetEventKey> EventKey;                                         // 0x00C0 (size: 0x10)
    Unreal::TArray<FCharacterAssetSteerKey> SteerKey;                                         // 0x00D0 (size: 0x10)
    Unreal::TArray<FCharacterAssetSwitchKey> SwitchKey;                                       // 0x00E0 (size: 0x10)
    Unreal::TArray<FCharacterAssetBranchKey> BranchKey;                                       // 0x00F0 (size: 0x10)
    Unreal::TArray<FCharacterAssetSEKey> SEKey;                                               // 0x0100 (size: 0x10)
    Unreal::TArray<FCharacterAssetVoiceKey> VoiceKey;                                         // 0x0110 (size: 0x10)
    Unreal::TArray<FCharacterAssetAutoTriggerKey> AutoTriggerKey;                             // 0x0120 (size: 0x10)
    Unreal::TArray<FCharacterAssetTriggerKey> TriggerKey;                                     // 0x0130 (size: 0x10)
    Unreal::TArray<FCharacterAssetFacialMergeKey> FacialMergeKey;                             // 0x0140 (size: 0x10)
    Unreal::TArray<FCharacterAssetMergeKey> MergeKey;                                         // 0x0150 (size: 0x10)
    Unreal::TArray<FCharacterAssetCameraKey> CameraKey;                                       // 0x0160 (size: 0x10)
    Unreal::TArray<FCharacterAssetFacialKey> FacialKey;                                       // 0x0170 (size: 0x10)
    Unreal::TArray<FCharacterAssetMotionKey> MotionKey;                                       // 0x0180 (size: 0x10)
    Unreal::TArray<FAttackDataKey> AttackDataKeyList;                                         // 0x0190 (size: 0x10)
    Unreal::TArray<FUniqueCollisionKey> UniqueCollisionKeyList;                               // 0x01A0 (size: 0x10)
    Unreal::TArray<FAttackCollisionKey> AttackCollisionKeyList;                               // 0x01B0 (size: 0x10)
    Unreal::TArray<FDamageCollisionKey> DamageCollisionKeyList;                               // 0x01C0 (size: 0x10)
    Unreal::TArray<FPushCollisionKey> PushCollisionKeyList;                                   // 0x01D0 (size: 0x10)

}; // Size: 0x1E0

struct FCharacterAssetStyle
{
    FStyleData StyleData;                                                             // 0x0000 (size: 0x68)
    Unreal::TArray<FCharacterAssetAction> ActionList;                                         // 0x0068 (size: 0x10)

}; // Size: 0x78

struct FHitPatternUnique
{
    Unreal::TArray<int32> UniqueList;                                                         // 0x0000 (size: 0x10)

}; // Size: 0x10

struct FHitPatternPush
{
    Unreal::TArray<int32> PushList;                                                           // 0x0000 (size: 0x10)

}; // Size: 0x10

struct FHitPatternDamage
{
    Unreal::TArray<int32> HeadList;                                                           // 0x0000 (size: 0x10)
    Unreal::TArray<int32> BodyList;                                                           // 0x0010 (size: 0x10)
    Unreal::TArray<int32> LegList;                                                            // 0x0020 (size: 0x10)
    Unreal::TArray<int32> CaughtList;                                                         // 0x0030 (size: 0x10)

}; // Size: 0x40

struct FHitPatternAttack
{
    Unreal::TArray<int32> AttackList;                                                         // 0x0000 (size: 0x10)
    Unreal::TArray<int32> CatchList;                                                          // 0x0010 (size: 0x10)
    Unreal::TArray<int32> AtemiList;                                                          // 0x0020 (size: 0x10)

}; // Size: 0x30

class UCharacterAsset : public UObject
{
public:
    uint8 padding[0x28];
    Unreal::TArray<FProjectileData> ProjectileDataList;                                       // 0x0028 (size: 0x10)
    Unreal::TArray<FAttackData> AttackDataList;                                               // 0x0038 (size: 0x10)
    Unreal::TArray<FHitRectData> UniqueCollisionParamList;                                    // 0x0048 (size: 0x10)
    Unreal::TArray<FHitRectData> PushCollisionParamList;                                      // 0x0058 (size: 0x10)
    Unreal::TArray<FHitRectData> CaughtCollisionParamList;                                    // 0x0068 (size: 0x10)
    Unreal::TArray<FHitRectData> LegCollisionParamList;                                       // 0x0078 (size: 0x10)
    Unreal::TArray<FHitRectData> BodyCollisionParamList;                                      // 0x0088 (size: 0x10)
    Unreal::TArray<FHitRectData> HeadCollisionParamList;                                      // 0x0098 (size: 0x10)
    Unreal::TArray<FHitRectData> AtemiCollisionParamList;                                     // 0x00A8 (size: 0x10)
    Unreal::TArray<FHitRectData> CatchCollisionParamList;                                     // 0x00B8 (size: 0x10)
    Unreal::TArray<FHitRectData> AttackCollisionParamList;                                    // 0x00C8 (size: 0x10)
    Unreal::TArray<FCharacterAssetCommandGroup> CommandGroup;                                 // 0x00D8 (size: 0x10)
    Unreal::TArray<FCharacterAssetTriggerGroup> TriggerGroup;                                 // 0x00E8 (size: 0x10)
    Unreal::TArray<FCharacterAssetTrigger> Trigger;                                           // 0x00F8 (size: 0x10)
    Unreal::TArray<FCharacterAssetStyle> StyleList;                                           // 0x0108 (size: 0x10)
    Unreal::TArray<FHitPatternUnique> UniqueCollisionIndexList;                               // 0x0118 (size: 0x10)
    Unreal::TArray<FHitPatternPush> PushCollisionIndexList;                                   // 0x0128 (size: 0x10)
    Unreal::TArray<FHitPatternDamage> DamageCollisionIndexList;                               // 0x0138 (size: 0x10)
    Unreal::TArray<FHitPatternAttack> AttackCollisionIndexList;                               // 0x0148 (size: 0x10)
    int32 BattleVersion = 0;                                                              // 0x0158 (size: 0x4)
    int32 VersionMMDDHHMM = 0;                                                            // 0x015C (size: 0x4)
    int32 VersionYYYY = 0;                                                                // 0x0160 (size: 0x4)

}; // Size: 0x168
