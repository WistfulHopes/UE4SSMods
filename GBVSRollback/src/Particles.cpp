#include "Particles.hpp"
#include "BattleState.hpp"
#include "RollbackContext.hpp"
#include "Unreal.hpp"

#include <ranges>
#include <unordered_map>

RC::Unreal::UClass* UParticleSystemComponent::StaticClass = nullptr;

namespace
{
	int32_t ParticleGetPlayerIdx(const OBJ_CBase* Obj)
	{
		const auto* Parent = Obj->m_pParentPly;
		return Parent ? Parent->m_MemberID + 3 * Parent->m_SideID : 6;
	}
}

FPSCCacheKey MakeUnlinkedPSCKey(OBJ_CBase* obj, CXXBYTE<32>* name)
{
	auto& Ctx = GetRollbackContext();

	FPSCCacheKey key{};
	key.player = ParticleGetPlayerIdx(obj);
	key.obj = obj;
	key.GameFrame = Ctx.GameFrame;
	strcpy(key.particleName.m_Buf, name->m_Buf);
	strcpy(key.actname.m_Buf, obj->m_CurActionName.m_Buf);
	return key;
}

void AddUnlinkedPSCToCache(OBJ_CBase* obj, CXXBYTE<32>* name, UParticleSystemComponent* psc)
{
	auto& Ctx = GetRollbackContext();

	auto key = MakeUnlinkedPSCKey(obj, name);
	if (Ctx.Particles.unlinkedPscCache.contains(key)) return;
	Ctx.Particles.unlinkedPscCache.insert({key, psc});
	Ctx.Particles.unlinkedPscCacheUsed.insert({key, psc});
}

bool UseUnlinkedPSC(OBJ_CBase* obj, CXXBYTE<32>* name)
{
	auto& Ctx = GetRollbackContext();

	if (auto key = MakeUnlinkedPSCKey(obj, name); Ctx.Particles.unlinkedPscCache.contains(key))
	{
		Ctx.Particles.unlinkedPscCacheUsed.insert({key, Ctx.Particles.unlinkedPscCache[key]});
		return true;
	}

	return false;
}

FPSCCacheKey MakePSCKeyFromObj(OBJ_CBase* obj, CXXBYTE<32>* name)
{
	auto& Ctx = GetRollbackContext();
	
	FPSCCacheKey key{};
	key.player = ParticleGetPlayerIdx(obj);
	key.obj = obj;
	key.GameFrame = Ctx.GameFrame;
	strcpy(key.particleName.m_Buf, name->m_Buf);
	strcpy(key.actname.m_Buf, obj->m_CurActionName.m_Buf);
	
	return key;
}

FPSCCacheKey MakePSCKeyFromRBData(OBJ_CBase* obj)
{
	auto& Ctx = GetRollbackContext();
	const auto& Ext = Ctx.GetObjExt(obj);
	
	FPSCCacheKey key{};
	key.player = ParticleGetPlayerIdx(obj);
	key.obj = obj;
	key.GameFrame = Ctx.GameFrame;
	strcpy(key.actname.m_Buf, Ext.m_RollbackData.LinkParticleActName.m_Buf);
	strcpy(key.particleName.m_Buf, Ext.m_RollbackData.LinkParticleName.m_Buf);
	
	return key;
}


UParticleSystemComponent* GetCachedPSC(OBJ_CBase* obj, CXXBYTE<32>* name)
{
	auto& Ctx = GetRollbackContext();

	FPSCCacheKey key{MakePSCKeyFromObj(obj, name)};
	
	const auto iter = Ctx.Particles.pscCache.find(key);
	if (iter == Ctx.Particles.pscCache.end())
		return nullptr;

	auto result = iter->second;
	Ctx.Particles.pscCache.erase(iter);
	return result;
}

UParticleSystemComponent* GetCachedPSCForSet(OBJ_CBase* obj)
{
	auto& Ctx = GetRollbackContext();

	if (Ctx.GetObjSlot(obj) == InvalidSlot)
		return nullptr;

	FPSCCacheKey key{MakePSCKeyFromRBData(obj)};

	const auto iter = Ctx.Particles.pscCache.find(key);
	if (iter == Ctx.Particles.pscCache.end())
		return nullptr;

	auto result = iter->second;
	Ctx.Particles.pscCache.erase(iter);
	return result;
}

bool Rollback_ProcessCachedPSC(OBJ_CBase* obj, CXXBYTE<32>* name)
{
	auto& Ctx = GetRollbackContext();

	if (!Ctx.bIsFastForward && !Ctx.bIsSave) return false;

	UParticleSystemComponent* cachedPsc;

	if (Ctx.bIsSave)
	{
		cachedPsc = GetCachedPSCForSet(obj);
	}
	else
	{
		if (obj->m_pLinkPSC)
		{
			DeleteLinkPSC_Hook(obj);
		}

		if (name->m_Buf[0] == 0) return true;

		cachedPsc = GetCachedPSC(obj, name);
	}

	if (cachedPsc == nullptr)
	{
		obj->m_pLinkPSC = nullptr;
		return false;
	}

	cachedPsc->LinkObjPtr = obj;
	obj->m_pLinkPSC = cachedPsc;

	if (!Ctx.GetObjExt(obj).m_RollbackData.bLinkParticleSet)
	{
		Rollback_OnLinkParticle(obj, name);
	}
	return true;
}

bool Rollback_ProcessCachedUnlinkedPSC(OBJ_CBase* obj, CXXBYTE<32>* name)
{
	if (!GetRollbackContext().bIsFastForward) return false;

	return UseUnlinkedPSC(obj, name);
}

void Rollback_OnLinkParticle(OBJ_CBase* obj, CXXBYTE<32>* name)
{
	auto& Ctx = GetRollbackContext();

	auto& data = Ctx.GetObjExt(obj).m_RollbackData;
	strcpy(data.LinkParticleName.m_Buf, name->m_Buf);
	strcpy(data.LinkParticleActName.m_Buf, obj->m_CurActionName.m_Buf);
	data.LinkParticleStartFrame = Ctx.GameFrame;
}

bool AddLinkPSCToCache(OBJ_CBase* obj)
{
	auto& Ctx = GetRollbackContext();

	if (Ctx.GetObjSlot(obj) == InvalidSlot)
		return false;
	
	FPSCCacheKey key{MakePSCKeyFromRBData(obj)};

	if (Ctx.Particles.pscCache.contains(key))
		return false;

	Ctx.Particles.pscCache.insert({key, obj->m_pLinkPSC});
	return true;
}

void PSC_TickElapsedFrames()
{
	auto& Ctx = GetRollbackContext();
	if (!Ctx.GameState || !Ctx.GameState->PSCManager)
		return;

	std::unordered_map<UParticleSystemComponent*, int32_t> Particles;

	for (auto Components = Ctx.GameState->PSCManager->GetComponentsByClass(
		     UParticleSystemComponent::StaticClass); const auto Component : Components)
	{
		auto* psc = reinterpret_cast<UParticleSystemComponent*>(Component);
		if (!psc)
			continue;

		const auto iter = Ctx.PSCElapsedFrames.find(psc);
		const int32_t Previous = iter != Ctx.PSCElapsedFrames.end() ? iter->second : 0;
		Particles.insert({psc, Previous + 1});
	}

	Ctx.PSCElapsedFrames = std::move(Particles);
}

void PSC_RewindElapsedFrames(int32_t FramesRolledBack)
{
	auto& Ctx = GetRollbackContext();

	for (auto& Elapsed : Ctx.PSCElapsedFrames | std::views::values)
		Elapsed -= FramesRolledBack;
}

int32_t PSC_GetElapsedFrames(UParticleSystemComponent* psc)
{
	auto& Ctx = GetRollbackContext();

	const auto iter = Ctx.PSCElapsedFrames.find(psc);
	return iter != Ctx.PSCElapsedFrames.end() ? iter->second : 0;
}
