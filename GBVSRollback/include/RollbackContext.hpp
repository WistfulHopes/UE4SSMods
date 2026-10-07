#pragma once

#include "BattleState.hpp"
#include "Particles.hpp"
#include "Pawn.hpp"

#include <unordered_map>

struct RollbackContext
{
	class AREDGameState_Battle* GameState = nullptr;
	AA_CRandMT* Random = nullptr;
	void* SystemManager = nullptr;

	uint32_t GameFrame = 0;
	int32_t Inputs[2]{};

	bool bCanRollback = false;
	bool bRollbackTest = false;
	bool bIsSave = false;
	bool bIsFastForward = false;

	int32_t RollbackFramesRemaining = 0;

	OBJ_CBaseExt ObjExt[TotalObjCount]{};

	Particle_RollbackData Particles{};
	Pawn_RollbackData Pawns{};

	std::unordered_map<UParticleSystemComponent*, int32_t> PSCElapsedFrames{};

	int32_t GetObjSlot(const OBJ_CBase* Obj) const;

	OBJ_CBaseExt& GetObjExt(const OBJ_CBase* Obj);

	void ResetForNewMatch();

private:
	OBJ_CBaseExt DefaultObjExt{};
};

RollbackContext& GetRollbackContext();
