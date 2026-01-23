#include "Particles.hpp"
#include "BattleState.hpp"
#include "Unreal.hpp"

RC::Unreal::UClass* UParticleSystemComponent::StaticClass = nullptr;

FPSCCacheKey MakeUnlinkedPSCKey(OBJ_CBase* obj, CXXBYTE<32>* name)
{
	auto* parentPlayer = obj->m_pParentPly;
	FPSCCacheKey key{};
	int playerId = 6;
	if (parentPlayer != nullptr)
	{
		playerId = parentPlayer->m_MemberID + 3 * parentPlayer->m_SideID;
	}
	key.player = playerId;
	key.obj = obj;
	key.GameFrame = GameFrame;
	strcpy(key.particleName.m_Buf, name->m_Buf);
	strcpy(key.actname.m_Buf, obj->m_CurActionName.m_Buf);
	return key;
}

void AddUnlinkedPSCToCache(OBJ_CBase* obj, CXXBYTE<32>* name, UParticleSystemComponent* psc)
{
	auto key = MakeUnlinkedPSCKey(obj, name);
	if (rollbackData.unlinkedPscCache.contains(key)) return;
    rollbackData.unlinkedPscCache.insert({key, psc});
    rollbackData.unlinkedPscCacheUsed.insert({key, psc});
}

bool UseUnlinkedPSC(OBJ_CBase* obj, CXXBYTE<32>* name)
{
	if (auto key = MakeUnlinkedPSCKey(obj, name); rollbackData.unlinkedPscCache.contains(key))
	{
	    rollbackData.unlinkedPscCacheUsed.insert({key, rollbackData.unlinkedPscCache[key]});
		return true;
	}

	return false;
}

UParticleSystemComponent* GetCachedPSC(OBJ_CBase* obj, CXXBYTE<32>* name)
{
	auto* parentPlayer = obj->m_pParentPly;
	FPSCCacheKey key{};
	int playerId = 6;
	if (parentPlayer != nullptr)
	{
		playerId = parentPlayer->m_MemberID + 3 * parentPlayer->m_SideID;
	}
	key.player = playerId;
	key.obj = obj;
	key.GameFrame = GameFrame;
	strcpy(key.particleName.m_Buf, name->m_Buf);
	strcpy(key.actname.m_Buf, obj->m_CurActionName.m_Buf);
	const auto iter2 = rollbackData.pscCache.find(key);
	if (iter2 == rollbackData.pscCache.end())
		return nullptr;
	auto result = iter2->second;
	rollbackData.pscCache.erase(iter2);
	return result;
}

UParticleSystemComponent* GetCachedPSCForSet(OBJ_CBase* obj)
{
	auto* parentPlayer = obj->m_pParentPly;
	FPSCCacheKey key{};
	int playerId = 6;
	if (parentPlayer != nullptr)
	{
		playerId = parentPlayer->m_MemberID + 3 * parentPlayer->m_SideID;
	}
	const auto iter = objData.find(obj);
	if (iter != objData.end())
	{
		key.player = playerId;
		key.obj = obj;
		key.GameFrame = GameFrame;
		strcpy(key.actname.m_Buf, iter->second.m_RollbackData.LinkParticleActName.m_Buf);
		strcpy(key.particleName.m_Buf, iter->second.m_RollbackData.LinkParticleName.m_Buf);
		const auto iter2 = rollbackData.pscCache.find(key);
		if (iter2 == rollbackData.pscCache.end())
			return nullptr;
		auto result = iter2->second;
		rollbackData.pscCache.erase(iter2);
		return result;
	}
	return nullptr;
}

bool Rollback_ProcessCachedPSC(OBJ_CBase* obj, CXXBYTE<32>* name)
{
	if (!bIsRollback && !bIsSave) return false;
	
	UParticleSystemComponent* cachedPsc;

	if (bIsSave)
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
	const auto iter = objData.find(obj);
	if (!iter->second.m_RollbackData.bLinkParticleSet)
	{
		Rollback_OnLinkParticle(obj, name);
	}
	return true;
}

bool Rollback_ProcessCachedUnlinkedPSC(OBJ_CBase* obj, CXXBYTE<32>* name)
{
	if (!bIsRollback) return false;

	return UseUnlinkedPSC(obj, name);
}

void Rollback_OnLinkParticle(OBJ_CBase* obj, CXXBYTE<32>* name)
{
	auto data = &objData[obj].m_RollbackData;
	strcpy(data->LinkParticleName.m_Buf, name->m_Buf);
	strcpy(data->LinkParticleActName.m_Buf, obj->m_CurActionName.m_Buf);
	data->LinkParticleStartFrame = GameFrame;
}

bool AddLinkPSCToCache(OBJ_CBase* obj)
{
	auto* parentPlayer = obj->m_pParentPly;
	FPSCCacheKey key{};
	int playerId = 6;
	if (parentPlayer != nullptr)
	{
		playerId = parentPlayer->m_MemberID + 3 * parentPlayer->m_SideID;
	}
	auto iter = objData.find(obj);
	if (iter != objData.end())
	{
		key.player = playerId;
		key.obj = obj;
		key.GameFrame = iter->second.m_RollbackData.LinkParticleStartFrame;
		strcpy(key.particleName.m_Buf, iter->second.m_RollbackData.LinkParticleName.m_Buf);
		strcpy(key.actname.m_Buf, iter->second.m_RollbackData.LinkParticleActName.m_Buf);
		if (rollbackData.pscCache.contains(key)) 
			return false;
		rollbackData.pscCache.insert({ key, obj->m_pLinkPSC });
		return true;
	}
	return false;
}