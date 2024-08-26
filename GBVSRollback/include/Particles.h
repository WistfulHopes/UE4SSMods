#pragma once

#include <map>
#include <Unreal/UActorComponent.hpp>
#include "CXXBYTE.h"
#include "struct_util.h"

template <class T>
inline void hash_combine(std::size_t& s, const T& v)
{
    std::hash<T> h;
    s ^= h(v) + 0x9e3779b9 + (s << 6) + (s >> 2);
}

struct FPSCCacheKey
{
    int player;
    class OBJ_CBase* obj;
    CXXBYTE<32> actname;
    CXXBYTE<32> particleName;

    bool operator<(const FPSCCacheKey& rhs) const noexcept
    {
        constexpr std::hash<std::string> hasher;
        auto thisActHash = hasher(actname.m_Buf);
        return this->player + reinterpret_cast<uintptr_t>(&obj) + hasher(actname.m_Buf) + hasher(particleName.m_Buf) <
            rhs.player + reinterpret_cast<uintptr_t>(&rhs.obj) + hasher(rhs.actname.m_Buf) + hasher(rhs.particleName.m_Buf);
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
    FIELD(0x7B8, class OBJ_CBase*, LinkObjPtr);
    FIELD(0x7C4, unsigned int, PointLightId);
};

struct Particle_RollbackData
{
    std::map<FPSCCacheKey, UParticleSystemComponent*> pscCache;
    std::map<FPSCCacheKey, UParticleSystemComponent*> unlinkedPscCache;
};

extern Particle_RollbackData rollbackData;

FPSCCacheKey MakeUnlinkedPSCKey(class AREDGameState_Battle* GameState, class OBJ_CBase* obj, CXXBYTE<32>* name);
UParticleSystemComponent* GetCachedPSCForSet(class OBJ_CBase* obj);
bool Rollback_ProcessCachedPSC(class OBJ_CBase* obj, const CXXBYTE<32>* name, int objType, bool useArg);
void Rollback_OnLinkParticle(class OBJ_CBase* obj, const CXXBYTE<32>* name, int objType, bool useArg);
void AddLinkPSCToCache(class OBJ_CBase* obj);
