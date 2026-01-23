#pragma once

#include <map>
#include <Unreal/UActorComponent.hpp>
#include "CXXBYTE.hpp"
#include "struct_util.hpp"

struct FPSCCacheKey
{
    int player;
    class OBJ_CBase* obj;
    int GameFrame;
    CXXBYTE<32> actname;
    CXXBYTE<32> particleName;

    bool operator<(const FPSCCacheKey& rhs) const noexcept
    {
        constexpr std::hash<std::string> hasher;
        
        if (player < rhs.player) return true;
        if (player > rhs.player) return false;
        
        if (reinterpret_cast<uintptr_t>(obj) < reinterpret_cast<uintptr_t>(rhs.obj)) return true;
        if (reinterpret_cast<uintptr_t>(obj) > reinterpret_cast<uintptr_t>(rhs.obj)) return false;
        
        if (GameFrame < rhs.GameFrame) return true;
        if (GameFrame > rhs.GameFrame) return false;
        
        if (hasher(actname.m_Buf) < hasher(rhs.actname.m_Buf)) return true;
        if (hasher(actname.m_Buf) > hasher(rhs.actname.m_Buf)) return false;
        
        return hasher(particleName.m_Buf) < hasher(rhs.particleName.m_Buf);
    }
};

struct CCreateArg
{
    CXXBYTE<32> m_CreateArg_SocketName;
    int m_CreateArg_Angle;
    int m_CreateArg_AngleY;
    int m_CreateArg_OffsetPosX;
    int m_CreateArg_OffsetPosY;
    int m_CreateArg_OffsetPosZ;
    int m_CreateArg_ScaleX;
    int m_CreateArg_ScaleY;
    int m_CreateArg_ScaleZ;
    int m_CreateArg_Hikitsugi0;
    unsigned int m_CreateArg_HkrColor;
    unsigned int m_CreateArg_MltColor;
    int m_CreateArg_TransPriority;
    uint8_t m_CreateArg_Direction;
    bool m_CreateArg_SocketUse;
    bool m_CreateArg_SocketWithRot;
    bool m_CreateArg_NoAssert;
    int m_CreateArg_PointLightSide;
    int m_CreateArg_PointLightMember;
};

class UParticleSystem : public RC::Unreal::UObject
{
public:
    FIELD(0x34, float, WarmupTime);
    FIELD(0x38, float, WarmupTickRate);
    FIELD(0xC0, float, Delay);
};

class UParticleSystemComponent : public RC::Unreal::USceneComponent
{
public:
    BIT_FIELD(0x59A, 0x20, bAutoDestroy);
    FIELD(0x658, float, CustomTimeDilation);
    FIELD(0x7B8, class OBJ_CBase*, LinkObjPtr);
    FIELD(0x7C0, bool, bPause);
    FIELD(0x7C4, unsigned int, PointLightId);
    
    static RC::Unreal::UClass* StaticClass;
};

struct Particle_RollbackData
{
    std::map<FPSCCacheKey, UParticleSystemComponent*> pscCache;
    std::map<FPSCCacheKey, UParticleSystemComponent*> unlinkedPscCache;
    std::map<FPSCCacheKey, UParticleSystemComponent*> unlinkedPscCacheUsed;
};

extern Particle_RollbackData rollbackData;

FPSCCacheKey MakeUnlinkedPSCKey(OBJ_CBase* obj, CXXBYTE<32>* name);
void AddUnlinkedPSCToCache(OBJ_CBase* obj, CXXBYTE<32>* name, UParticleSystemComponent* psc);
bool UseUnlinkedPSC(OBJ_CBase* obj, CXXBYTE<32>* name);
UParticleSystemComponent* GetCachedPSC(OBJ_CBase* obj, CXXBYTE<32>* name);
UParticleSystemComponent* GetCachedPSCForSet(OBJ_CBase* obj);
bool Rollback_ProcessCachedPSC(OBJ_CBase* obj, CXXBYTE<32>* name);
bool Rollback_ProcessCachedUnlinkedPSC(OBJ_CBase* obj, CXXBYTE<32>* name);
void Rollback_OnLinkParticle(OBJ_CBase* obj, CXXBYTE<32>* name);
bool AddLinkPSCToCache(OBJ_CBase* obj);
void DeleteLinkPSC_Hook(OBJ_CBase* pThis);