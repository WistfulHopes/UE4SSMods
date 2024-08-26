#include "BattleState.h"
#include "Unreal.h"
#include <cstring>

void RollbackData::SaveObj(BATTLE_CObjectManager* InObjManager)
{
	for (int i = 0; i < 400; i++)
	{
        if (InObjManager->m_ObjVector[i].m_ActiveState > 0)
        {
            std::memcpy(&ObjManager->m_ObjVector[i].ObjBaseSyncBegin, &InObjManager->m_ObjVector[i].ObjBaseSyncBegin, sizeof(OBJ_CBase) - 8);
        	auto from = objData[&ObjManager->m_ObjVector[i]];
        	auto to = StoredObjData[&ObjManager->m_ObjVector[i]];
        	std::memcpy(&to, &from, sizeof(OBJ_CBaseExt));
        }
	}
}

void RollbackData::SaveChara(BATTLE_CObjectManager* InObjManager)
{
	for (int i = 0; i < 6; i++)
	{
        std::memcpy(&ObjManager->m_CharVector[i].ObjBaseSyncBegin, &InObjManager->m_CharVector[i].ObjBaseSyncBegin, sizeof(OBJ_CCharBase) - 8);
		auto from = objData[&ObjManager->m_CharVector[i]];
		auto to = StoredObjData[&ObjManager->m_CharVector[i]];
		std::memcpy(&to, &from, sizeof(OBJ_CBaseExt));
	}
}

void RollbackData::SaveRPG(BATTLE_CObjectManager* InObjManager)
{
	for (int i = 0; i < 10; i++)
	{
        if (InObjManager->m_ObjVector[i].m_ActiveState > 0)
        {
            std::memcpy(&ObjManager->m_RpgVector[i].ObjBaseSyncBegin, &InObjManager->m_RpgVector[i].ObjBaseSyncBegin, sizeof(OBJ_CRPGBase) - 8);
        	auto from = objData[&ObjManager->m_RpgVector[i]];
        	auto to = StoredObjData[&ObjManager->m_RpgVector[i]];
        	std::memcpy(&to, &from, sizeof(OBJ_CBaseExt));
        }
	}
}

void RollbackData::SaveState(AREDGameState_Battle* GameState) {
	if (ObjManager == nullptr)
		ObjManager = static_cast<BATTLE_CObjectManager*>(RC::Unreal::FMemory::Malloc(sizeof(BATTLE_CObjectManager)));
	std::memcpy(ObjManager, GameState->BattleObjectManager, 0x83C0);
    std::memcpy(reinterpret_cast<char*>(ObjManager) + 0x12637E0, reinterpret_cast<char*>(GameState->BattleObjectManager) + 0x12637E0, 0x23BF0);
    SaveObj(GameState->BattleObjectManager);
    SaveChara(GameState->BattleObjectManager);
    SaveRPG(GameState->BattleObjectManager);
	if (ScrManager == nullptr)
		ScrManager = static_cast<BATTLE_CScreenManager*>(RC::Unreal::FMemory::Malloc(sizeof(BATTLE_CScreenManager)));
	std::memcpy(ScrManager, GameState->BattleScreenManager, sizeof(BATTLE_CScreenManager));
	if (State == nullptr)
		State = static_cast<BattleState*>(RC::Unreal::FMemory::Malloc(sizeof(BattleState)));
	std::memcpy(State, GameState->State, sizeof(BattleState));
	if (EvtManager == nullptr)
		EvtManager = static_cast<BattleEventManager*>(RC::Unreal::FMemory::Malloc(sizeof(BattleEventManager)));
	std::memcpy(EvtManager, GameState->EventManager, sizeof(BattleEventManager));

	if (BtlEvent)
	{
		RC::Unreal::FMemory::Free(BtlEvent);
		BtlEvent = nullptr;
	}
	
	if (GameState->EventManager->m_pBattleEvent)
	{
		switch (GameState->EventManager->m_CurrentBEMState)
		{
		case 0:
			BtlEvent = RC::Unreal::FMemory::Malloc(0x28);
			std::memcpy(BtlEvent, GameState->EventManager->m_pBattleEvent, 0x28);
			break;
		case 1:
			BtlEvent = RC::Unreal::FMemory::Malloc(0x30);
			std::memcpy(BtlEvent, GameState->EventManager->m_pBattleEvent, 0x30);
			break;
		case 2:
			BtlEvent = RC::Unreal::FMemory::Malloc(0x30);
			std::memcpy(BtlEvent, GameState->EventManager->m_pBattleEvent, 0x30);
			break;
		case 3:
			BtlEvent = RC::Unreal::FMemory::Malloc(0x30);
			std::memcpy(BtlEvent, GameState->EventManager->m_pBattleEvent, 0x30);
			break;
		case 4:
			BtlEvent = RC::Unreal::FMemory::Malloc(0x28);
			std::memcpy(BtlEvent, GameState->EventManager->m_pBattleEvent, 0x28);
			break;
		case 5:
			BtlEvent = RC::Unreal::FMemory::Malloc(0x30);
			std::memcpy(BtlEvent, GameState->EventManager->m_pBattleEvent, 0x30);
			break;
		case 7:
			BtlEvent = RC::Unreal::FMemory::Malloc(0x48);
			std::memcpy(BtlEvent, GameState->EventManager->m_pBattleEvent, 0x48);
			break;
		case 8:
			BtlEvent = RC::Unreal::FMemory::Malloc(0x50);
			std::memcpy(BtlEvent, GameState->EventManager->m_pBattleEvent, 0x50);
			break;
		case 9:
			BtlEvent = RC::Unreal::FMemory::Malloc(0x58);
			std::memcpy(BtlEvent, GameState->EventManager->m_pBattleEvent, 0x58);
			break;
		case 10:
			BtlEvent = RC::Unreal::FMemory::Malloc(0x50);
			std::memcpy(BtlEvent, GameState->EventManager->m_pBattleEvent, 0x50);
			break;
		case 11:
			BtlEvent = RC::Unreal::FMemory::Malloc(0x50);
			std::memcpy(BtlEvent, GameState->EventManager->m_pBattleEvent, 0x50);
			break;
		case 12:
			BtlEvent = RC::Unreal::FMemory::Malloc(0x28);
			std::memcpy(BtlEvent, GameState->EventManager->m_pBattleEvent, 0x28);
			break;
		case 14:
			BtlEvent = RC::Unreal::FMemory::Malloc(0x50);
			std::memcpy(BtlEvent, GameState->EventManager->m_pBattleEvent, 0x50);
			break;
		case 15:
			BtlEvent = RC::Unreal::FMemory::Malloc(0x40);
			std::memcpy(BtlEvent, GameState->EventManager->m_pBattleEvent, 0x40);
			break;
		case 16:
			BtlEvent = RC::Unreal::FMemory::Malloc(0x38);
			std::memcpy(BtlEvent, GameState->EventManager->m_pBattleEvent, 0x38);
			break;
		case 17:
			BtlEvent = RC::Unreal::FMemory::Malloc(0x30);
			std::memcpy(BtlEvent, GameState->EventManager->m_pBattleEvent, 0x30);
			break;
		case 18:
			BtlEvent = RC::Unreal::FMemory::Malloc(0x28);
			std::memcpy(BtlEvent, GameState->EventManager->m_pBattleEvent, 0x28);
			break;
		case 19:
			BtlEvent = RC::Unreal::FMemory::Malloc(0x40);
			std::memcpy(BtlEvent, GameState->EventManager->m_pBattleEvent, 0x40);
			break;
		case 20:
			BtlEvent = RC::Unreal::FMemory::Malloc(0x28);
			std::memcpy(BtlEvent, GameState->EventManager->m_pBattleEvent, 0x28);
			break;
		case 21:
		case 22:
			BtlEvent = RC::Unreal::FMemory::Malloc(0x88);
			std::memcpy(BtlEvent, GameState->EventManager->m_pBattleEvent, 0x88);
			break;
		case 23:
			BtlEvent = RC::Unreal::FMemory::Malloc(0x30);
			std::memcpy(BtlEvent, GameState->EventManager->m_pBattleEvent, 0x30);
			break;
		case 24:
			BtlEvent = RC::Unreal::FMemory::Malloc(0x40);
			std::memcpy(BtlEvent, GameState->EventManager->m_pBattleEvent, 0x40);
			break;
		case 25:
			BtlEvent = RC::Unreal::FMemory::Malloc(0x40);
			std::memcpy(BtlEvent, GameState->EventManager->m_pBattleEvent, 0x40);
			break;
		case 26:
			BtlEvent = RC::Unreal::FMemory::Malloc(0x38);
			std::memcpy(BtlEvent, GameState->EventManager->m_pBattleEvent, 0x38);
			break;
		case 27:
			BtlEvent = RC::Unreal::FMemory::Malloc(0x58);
			std::memcpy(BtlEvent, GameState->EventManager->m_pBattleEvent, 0x58);
			break;
		case 28:
			BtlEvent = RC::Unreal::FMemory::Malloc(0x30);
			std::memcpy(BtlEvent, GameState->EventManager->m_pBattleEvent, 0x30);
			break;
		case 29:
			BtlEvent = RC::Unreal::FMemory::Malloc(0x178);
			std::memcpy(BtlEvent, GameState->EventManager->m_pBattleEvent, 0x178);
			break;
		case 30:
			BtlEvent = RC::Unreal::FMemory::Malloc(0x68);
			std::memcpy(BtlEvent, GameState->EventManager->m_pBattleEvent, 0x68);
			break;
		case 31:
			BtlEvent = RC::Unreal::FMemory::Malloc(0x30);
			std::memcpy(BtlEvent, GameState->EventManager->m_pBattleEvent, 0x30);
			break;
		case 32:
			BtlEvent = RC::Unreal::FMemory::Malloc(0x30);
			std::memcpy(BtlEvent, GameState->EventManager->m_pBattleEvent, 0x30);
			break;
		case 33:
			BtlEvent = RC::Unreal::FMemory::Malloc(0x40);
			std::memcpy(BtlEvent, GameState->EventManager->m_pBattleEvent, 0x40);
			break;
		case 34:
			BtlEvent = RC::Unreal::FMemory::Malloc(0x30);
			std::memcpy(BtlEvent, GameState->EventManager->m_pBattleEvent, 0x30);
			break;
		case 35:
			BtlEvent = RC::Unreal::FMemory::Malloc(0x28);
			std::memcpy(BtlEvent, GameState->EventManager->m_pBattleEvent, 0x28);
			break;
		case 36:
			BtlEvent = RC::Unreal::FMemory::Malloc(0x48);
			std::memcpy(BtlEvent, GameState->EventManager->m_pBattleEvent, 0x48);
			break;
		case 37:
			BtlEvent = RC::Unreal::FMemory::Malloc(0x30);
			std::memcpy(BtlEvent, GameState->EventManager->m_pBattleEvent, 0x30);
			break;
		case 38:
			BtlEvent = RC::Unreal::FMemory::Malloc(0x38);
			std::memcpy(BtlEvent, GameState->EventManager->m_pBattleEvent, 0x38);
			break;
		case 39:
			BtlEvent = RC::Unreal::FMemory::Malloc(0x38);
			std::memcpy(BtlEvent, GameState->EventManager->m_pBattleEvent, 0x38);
			break;
		case 40:
			BtlEvent = RC::Unreal::FMemory::Malloc(0x28);
			std::memcpy(BtlEvent, GameState->EventManager->m_pBattleEvent, 0x28);
			break;
		case 41:
			BtlEvent = RC::Unreal::FMemory::Malloc(0x28);
			std::memcpy(BtlEvent, GameState->EventManager->m_pBattleEvent, 0x28);
			break;
		default:
			break;
		}
	}
	
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
			std::memcpy(&InObjManager->m_ObjVector[i].ObjBaseSyncBegin, &ObjManager->m_ObjVector[i].ObjBaseSyncBegin, sizeof(OBJ_CBase) - 8);
			auto from = StoredObjData[&ObjManager->m_ObjVector[i]];
			auto to = objData[&ObjManager->m_ObjVector[i]];
			std::memcpy(&to, &from, sizeof(OBJ_CBaseExt));
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
        std::memcpy(&InObjManager->m_CharVector[i].ObjBaseSyncBegin, &ObjManager->m_CharVector[i].ObjBaseSyncBegin, sizeof(OBJ_CCharBase) - 8);
		auto from = StoredObjData[&ObjManager->m_CharVector[i]];
		auto to = objData[&ObjManager->m_CharVector[i]];
		std::memcpy(&to, &from, sizeof(OBJ_CBaseExt));
	}
}

void RollbackData::LoadRPG(BATTLE_CObjectManager* InObjManager)
{
	for (int i = 0; i < 10; i++)
	{
		if (ObjManager->m_ObjVector[i].m_ActiveState > 0)
		{
            std::memcpy(&InObjManager->m_RpgVector[i].ObjBaseSyncBegin, &ObjManager->m_RpgVector[i].ObjBaseSyncBegin, sizeof(OBJ_CRPGBase) - 8);
			auto from = StoredObjData[&ObjManager->m_RpgVector[i]];
			auto to = objData[&ObjManager->m_RpgVector[i]];
			std::memcpy(&to, &from, sizeof(OBJ_CBaseExt));
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

	if (GameState->EventManager->m_pBattleEvent)
	{
		RC::Unreal::FMemory::Free(GameState->EventManager->m_pBattleEvent);
		GameState->EventManager->m_pBattleEvent = nullptr;
	}

	if (BtlEvent)
	{
		switch (GameState->EventManager->m_CurrentBEMState)
		{
		case 0:
			GameState->EventManager->m_pBattleEvent = RC::Unreal::FMemory::Malloc(0x28);
			std::memcpy(GameState->EventManager->m_pBattleEvent, BtlEvent, 0x28);
			break;
		case 1:
			GameState->EventManager->m_pBattleEvent = RC::Unreal::FMemory::Malloc(0x30);
			std::memcpy(GameState->EventManager->m_pBattleEvent, BtlEvent, 0x30);
			break;
		case 2:
			GameState->EventManager->m_pBattleEvent = RC::Unreal::FMemory::Malloc(0x30);
			std::memcpy(GameState->EventManager->m_pBattleEvent, BtlEvent, 0x30);
			break;
		case 3:
			GameState->EventManager->m_pBattleEvent = RC::Unreal::FMemory::Malloc(0x30);
			std::memcpy(GameState->EventManager->m_pBattleEvent, BtlEvent, 0x30);
			break;
		case 4:
			GameState->EventManager->m_pBattleEvent = RC::Unreal::FMemory::Malloc(0x28);
			std::memcpy(GameState->EventManager->m_pBattleEvent, BtlEvent, 0x28);
			break;
		case 5:
			GameState->EventManager->m_pBattleEvent = RC::Unreal::FMemory::Malloc(0x30);
			std::memcpy(GameState->EventManager->m_pBattleEvent, BtlEvent, 0x30);
			break;
		case 7:
			GameState->EventManager->m_pBattleEvent = RC::Unreal::FMemory::Malloc(0x48);
			std::memcpy(GameState->EventManager->m_pBattleEvent, BtlEvent, 0x48);
			break;
		case 8:
			GameState->EventManager->m_pBattleEvent = RC::Unreal::FMemory::Malloc(0x50);
			std::memcpy(GameState->EventManager->m_pBattleEvent, BtlEvent, 0x50);
			break;
		case 9:
			GameState->EventManager->m_pBattleEvent = RC::Unreal::FMemory::Malloc(0x58);
			std::memcpy(GameState->EventManager->m_pBattleEvent, BtlEvent, 0x58);
			break;
		case 10:
			GameState->EventManager->m_pBattleEvent = RC::Unreal::FMemory::Malloc(0x50);
			std::memcpy(GameState->EventManager->m_pBattleEvent, BtlEvent, 0x50);
			break;
		case 11:
			GameState->EventManager->m_pBattleEvent = RC::Unreal::FMemory::Malloc(0x50);
			std::memcpy(GameState->EventManager->m_pBattleEvent, BtlEvent, 0x50);
			break;
		case 12:
			GameState->EventManager->m_pBattleEvent = RC::Unreal::FMemory::Malloc(0x28);
			std::memcpy(GameState->EventManager->m_pBattleEvent, BtlEvent, 0x28);
			break;
		case 14:
			GameState->EventManager->m_pBattleEvent = RC::Unreal::FMemory::Malloc(0x50);
			std::memcpy(GameState->EventManager->m_pBattleEvent, BtlEvent, 0x50);
			break;
		case 15:
			GameState->EventManager->m_pBattleEvent = RC::Unreal::FMemory::Malloc(0x40);
			std::memcpy(GameState->EventManager->m_pBattleEvent, BtlEvent, 0x40);
			break;
		case 16:
			GameState->EventManager->m_pBattleEvent = RC::Unreal::FMemory::Malloc(0x38);
			std::memcpy(GameState->EventManager->m_pBattleEvent, BtlEvent, 0x38);
			break;
		case 17:
			GameState->EventManager->m_pBattleEvent = RC::Unreal::FMemory::Malloc(0x30);
			std::memcpy(GameState->EventManager->m_pBattleEvent, BtlEvent, 0x30);
			break;
		case 18:
			GameState->EventManager->m_pBattleEvent = RC::Unreal::FMemory::Malloc(0x28);
			std::memcpy(GameState->EventManager->m_pBattleEvent, BtlEvent, 0x28);
			break;
		case 19:
			GameState->EventManager->m_pBattleEvent = RC::Unreal::FMemory::Malloc(0x40);
			std::memcpy(GameState->EventManager->m_pBattleEvent, BtlEvent, 0x40);
			break;
		case 20:
			GameState->EventManager->m_pBattleEvent = RC::Unreal::FMemory::Malloc(0x28);
			std::memcpy(GameState->EventManager->m_pBattleEvent, BtlEvent, 0x28);
			break;
		case 21:
		case 22:
			GameState->EventManager->m_pBattleEvent = RC::Unreal::FMemory::Malloc(0x88);
			std::memcpy(GameState->EventManager->m_pBattleEvent, BtlEvent, 0x88);
			break;
		case 23:
			GameState->EventManager->m_pBattleEvent = RC::Unreal::FMemory::Malloc(0x30);
			std::memcpy(GameState->EventManager->m_pBattleEvent, BtlEvent, 0x30);
			break;
		case 24:
			GameState->EventManager->m_pBattleEvent = RC::Unreal::FMemory::Malloc(0x40);
			std::memcpy(GameState->EventManager->m_pBattleEvent, BtlEvent, 0x40);
			break;
		case 25:
			GameState->EventManager->m_pBattleEvent = RC::Unreal::FMemory::Malloc(0x40);
			std::memcpy(GameState->EventManager->m_pBattleEvent, BtlEvent, 0x40);
			break;
		case 26:
			GameState->EventManager->m_pBattleEvent = RC::Unreal::FMemory::Malloc(0x38);
			std::memcpy(GameState->EventManager->m_pBattleEvent, BtlEvent, 0x38);
			break;
		case 27:
			GameState->EventManager->m_pBattleEvent = RC::Unreal::FMemory::Malloc(0x58);
			std::memcpy(GameState->EventManager->m_pBattleEvent, BtlEvent, 0x58);
			break;
		case 28:
			GameState->EventManager->m_pBattleEvent = RC::Unreal::FMemory::Malloc(0x30);
			std::memcpy(GameState->EventManager->m_pBattleEvent, BtlEvent, 0x30);
			break;
		case 29:
			GameState->EventManager->m_pBattleEvent = RC::Unreal::FMemory::Malloc(0x178);
			std::memcpy(GameState->EventManager->m_pBattleEvent, BtlEvent, 0x178);
			break;
		case 30:
			GameState->EventManager->m_pBattleEvent = RC::Unreal::FMemory::Malloc(0x68);
			std::memcpy(GameState->EventManager->m_pBattleEvent, BtlEvent, 0x68);
			break;
		case 31:
			GameState->EventManager->m_pBattleEvent = RC::Unreal::FMemory::Malloc(0x30);
			std::memcpy(GameState->EventManager->m_pBattleEvent, BtlEvent, 0x30);
			break;
		case 32:
			GameState->EventManager->m_pBattleEvent = RC::Unreal::FMemory::Malloc(0x30);
			std::memcpy(GameState->EventManager->m_pBattleEvent, BtlEvent, 0x30);
			break;
		case 33:
			GameState->EventManager->m_pBattleEvent = RC::Unreal::FMemory::Malloc(0x40);
			std::memcpy(GameState->EventManager->m_pBattleEvent, BtlEvent, 0x40);
			break;
		case 34:
			GameState->EventManager->m_pBattleEvent = RC::Unreal::FMemory::Malloc(0x30);
			std::memcpy(GameState->EventManager->m_pBattleEvent, BtlEvent, 0x30);
			break;
		case 35:
			GameState->EventManager->m_pBattleEvent = RC::Unreal::FMemory::Malloc(0x28);
			std::memcpy(GameState->EventManager->m_pBattleEvent, BtlEvent, 0x28);
			break;
		case 36:
			GameState->EventManager->m_pBattleEvent = RC::Unreal::FMemory::Malloc(0x48);
			std::memcpy(GameState->EventManager->m_pBattleEvent, BtlEvent, 0x48);
			break;
		case 37:
			GameState->EventManager->m_pBattleEvent = RC::Unreal::FMemory::Malloc(0x30);
			std::memcpy(GameState->EventManager->m_pBattleEvent, BtlEvent, 0x30);
			break;
		case 38:
			GameState->EventManager->m_pBattleEvent = RC::Unreal::FMemory::Malloc(0x38);
			std::memcpy(GameState->EventManager->m_pBattleEvent, BtlEvent, 0x38);
			break;
		case 39:
			GameState->EventManager->m_pBattleEvent = RC::Unreal::FMemory::Malloc(0x38);
			std::memcpy(GameState->EventManager->m_pBattleEvent, BtlEvent, 0x38);
			break;
		case 40:
			GameState->EventManager->m_pBattleEvent = RC::Unreal::FMemory::Malloc(0x28);
			std::memcpy(GameState->EventManager->m_pBattleEvent, BtlEvent, 0x28);
			break;
		case 41:
			GameState->EventManager->m_pBattleEvent = RC::Unreal::FMemory::Malloc(0x28);
			std::memcpy(GameState->EventManager->m_pBattleEvent, BtlEvent, 0x28);
			break;
		default:
			break;
		}
	}
	GameState->BattleScreenManager->m_pCamera->m_Pos = CamRollbackData.m_Pos;
	GameState->BattleScreenManager->m_pCamera->m_Up = CamRollbackData.m_Up;
	GameState->BattleScreenManager->m_pCamera->m_At = CamRollbackData.m_At;
	GameState->BGLocation->m_OffsetMatrix = BGRollbackData.m_OffsetMatrix;
	GameState->BGLocation->m_OffsetMove_Location = BGRollbackData.m_OffsetMove_Location;
	GameState->BGLocation->m_OffsetMove_Rotation = BGRollbackData.m_OffsetMove_Rotation;
}