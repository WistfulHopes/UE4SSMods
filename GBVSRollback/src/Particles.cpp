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
	const auto iter = objData.find(obj);
	if (iter != objData.end())
	{
		key.player = playerId;
		key.obj = obj;
		strcpy(key.actname.m_Buf, iter->second.m_RollbackData.LinkParticleActName.m_Buf);
		strcpy(key.particleName.m_Buf, iter->second.m_RollbackData.LinkParticleName.m_Buf);
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
	if (!bIsRollback) return false;
	UParticleSystemComponent* cachedPsc = GetCachedPSCForSet(obj);
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
		const CCreateArg arg = *reinterpret_cast<CCreateArg*>(&reinterpret_cast<char*>(obj)[0xA3FC]);
		strcpy(iter->second.m_RollbackData.LinkParticleName.m_Buf, name->m_Buf);
		iter->second.m_RollbackData.LinkParticleObjType = objType;
		strcpy(iter->second.m_RollbackData.LinkParticleCreateArg.m_CreateArg_SocketName.m_Buf, arg.m_CreateArg_SocketName.m_Buf);
		iter->second.m_RollbackData.LinkParticleUseArg = useArg;
		iter->second.m_RollbackData.LinkParticleCreateArg.m_CreateArg_Angle = arg.m_CreateArg_Angle;
		iter->second.m_RollbackData.LinkParticleCreateArg.m_CreateArg_OffsetPosZ = arg.m_CreateArg_OffsetPosZ;
		iter->second.m_RollbackData.LinkParticleCreateArg.m_CreateArg_Hikitsugi0 = arg.m_CreateArg_Hikitsugi0;
		iter->second.m_RollbackData.LinkParticleCreateArg.m_CreateArg_TransPriority = arg.m_CreateArg_TransPriority;
		strcpy(iter->second.m_RollbackData.LinkParticleActName.m_Buf, obj->m_CurActionName.m_Buf);
	}
	return true;
}

void Rollback_OnLinkParticle(OBJ_CBase* obj, const CXXBYTE<32>* name, int objType, bool useArg)
{
	auto data = &objData[obj].m_RollbackData;
	strcpy(data->LinkParticleName.m_Buf, name->m_Buf);
	data->LinkParticleObjType = objType;
	strcpy(data->LinkParticleCreateArg.m_CreateArg_SocketName.m_Buf, obj->m_CreateArg.m_CreateArg_SocketName.m_Buf);
	data->LinkParticleUseArg = useArg;
	data->LinkParticleCreateArg.m_CreateArg_Angle = obj->m_CreateArg.m_CreateArg_Angle;
	data->LinkParticleCreateArg.m_CreateArg_AngleY = obj->m_CreateArg.m_CreateArg_AngleY;
	data->LinkParticleCreateArg.m_CreateArg_OffsetPosX = obj->m_CreateArg.m_CreateArg_OffsetPosX;
	data->LinkParticleCreateArg.m_CreateArg_OffsetPosY = obj->m_CreateArg.m_CreateArg_OffsetPosY;
	data->LinkParticleCreateArg.m_CreateArg_OffsetPosZ = obj->m_CreateArg.m_CreateArg_OffsetPosZ;
	data->LinkParticleCreateArg.m_CreateArg_ScaleX = obj->m_CreateArg.m_CreateArg_ScaleX;
	data->LinkParticleCreateArg.m_CreateArg_ScaleY = obj->m_CreateArg.m_CreateArg_ScaleY;
	data->LinkParticleCreateArg.m_CreateArg_ScaleZ = obj->m_CreateArg.m_CreateArg_ScaleZ;
	data->LinkParticleCreateArg.m_CreateArg_Hikitsugi0 = obj->m_CreateArg.m_CreateArg_Hikitsugi0;
	data->LinkParticleCreateArg.m_CreateArg_HkrColor = obj->m_CreateArg.m_CreateArg_HkrColor;
	data->LinkParticleCreateArg.m_CreateArg_MltColor = obj->m_CreateArg.m_CreateArg_MltColor;
	data->LinkParticleCreateArg.m_CreateArg_TransPriority = obj->m_CreateArg.m_CreateArg_TransPriority;
	data->LinkParticleCreateArg.m_CreateArg_Direction = obj->m_CreateArg.m_CreateArg_Direction;
	data->LinkParticleCreateArg.m_CreateArg_SocketUse = obj->m_CreateArg.m_CreateArg_SocketUse;
	data->LinkParticleCreateArg.m_CreateArg_SocketWithRot = obj->m_CreateArg.m_CreateArg_SocketWithRot;
	data->LinkParticleCreateArg.m_CreateArg_NoAssert = obj->m_CreateArg.m_CreateArg_NoAssert;
	data->LinkParticleCreateArg.m_CreateArg_PointLightSide = obj->m_CreateArg.m_CreateArg_PointLightSide;
	data->LinkParticleCreateArg.m_CreateArg_PointLightMember = obj->m_CreateArg.m_CreateArg_PointLightMember;
	strcpy(data->LinkParticleActName.m_Buf, obj->m_CurActionName.m_Buf);
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
	auto iter = objData.find(obj);
	if (iter != objData.end())
	{
		key.player = playerId;
		key.obj = obj;
		strcpy(key.particleName.m_Buf, iter->second.m_RollbackData.LinkParticleName.m_Buf);
		strcpy(key.actname.m_Buf, iter->second.m_RollbackData.LinkParticleActName.m_Buf);
		rollbackData.pscCache.insert({ key, obj->m_pLinkPSC });
	}
}
