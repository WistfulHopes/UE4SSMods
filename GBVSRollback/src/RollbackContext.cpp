#include "RollbackContext.hpp"
#include "Unreal.hpp"

RollbackContext& GetRollbackContext()
{
	static RollbackContext Context;
	return Context;
}

int32_t RollbackContext::GetObjSlot(const OBJ_CBase* Obj) const
{
	if (!Obj || !GameState || !GameState->BattleObjectManager)
		return InvalidSlot;

	auto* Manager = GameState->BattleObjectManager;
	const auto Address = reinterpret_cast<uintptr_t>(Obj);

	const auto ObjBase = reinterpret_cast<uintptr_t>(&Manager->m_ObjVector[0]);
	if (Address >= ObjBase && Address < ObjBase + sizeof(OBJ_CBase) * ObjCount)
	{
		const auto Offset = Address - ObjBase;
		if (Offset % sizeof(OBJ_CBase) != 0)
			return InvalidSlot;
		return static_cast<int32_t>(Offset / sizeof(OBJ_CBase));
	}

	const auto CharaBase = reinterpret_cast<uintptr_t>(&Manager->m_CharVector[0]);
	if (Address >= CharaBase && Address < CharaBase + sizeof(OBJ_CCharBase) * CharCount)
	{
		const auto Offset = Address - CharaBase;
		if (Offset % sizeof(OBJ_CCharBase) != 0)
			return InvalidSlot;
		return ObjCount + static_cast<int32_t>(Offset / sizeof(OBJ_CCharBase));
	}

	const auto StageBase = reinterpret_cast<uintptr_t>(&Manager->m_StageVector[0]);
	if (Address >= StageBase && Address < StageBase + sizeof(OBJ_CStageBase) * StageCount)
	{
		const auto Offset = Address - StageBase;
		if (Offset % sizeof(OBJ_CStageBase) != 0)
			return InvalidSlot;
		return ObjCount + CharCount + static_cast<int32_t>(Offset / sizeof(OBJ_CStageBase));
	}

	return InvalidSlot;
}

OBJ_CBaseExt& RollbackContext::GetObjExt(const OBJ_CBase* Obj)
{
	const int32_t Slot = GetObjSlot(Obj);
	if (Slot == InvalidSlot)
	{
		DefaultObjExt = OBJ_CBaseExt();
		return DefaultObjExt;
	}
	return ObjExt[Slot];
}

void RollbackContext::ResetForNewMatch()
{
	GameFrame = 0;
	Random = nullptr;
	Inputs[0] = 0;
	Inputs[1] = 0;
	bIsSave = false;
	bIsFastForward = false;
	RollbackFramesRemaining = 0;

	for (auto& Ext : ObjExt)
		Ext = OBJ_CBaseExt();

	Particles = Particle_RollbackData();
	Pawns = Pawn_RollbackData();
	PSCElapsedFrames.clear();
}
