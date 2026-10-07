#include "BattleState.hpp"
#include "RollbackContext.hpp"
#include "Unreal.hpp"
#include "lz4.h"

#include <vector>

namespace
{
	constexpr uint16_t BattleEventSizes[BEMCount + 1] = {
		0x28, 0x30, 0x30, 0x30, 0x28, 0x30, 0x000, 0x48,
		0x50, 0x58, 0x50, 0x50, 0x28, 0x000, 0x50, 0x40,
		0x38, 0x30, 0x28, 0x40, 0x28, 0x88, 0x88, 0x30,
		0x40, 0x40, 0x38, 0x58, 0x30, 0x178, 0x68, 0x30,
		0x30, 0x40, 0x30, 0x28, 0x48, 0x30, 0x38, 0x38,
		0x28, 0x28,
	};

	static_assert(std::size(BattleEventSizes) == BEMCount + 1);
}

size_t GetBattleEventSize(uint32_t BEMState)
{
	if (BEMState > BEMCount)
		return 0;

	return BattleEventSizes[BEMState];
}

void RollbackData::SaveState(RollbackContext& Ctx)
{
	auto* GameState = Ctx.GameState;
	auto* Manager = GameState->BattleObjectManager;

	std::memcpy(ObjManagerHead, reinterpret_cast<const char*>(Manager) + ObjManagerHeadOffset, ObjManagerHeadSize);
	std::memcpy(ObjManagerTail, reinterpret_cast<const char*>(Manager) + ObjManagerTailOffset, ObjManagerTailSize);

	for (int32_t i = 0; i < ObjCount; i++)
	{
		bObjActive[i] = Manager->m_ObjVector[i].m_ActiveState != ACTV_NOT_ACTIVE;
		if (bObjActive[i])
		{
			std::memcpy(Obj[i], reinterpret_cast<const char*>(&Manager->m_ObjVector[i]) + SyncSkipOffset, sizeof(Obj[i]));
		}
	}

	for (int32_t i = 0; i < CharCount; i++)
		std::memcpy(Chara[i], reinterpret_cast<const char*>(&Manager->m_CharVector[i]) + SyncSkipOffset, sizeof(Chara[i]));

	for (int32_t i = 0; i < StageCount; i++)
		std::memcpy(Stage[i], reinterpret_cast<const char*>(&Manager->m_StageVector[i]) + SyncSkipOffset, sizeof(Stage[i]));

	std::memcpy(ObjExt, Ctx.ObjExt, sizeof(ObjExt));

	std::memcpy(ScrManager, GameState->BattleScreenManager, sizeof(ScrManager));
	std::memcpy(State, reinterpret_cast<const char*>(GameState->State) + SyncSkipOffset, sizeof(State));
	std::memcpy(EvtManager, reinterpret_cast<const char*>(GameState->EventManager) + SyncSkipOffset, sizeof(EvtManager));

	BattleEventState = static_cast<uint32_t>(GameState->EventManager->m_CurrentBEMState);
	BattleEventSize = 0;
	std::memset(BattleEvent, 0, sizeof(BattleEvent));

	if (GameState->EventManager->m_pBattleEvent)
	{
		if (const size_t Size = GetBattleEventSize(BattleEventState))
		{
			std::memcpy(BattleEvent, GameState->EventManager->m_pBattleEvent, Size);
			BattleEventSize = static_cast<uint32_t>(Size);
		}
	}

	Cam.m_Pos = GameState->BattleScreenManager->m_pCamera->m_Pos;
	Cam.m_Up = GameState->BattleScreenManager->m_pCamera->m_Up;
	Cam.m_At = GameState->BattleScreenManager->m_pCamera->m_At;

	BG.m_OffsetMatrix = GameState->BGLocation->m_OffsetMatrix;
	BG.m_OffsetMove_Location = GameState->BGLocation->m_OffsetMove_Location;
	BG.m_OffsetMove_Rotation = GameState->BGLocation->m_OffsetMove_Rotation;

	if (Ctx.Random)
	{
		std::memcpy(Rand.m_State, Ctx.Random->m_State, sizeof(Rand.m_State));
		Rand.m_Left = Ctx.Random->m_Left;
		Rand.m_Initf = Ctx.Random->m_Initf;
		Rand.m_pNext = Ctx.Random->m_pNext;
	}

	SavedGameFrame = Ctx.GameFrame;
	Inputs[0] = Ctx.Inputs[0];
	Inputs[1] = Ctx.Inputs[1];
}

void RollbackData::LoadState(RollbackContext& Ctx)
{
	auto* GameState = Ctx.GameState;
	auto* Manager = GameState->BattleObjectManager;

	std::memcpy(reinterpret_cast<char*>(Manager) + ObjManagerHeadOffset, ObjManagerHead, ObjManagerHeadSize);
	std::memcpy(reinterpret_cast<char*>(Manager) + ObjManagerTailOffset, ObjManagerTail, ObjManagerTailSize);

	for (int32_t i = 0; i < ObjCount; i++)
	{
		if (bObjActive[i])
			std::memcpy(reinterpret_cast<char*>(&Manager->m_ObjVector[i]) + SyncSkipOffset, Obj[i], sizeof(Obj[i]));
		else
			Manager->m_ObjVector[i].m_ActiveState = ACTV_NOT_ACTIVE;
	}

	for (int32_t i = 0; i < CharCount; i++)
		std::memcpy(reinterpret_cast<char*>(&Manager->m_CharVector[i]) + SyncSkipOffset, Chara[i], sizeof(Chara[i]));

	for (int32_t i = 0; i < StageCount; i++)
		std::memcpy(reinterpret_cast<char*>(&Manager->m_StageVector[i]) + SyncSkipOffset, Stage[i], sizeof(Stage[i]));

	std::memcpy(Ctx.ObjExt, ObjExt, sizeof(ObjExt));

	std::memcpy(GameState->BattleScreenManager, ScrManager, sizeof(ScrManager));
	std::memcpy(reinterpret_cast<char*>(GameState->State) + SyncSkipOffset, State, sizeof(State));

	void* LiveBattleEvent = GameState->EventManager->m_pBattleEvent;

	std::memcpy(reinterpret_cast<char*>(GameState->EventManager) + SyncSkipOffset, EvtManager, sizeof(EvtManager));

	if (LiveBattleEvent)
		RC::Unreal::FMemory::Free(LiveBattleEvent);

	GameState->EventManager->m_pBattleEvent = nullptr;

	if (BattleEventSize)
	{
		void* Event = RC::Unreal::FMemory::Malloc(BattleEventSize);
		std::memcpy(Event, BattleEvent, BattleEventSize);
		GameState->EventManager->m_pBattleEvent = Event;
	}

	GameState->BattleScreenManager->m_pCamera->m_Pos = Cam.m_Pos;
	GameState->BattleScreenManager->m_pCamera->m_Up = Cam.m_Up;
	GameState->BattleScreenManager->m_pCamera->m_At = Cam.m_At;

	GameState->BGLocation->m_OffsetMatrix = BG.m_OffsetMatrix;
	GameState->BGLocation->m_OffsetMove_Location = BG.m_OffsetMove_Location;
	GameState->BGLocation->m_OffsetMove_Rotation = BG.m_OffsetMove_Rotation;

	if (Ctx.Random)
	{
		std::memcpy(Ctx.Random->m_State, Rand.m_State, sizeof(Rand.m_State));
		Ctx.Random->m_Left = Rand.m_Left;
		Ctx.Random->m_Initf = Rand.m_Initf;
		Ctx.Random->m_pNext = Rand.m_pNext;
	}

	Ctx.GameFrame = SavedGameFrame;
	Ctx.Inputs[0] = Inputs[0];
	Ctx.Inputs[1] = Inputs[1];
}

namespace
{
	constexpr size_t FixedRegionSize = offsetof(RollbackData, Obj);
	constexpr size_t ObjPayloadSize = sizeof(RollbackData::Obj[0]);

	std::vector<uint8_t>& PackBuffer()
	{
		static std::vector<uint8_t> Buffer(sizeof(RollbackData));
		return Buffer;
	}

	size_t PackSnapshot(const RollbackData& Snapshot, uint8_t* Dst)
	{
		std::memcpy(Dst, &Snapshot, FixedRegionSize);
		size_t Out = FixedRegionSize;

		for (int32_t i = 0; i < ObjCount; i++)
		{
			if (!Snapshot.bObjActive[i])
				continue;

			std::memcpy(Dst + Out, Snapshot.Obj[i], ObjPayloadSize);
			Out += ObjPayloadSize;
		}

		return Out;
	}

	bool UnpackSnapshot(RollbackData& Snapshot, const uint8_t* Src, size_t Size)
	{
		if (Size < FixedRegionSize)
			return false;

		std::memcpy(&Snapshot, Src, FixedRegionSize);
		size_t In = FixedRegionSize;

		for (int32_t i = 0; i < ObjCount; i++)
		{
			if (!Snapshot.bObjActive[i])
				continue;

			if (In + ObjPayloadSize > Size)
				return false;

			std::memcpy(Snapshot.Obj[i], Src + In, ObjPayloadSize);
			In += ObjPayloadSize;
		}

		return In == Size;
	}
}

size_t RollbackData::PackedSize() const
{
	size_t Live = 0;
	for (const bool i : bObjActive)
		if (i) Live++;

	return FixedRegionSize + Live * ObjPayloadSize;
}

size_t RollbackData::Compress(uint8_t* Dst, size_t DstSize) const
{
	auto& Buffer = PackBuffer();
	const size_t SrcSize = PackSnapshot(*this, Buffer.data());

	return LZ4_compress_default((const char*)Buffer.data(), (char*)Dst, SrcSize, DstSize);
}

bool RollbackData::Decompress(const uint8_t* Src, size_t SrcSize)
{
	auto& Buffer = PackBuffer();
	const size_t Unpacked = LZ4_decompress_safe((const char*)Src, (char*)Buffer.data(), SrcSize, Buffer.size());
	if (Unpacked == 0)
		return false;

	return UnpackSnapshot(*this, Buffer.data(), Unpacked);
}