#pragma once

#include <Unreal/AActor.hpp>
#include <Unreal/Core/Containers/Array.hpp>
#include "struct_util.h"
#include "BattleState.h"
#include "Particles.h"

class AGameState : public RC::Unreal::AActor {};

class AREDPSCManager : public RC::Unreal::AActor {};

class AREDGameState_Battle : public AGameState
{
public:
	static inline AREDGameState_Battle* instance;

	static AREDGameState_Battle* GetGameState()
	{
		return instance;
	}

	FIELD(0x638, AREDPSCManager*, PSCManager);
	FIELD(0xB38, BATTLE_CObjectManager*, BattleObjectManager);
	FIELD(0xB40, BATTLE_CScreenManager*, BattleScreenManager);
	FIELD(0xB50, BattleState*, State);
	FIELD(0xB58, BattleEventManager*, EventManager);
	FIELD(0xB68, BattleBGLocation*, BGLocation);
};
