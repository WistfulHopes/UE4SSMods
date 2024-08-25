#pragma once

#include <map>
#include <Unreal/UActorComponent.hpp>
#include "CXXBYTE.h"
#include "struct_util.h"

template <class T>
inline void hash_combine(std::size_t & s, const T & v)
{
	std::hash<T> h;
	s^= h(v) + 0x9e3779b9 + (s<< 6) + (s>> 2);
}

struct FPSCCacheKey
{
	int player;
	class OBJ_CBase* obj;
	CXXBYTE<32> actname;
	CXXBYTE<32> particleName;
	bool operator<(const FPSCCacheKey& rhs) const noexcept
	{
		return this->player + reinterpret_cast<uintptr_t>(&obj) + this->actname.m_Buf[0] + this->particleName.m_Buf[0] < rhs.player + reinterpret_cast<uintptr_t>(&rhs.obj) + rhs.actname.m_Buf[0] + rhs.particleName.m_Buf[0];
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
};

struct Obj_RollbackData
{
	CXXBYTE<32> LinkParticleName;
	int LinkParticleObjType;
	bool LinkParticleUseArg;
	CCreateArg LinkParticleCreateArg;
	bool bLinkParticleSet;
	CXXBYTE<32> LinkParticleActName;
};

class UParticleSystemComponent : public RC::Unreal::USceneComponent 
{
public:
	FIELD(0x7B8, class OBJ_CBase*, LinkObjPtr);
};

struct Particle_RollbackData
{
	std::map<FPSCCacheKey, UParticleSystemComponent*> pscCache;
	std::map<FPSCCacheKey, UParticleSystemComponent*> unlinkedPscCache;
};

extern std::unordered_map<class OBJ_CBase*, Obj_RollbackData> objRollbackData;
extern Particle_RollbackData rollbackData;

FPSCCacheKey MakeUnlinkedPSCKey(class AREDGameState_Battle* GameState, class OBJ_CBase* obj, CXXBYTE<32>* name);
UParticleSystemComponent* GetCachedPSCForSet(class OBJ_CBase* obj);
bool Rollback_ProcessCachedPSC(class OBJ_CBase* obj, const CXXBYTE<32>* name, int objType, bool useArg);
void Rollback_OnLinkParticle(class OBJ_CBase* obj, const CXXBYTE<32>* name, int objType, bool useArg);
void AddLinkPSCToCache(class OBJ_CBase* obj);