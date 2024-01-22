#include "Particles.h"
#include "BattleState.h"
#include "Unreal.h"

FPSCCacheKey MakeUnlinkedPSCKey(AREDGameState_Battle* GameState, OBJ_CBase* obj, CXXBYTE<32>* name)
{
	auto* parentPlayer = reinterpret_cast<OBJ_CCharBase*>(reinterpret_cast<uintptr_t>(obj) + 0x260);
	FPSCCacheKey key{};
	int playerId = 6;
	if (parentPlayer != nullptr)
	{
		playerId = parentPlayer->m_MemberID + 3 * parentPlayer->m_SideID;
	}
	key.player = playerId;
	key.obj = obj;
	strcpy(key.particleName.m_Buf, name->m_Buf);
	strcpy(key.actname.m_Buf, obj->m_CurActionName.m_Buf);
	return key;
}

UParticleSystemComponent* GetCachedPSCForSet(OBJ_CBase* obj)
{
	auto* parentPlayer = reinterpret_cast<OBJ_CCharBase*>(reinterpret_cast<uintptr_t>(obj) + 0x260);
	FPSCCacheKey key{};
	int playerId = 6;
	if (parentPlayer != nullptr)
	{
		playerId = parentPlayer->m_MemberID + 3 * parentPlayer->m_SideID;
	}
	const auto iter = objRollbackData.find(obj);
	if (iter != objRollbackData.end())
	{
		key.player = playerId;
		key.obj = obj;
		strcpy(key.actname.m_Buf, iter->second.LinkParticleActName.m_Buf);
		strcpy(key.particleName.m_Buf, iter->second.LinkParticleName.m_Buf);
		const auto iter2 = rollbackData.pscCache.find(key);
		if (iter2 == rollbackData.pscCache.end())
			return nullptr;
		rollbackData.pscCache.erase(iter2);
		return obj->m_pLinkPSC;
	}
	return nullptr;
}

bool Rollback_ProcessCachedPSC(OBJ_CBase* obj, const CXXBYTE<32>* name, int objType, bool useArg)
{
	UParticleSystemComponent* cachedPsc = GetCachedPSCForSet(obj);
	if (cachedPsc == nullptr)
	{
		((int*)obj)[0xA7A8] = 0;
		((int*)obj)[0xA7AC] = 0;
		return false;
	}
	reinterpret_cast<int**>(cachedPsc)[0x7B8] = reinterpret_cast<int*>(obj);
	reinterpret_cast<int**>(obj)[0xA7A8] = reinterpret_cast<int*>(cachedPsc);
	const auto iter = objRollbackData.find(obj);
	if (!iter->second.bLinkParticleSet)
	{
		const CCreateArg arg = *reinterpret_cast<CCreateArg*>(&reinterpret_cast<char*>(obj)[0xA3FC]);
		strcpy(iter->second.LinkParticleName.m_Buf, name->m_Buf);
		iter->second.LinkParticleObjType = objType;
		strcpy(iter->second.LinkParticleCreateArg.m_CreateArg_SocketName.m_Buf, arg.m_CreateArg_SocketName.m_Buf);
		iter->second.LinkParticleUseArg = useArg;
		iter->second.LinkParticleCreateArg.m_CreateArg_Angle = arg.m_CreateArg_Angle;
		iter->second.LinkParticleCreateArg.m_CreateArg_OffsetPosZ = arg.m_CreateArg_OffsetPosZ;
		iter->second.LinkParticleCreateArg.m_CreateArg_Hikitsugi0 = arg.m_CreateArg_Hikitsugi0;
		iter->second.LinkParticleCreateArg.m_CreateArg_TransPriority = arg.m_CreateArg_TransPriority;
		strcpy(iter->second.LinkParticleActName.m_Buf, obj->m_CurActionName.m_Buf);
	}
	return true;
}

void Rollback_OnLinkParticle(OBJ_CBase* obj, const CXXBYTE<32>* name, int objType, bool useArg)
{
	const auto* arg = reinterpret_cast<CCreateArg*>(&((char*)obj)[0xA3FC]);
	Obj_RollbackData data{};
	strcpy(data.LinkParticleName.m_Buf, name->m_Buf);
	data.LinkParticleObjType = objType;
	strcpy(data.LinkParticleCreateArg.m_CreateArg_SocketName.m_Buf, arg->m_CreateArg_SocketName.m_Buf);
	data.LinkParticleUseArg = useArg;
	data.LinkParticleCreateArg.m_CreateArg_Angle = arg->m_CreateArg_Angle;
	data.LinkParticleCreateArg.m_CreateArg_OffsetPosZ = arg->m_CreateArg_OffsetPosZ;
	data.LinkParticleCreateArg.m_CreateArg_Hikitsugi0 = arg->m_CreateArg_Hikitsugi0;
	data.LinkParticleCreateArg.m_CreateArg_TransPriority = arg->m_CreateArg_TransPriority;
	strcpy(data.LinkParticleActName.m_Buf, obj->m_CurActionName.m_Buf);
	objRollbackData.insert({obj, data });
}

void AddLinkPSCToCache(OBJ_CBase* obj)
{
	auto* parentPlayer = reinterpret_cast<OBJ_CCharBase*>(reinterpret_cast<uintptr_t>(obj) + 0x260);
	FPSCCacheKey key{};
	int playerId = 6;
	if (parentPlayer != nullptr)
	{
		playerId = parentPlayer->m_MemberID + 3 * parentPlayer->m_SideID;
	}
	auto iter = objRollbackData.find(obj);
	if (iter != objRollbackData.end())
	{
		key.player = playerId;
		key.obj = obj;
		strcpy(key.particleName.m_Buf, iter->second.LinkParticleName.m_Buf);
		strcpy(key.actname.m_Buf, iter->second.LinkParticleActName.m_Buf);
		rollbackData.pscCache.insert({ key, obj->m_pLinkPSC });
	}
}
