#include "Pawn.hpp"
#include "BattleState.hpp"

RC::Unreal::UClass* AREDPawnEffect::StaticClass = nullptr;

bool Rollback_ProcessCachedPawn(OBJ_CBase* ctx, CXXBYTE<32>* name, bool isCommon, bool effect, bool self)
{
    if (!bIsRollback && !bIsSave) return false;
	
	AREDPawn* pawn = nullptr;
	if (bIsRollback)
	{
		if (ctx->m_pPawn)
		{
			const auto iter = objData.find(ctx);
			if (iter->second.m_RollbackData.LinkPawnName == *name)
			{
				return true;
			}
			
			ClearLinkModel(ctx);			
		}
		
		if (*name == "")
			return true;
		
		pawn = GetCachedPawn(ctx, name);
	}
	else
	{
		pawn = GetCachedPawnForSet(ctx);
	}
	
	if (!pawn)
	{
		ClearLinkModel(ctx);
		return false;
	}
	else
	{
		pawn->ObjPtr = ctx;
		ctx->m_pPawn = pawn;
		const auto iter = objData.find(ctx);
		ctx->m_PawnName = iter->second.m_RollbackData.LinkPawnName;
		SetActorHiddenInGame(pawn, false);
		pawn->bIgnoreTick = false;
		SetActorTickEnabled(pawn, true);
		
		if (!iter->second.m_RollbackData.LinkPawnSet)
		{
			iter->second.m_RollbackData.LinkPawnName = *name;
			iter->second.m_RollbackData.LinkCommonPawn = isCommon;
			iter->second.m_RollbackData.LinkCommonPawnEffect = effect;
			iter->second.m_RollbackData.LinkCommonPawnSelf = self;
			iter->second.m_RollbackData.LinkPawnStartFrame = GameFrame;
			iter->second.m_RollbackData.LinkPawnActName = ctx->m_CurActionName;			
		}
	}
	
	return true;
}

FPawnCacheKey MakePawnKeyFromObj(OBJ_CBase* obj, CXXBYTE<32>* name, RC::Unreal::int32 Count)
{
	RC::Unreal::int32 player = 6;
	
	if (obj->m_pParentPly)
	{
		player = obj->m_SideID * 3 + obj->m_MemberID;
	}
	
	return FPawnCacheKey{player, GameFrame, obj->m_CurActionName, *name, Count};
}

FPawnCacheKey MakePawnKeyFromRBData(OBJ_CBase* obj, RC::Unreal::int32 Count)
{
	RC::Unreal::int32 player = 6;

	if (obj->m_pParentPly)
	{
		player = obj->m_SideID * 3 + obj->m_MemberID;
	}
	const auto iter = objData.find(obj);
	if (iter != objData.end())
	{
		return FPawnCacheKey{ player, iter->second.m_RollbackData.LinkPawnStartFrame, iter->second.m_RollbackData.LinkPawnActName, iter->second.m_RollbackData.LinkPawnName, Count };
	}
	
	return FPawnCacheKey{ player, 0, "", "", Count };
}

AREDPawn* GetCachedPawn(OBJ_CBase* ctx, CXXBYTE<32>* name)
{
	int i = 0;
	while (i < 10)
	{
		auto key = MakePawnKeyFromObj(ctx, name, i);
		const auto iter = pawnRollbackData.pawnCache.find(key);
		if (iter != pawnRollbackData.pawnCache.end())
		{
			auto result = iter->second;
			pawnRollbackData.pawnCache.erase(iter);
			return result;
		}
		i++;
	}
	return nullptr;
}

AREDPawn* GetCachedPawnForSet(OBJ_CBase* ctx)
{
	int i = 0;
	while (i < 10)
	{
		auto key = MakePawnKeyFromRBData(ctx, i);
		const auto iter = pawnRollbackData.pawnCache.find(key);
		if (iter != pawnRollbackData.pawnCache.end())
		{
			auto result = iter->second;
			pawnRollbackData.pawnCache.erase(iter);
			return result;
		}
		i++;
	}
	return nullptr;
}

bool AddLinkPawnToCache(OBJ_CBase* ctx)
{
	int i = 0;
	while (i < 10)
	{
		auto key = MakePawnKeyFromRBData(ctx, i);
		const auto iter = pawnRollbackData.pawnCache.find(key);
		if (iter == pawnRollbackData.pawnCache.end())
		{
			pawnRollbackData.pawnCache.insert(std::pair(key, ctx->m_pPawn));
			return true;
		}
		i++;
	}
	return false;
}

bool AddLinkPawnForQuickRestore(OBJ_CBase* ctx)
{
	for (auto& pawn : pawnRollbackData.PawnQuickRestoreCache)
	{
		if (!pawn)
		{
			pawn = ctx->m_pPawn;
			return true;
		}
	}
	return false;
}

void QuickRestoreLinkPawns()
{
	for (auto pawn : pawnRollbackData.PawnQuickRestoreCache)
	{
		if (!pawn) break;
		
		auto obj = pawn->ObjPtr;
		pawn->ObjPtr = obj;
		obj->m_pPawn = pawn;
		obj->m_PawnName = objData[obj].m_RollbackData.LinkPawnName;
	}
	
	RC::Unreal::FMemory::Memzero(pawnRollbackData.PawnQuickRestoreCache);
}
