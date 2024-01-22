#include "BattleState.h"
#include "Unreal.h"
#include <cstring>

void RollbackData::SaveObj(BATTLE_CObjectManager* InObjManager)
{
	for (int i = 0; i < 400; i++)
	{
        if (InObjManager->m_ObjVector[i].m_ActiveState > 0)
        {
            std::memcpy(&ObjManager->m_ObjVector[i], &InObjManager->m_ObjVector[i], sizeof(OBJ_CBase));
        }
	}
}

void RollbackData::SaveChara(BATTLE_CObjectManager* InObjManager)
{
	for (int i = 0; i < 6; i++)
	{
        std::memcpy(&ObjManager->m_CharVector[i], &InObjManager->m_CharVector[i], sizeof(OBJ_CCharBase));
	}
}

void RollbackData::SaveRPG(BATTLE_CObjectManager* InObjManager)
{
	for (int i = 0; i < 10; i++)
	{
        if (InObjManager->m_ObjVector[i].m_ActiveState > 0)
        {
            std::memcpy(&ObjManager->m_RpgVector[i], &InObjManager->m_RpgVector[i], sizeof(OBJ_CRPGBase));
        }
	}
}

void RollbackData::SaveState(AREDGameState_Battle* GameState) {
	if (ObjManager == nullptr)
		ObjManager = static_cast<BATTLE_CObjectManager*>(std::malloc(sizeof(BATTLE_CObjectManager)));
	std::memcpy(ObjManager, GameState->BattleObjectManager, 0x83C0);
    std::memcpy(reinterpret_cast<char*>(ObjManager) + 0x12637E0, reinterpret_cast<char*>(GameState->BattleObjectManager) + 0x12637E0, 0x23BF0);
    SaveObj(GameState->BattleObjectManager);
    SaveChara(GameState->BattleObjectManager);
    SaveRPG(GameState->BattleObjectManager);
	if (ScrManager == nullptr)
		ScrManager = static_cast<BATTLE_CScreenManager*>(std::malloc(sizeof(BATTLE_CScreenManager)));
	std::memcpy(ScrManager, GameState->BattleScreenManager, sizeof(BATTLE_CScreenManager));
	if (State == nullptr)
		State = static_cast<BattleState*>(std::malloc(sizeof(BattleState)));
	std::memcpy(State, GameState->State, sizeof(BattleState));
	if (EvtManager == nullptr)
		EvtManager = static_cast<BattleEventManager*>(std::malloc(sizeof(BattleEventManager)));
	std::memcpy(EvtManager, GameState->EventManager, sizeof(BattleEventManager));
	CamRollbackData.m_Pos = GameState->BattleScreenManager->m_pCamera->m_Pos;
	CamRollbackData.m_Up = GameState->BattleScreenManager->m_pCamera->m_Up;
	CamRollbackData.m_At = GameState->BattleScreenManager->m_pCamera->m_At;
	BGRollbackData.m_OffsetMatrix = GameState->BGLocation->m_OffsetMatrix;
	BGRollbackData.m_OffsetMove_Location = GameState->BGLocation->m_OffsetMove_Location;
	BGRollbackData.m_OffsetMove_Rotation = GameState->BGLocation->m_OffsetMove_Rotation;
}

void RollbackData::LoadObj(BATTLE_CObjectManager* InObjManager)
{
	for (int i = 0; i < 400; i++)
	{
		if (ObjManager->m_ObjVector[i].m_ActiveState > 0)
		{
			std::memcpy(&InObjManager->m_ObjVector[i], &ObjManager->m_ObjVector[i], sizeof(OBJ_CBase));
		}
		else
		{
			InObjManager->m_ObjVector[i].m_ActiveState = ACTV_NOT_ACTIVE;
		}
	}
}

void RollbackData::LoadChara(BATTLE_CObjectManager* InObjManager)
{
	for (int i = 0; i < 6; i++)
	{
        std::memcpy(&InObjManager->m_CharVector[i], &ObjManager->m_CharVector[i], sizeof(OBJ_CCharBase));
	}
}

void RollbackData::LoadRPG(BATTLE_CObjectManager* InObjManager)
{
	for (int i = 0; i < 10; i++)
	{
		if (ObjManager->m_ObjVector[i].m_ActiveState > 0)
		{
            std::memcpy(&InObjManager->m_RpgVector[i], &ObjManager->m_RpgVector[i], sizeof(OBJ_CRPGBase));
		}
		else
		{
			InObjManager->m_RpgVector[i].m_ActiveState = ACTV_NOT_ACTIVE;
		}
	}
}

void RollbackData::LoadState(AREDGameState_Battle* GameState)
{
	if (ObjManager == nullptr)
		return;
	std::memcpy(GameState->BattleObjectManager, ObjManager, 0x83C0);
	std::memcpy(reinterpret_cast<char*>(GameState->BattleObjectManager) + 0x12637E0, reinterpret_cast<char*>(ObjManager) + 0x12637E0, 0x23C10);
	LoadObj(GameState->BattleObjectManager);
	LoadChara(GameState->BattleObjectManager);
	LoadRPG(GameState->BattleObjectManager);
	if (ScrManager == nullptr)
		return;
	std::memcpy(GameState->BattleScreenManager, ScrManager, sizeof(BATTLE_CScreenManager));
	if (State == nullptr)
		return;
	std::memcpy(GameState->State, State, sizeof(BattleState));
	if (EvtManager == nullptr)
		return;
	std::memcpy(GameState->EventManager, EvtManager, sizeof(BattleEventManager));
	GameState->BattleScreenManager->m_pCamera->m_Pos = CamRollbackData.m_Pos;
	GameState->BattleScreenManager->m_pCamera->m_Up = CamRollbackData.m_Up;
	GameState->BattleScreenManager->m_pCamera->m_At = CamRollbackData.m_At;
	GameState->BGLocation->m_OffsetMatrix = BGRollbackData.m_OffsetMatrix;
	GameState->BGLocation->m_OffsetMove_Location = BGRollbackData.m_OffsetMove_Location;
	GameState->BGLocation->m_OffsetMove_Rotation = BGRollbackData.m_OffsetMove_Rotation;
}