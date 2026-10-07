#include "Pawn.hpp"
#include "BattleState.hpp"
#include "RollbackContext.hpp"

RC::Unreal::UClass* AREDPawnEffect::StaticClass = nullptr;

namespace
{
	RC::Unreal::int32 GetPawnPlayerIdx(const OBJ_CBase* Obj)
	{
		return Obj->m_pParentPly ? Obj->m_SideID * 3 + Obj->m_MemberID : 6;
	}
}

bool Rollback_ProcessCachedPawn(OBJ_CBase* ctx, CXXBYTE<32>* name, bool isCommon, bool effect, bool self)
{
	auto& Ctx = GetRollbackContext();

	if (!Ctx.bIsFastForward && !Ctx.bIsSave) return false;

	AREDPawn* pawn = nullptr;
	if (Ctx.bIsFastForward)
	{
		if (ctx->m_pPawn)
		{
			if (Ctx.GetObjExt(ctx).m_RollbackData.LinkPawnName == *name)
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

	auto& Ext = Ctx.GetObjExt(ctx);

	pawn->ObjPtr = ctx;
	ctx->m_pPawn = pawn;
	ctx->m_PawnName = Ext.m_RollbackData.LinkPawnName;
	SetActorHiddenInGame(pawn, false);
	pawn->bIgnoreTick = false;
	SetActorTickEnabled(pawn, true);

	if (!Ext.m_RollbackData.LinkPawnSet)
	{
		Ext.m_RollbackData.LinkPawnName = *name;
		Ext.m_RollbackData.LinkCommonPawn = isCommon;
		Ext.m_RollbackData.LinkCommonPawnEffect = effect;
		Ext.m_RollbackData.LinkCommonPawnSelf = self;
		Ext.m_RollbackData.LinkPawnStartFrame = Ctx.GameFrame;
		Ext.m_RollbackData.LinkPawnActName = ctx->m_CurActionName;
	}

	return true;
}

FPawnCacheKey MakePawnKeyFromObj(OBJ_CBase* obj, CXXBYTE<32>* name, RC::Unreal::int32 Count)
{
	auto& Ctx = GetRollbackContext();

	return FPawnCacheKey{GetPawnPlayerIdx(obj), Ctx.GameFrame, obj->m_CurActionName, *name, Count};
}

FPawnCacheKey MakePawnKeyFromRBData(OBJ_CBase* obj, RC::Unreal::int32 Count)
{
	auto& Ctx = GetRollbackContext();

	if (Ctx.GetObjSlot(obj) == InvalidSlot)
		return FPawnCacheKey{GetPawnPlayerIdx(obj), 0, "", "", Count};

	const auto& Data = Ctx.GetObjExt(obj).m_RollbackData;
	return FPawnCacheKey{
		GetPawnPlayerIdx(obj), Data.LinkPawnStartFrame, Data.LinkPawnActName, Data.LinkPawnName, Count
	};
}

AREDPawn* GetCachedPawn(OBJ_CBase* ctx, CXXBYTE<32>* name)
{
	auto& Ctx = GetRollbackContext();

	for (int i = 0; i < 10; i++)
	{
		auto key = MakePawnKeyFromObj(ctx, name, i);
		const auto iter = Ctx.Pawns.pawnCache.find(key);
		if (iter != Ctx.Pawns.pawnCache.end())
		{
			auto result = iter->second;
			Ctx.Pawns.pawnCache.erase(iter);
			return result;
		}
	}
	return nullptr;
}

AREDPawn* GetCachedPawnForSet(OBJ_CBase* ctx)
{
	auto& Ctx = GetRollbackContext();

	for (int i = 0; i < 10; i++)
	{
		auto key = MakePawnKeyFromRBData(ctx, i);
		const auto iter = Ctx.Pawns.pawnCache.find(key);
		if (iter != Ctx.Pawns.pawnCache.end())
		{
			auto result = iter->second;
			Ctx.Pawns.pawnCache.erase(iter);
			return result;
		}
	}
	return nullptr;
}

bool AddLinkPawnToCache(OBJ_CBase* ctx)
{
	auto& Ctx = GetRollbackContext();

	for (int i = 0; i < 10; i++)
	{
		auto key = MakePawnKeyFromRBData(ctx, i);
		if (!Ctx.Pawns.pawnCache.contains(key))
		{
			Ctx.Pawns.pawnCache.insert(std::pair(key, ctx->m_pPawn));
			return true;
		}
	}
	return false;
}

bool AddLinkPawnForQuickRestore(OBJ_CBase* ctx)
{
	auto& Ctx = GetRollbackContext();

	for (auto& pawn : Ctx.Pawns.PawnQuickRestoreCache)
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
	auto& Ctx = GetRollbackContext();

	for (auto pawn : Ctx.Pawns.PawnQuickRestoreCache)
	{
		if (!pawn) break;

		auto obj = pawn->ObjPtr;
		if (!obj) continue;

		obj->m_pPawn = pawn;
		obj->m_PawnName = Ctx.GetObjExt(obj).m_RollbackData.LinkPawnName;
	}

	RC::Unreal::FMemory::Memzero(Ctx.Pawns.PawnQuickRestoreCache);
}
