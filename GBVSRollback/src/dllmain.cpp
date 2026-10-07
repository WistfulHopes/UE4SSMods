#include "safetyhook.hpp"
#include <BattleState.hpp>
#include <Particles.hpp>
#include <Unreal.hpp>
#include <UE4SSProgram.hpp>
#include <UnrealDef.hpp>
#include <Mod/CppUserModBase.hpp>
#include <polyhook2/Exceptions/AVehHook.hpp>
#include <Unreal/AGameModeBase.hpp>

#include "Pawn.hpp"
#include "RollbackContext.hpp"
#include "../deps/GekkoNet/GekkoLib/include/gekkonet.h"
#include "Containers/FAnsiString.hpp"
#include "SigScanner/SinglePassSigScanner.hpp"

GekkoSession* Session;

inline constexpr size_t GekkoStateSize = 1024 * 4 * 1024;

RollbackData* ScratchState = nullptr;

SafetyHookInline DeleteLinkPSC_Detour{};
SafetyHookInline UEParticleGet_Detour{};
SafetyHookInline CreateParticleArg_Detour{};
SafetyHookInline LinkParticleArg_Detour{};
SafetyHookInline OBJ_CBase_Ctor_Detour{};
SafetyHookInline ObjectConstructor_Detour{};
SafetyHookInline AfterFrameStep_Detour{};
SafetyHookInline UpdateBattle_Detour{};
SafetyHookInline LinkCommonPawnEffect_Detour{};

SafetyHookMid CreateParticleFromParticleDataAsset{};
SafetyHookMid ReleaseResource{};
SafetyHookMid ControlBattleObject_KeyRenewal_Detour{};
SafetyHookMid LinkPawn_Cache_Detour{};
SafetyHookMid LinkPawn_OnLink_Detour{};
SafetyHookMid LinkCommonPawnEffect_OnLink_Detour{};
SafetyHookMid LinkCommonPawn_Detour{};
SafetyHookMid LinkPawnTeamMember_Detour{};

// Particles

UParticleSystemComponent* pscToCache;

void DeleteLinkPSC_Hook(OBJ_CBase* pThis)
{
    DeleteLinkPSC_Detour.call(pThis);
    GetRollbackContext().GetObjExt(pThis).m_LinkParticleActiveFrame = -1;
}

void UEParticleGet_Hook(void* pThis, UParticleSystemComponent* UEParticle, void* arg)
{
    pscToCache = UEParticle;
    UEParticleGet_Detour.call(pThis, UEParticle, arg);
}

typedef void (*SetComponentTickEnabled_Func)(Unreal::UActorComponent*, bool bEnabled);
SetComponentTickEnabled_Func SetComponentTickEnabled;

typedef void* (*GetBattleKeyFromPadID_Func)(void*, int32_t padID);
GetBattleKeyFromPadID_Func GetBattleKeyFromPadID;

typedef int32_t (*Side2Pad_Func)(int32_t sideID, int32_t memberID);
Side2Pad_Func Side2Pad;

typedef int32_t (*GKF2RecFlag_Func)(int32_t padID, int32_t flag, bool isReverse);
GKF2RecFlag_Func GKF2RecFlag;

typedef void (*DeactivateSystem_Func)(UParticleSystemComponent*);
DeactivateSystem_Func DeactivateSystem;

typedef void (*KillParticlesForced_Func)(UParticleSystemComponent*);
KillParticlesForced_Func KillParticlesForced;

typedef void (*ReturnToEffectManager_Func)(AREDPawnEffect*);
ReturnToEffectManager_Func ReturnToEffectManager;

typedef void (*LinkCommonPawn_Func)(OBJ_CBase*);
LinkCommonPawn_Func LinkCommonPawn;

typedef void (*SwitchMeshSet_Func)(AREDPawn*, FName);
SwitchMeshSet_Func SwitchMeshSet;

typedef void (*SetupMaterials_Func)(AREDPawn*, FName, bool);
SetupMaterials_Func SetupMaterials;

ClearLinkModel_Func ClearLinkModel;

SetActorHiddenInGame_Func SetActorHiddenInGame;

SetActorHiddenInGame_Func SetActorTickEnabled;

void CreateParticleArg_Hook(OBJ_CBase* pThis, CXXBYTE<32>* name, int pos)
{
    if (Rollback_ProcessCachedUnlinkedPSC(pThis, name))
    {
        pThis->m_CreateArg.m_CreateArg_SocketName.m_Buf[0] = 0;
        pThis->m_CreateArg.m_CreateArg_Angle = 0;
        pThis->m_CreateArg.m_CreateArg_AngleY = 0;
        pThis->m_CreateArg.m_CreateArg_OffsetPosX = 0;
        pThis->m_CreateArg.m_CreateArg_OffsetPosY = 0;
        pThis->m_CreateArg.m_CreateArg_OffsetPosZ = 0;
        pThis->m_CreateArg.m_CreateArg_ScaleX = 1000;
        pThis->m_CreateArg.m_CreateArg_ScaleY = 1000;
        pThis->m_CreateArg.m_CreateArg_ScaleZ = 1000;
        pThis->m_CreateArg.m_CreateArg_Hikitsugi0 = 0;
        pThis->m_CreateArg.m_CreateArg_HkrColor = -1;
        pThis->m_CreateArg.m_CreateArg_MltColor = -1;
        pThis->m_CreateArg.m_CreateArg_TransPriority = 0;
        pThis->m_CreateArg.m_CreateArg_Direction = 0;
        pThis->m_CreateArg.m_CreateArg_SocketUse = false;
        pThis->m_CreateArg.m_CreateArg_SocketWithRot = false;
        pThis->m_CreateArg.m_CreateArg_NoAssert = false;
        pThis->m_CreateArg.m_CreateArg_PointLightSide = 0;
        pThis->m_CreateArg.m_CreateArg_PointLightMember = 0;
        return;
    }
    CreateParticleArg_Detour.call<void>(pThis, name, pos);
    AddUnlinkedPSCToCache(pThis, name, pscToCache);
}

void LinkParticleArg_Hook(OBJ_CBase* pThis, CXXBYTE<32>* name, int objType)
{
    if (Rollback_ProcessCachedPSC(pThis, name))
    {
        return;
    }
    
    LinkParticleArg_Detour.call<void>(pThis, name, objType);
    
    if (name->m_Buf[0] == 0) return;
    
    Rollback_OnLinkParticle(pThis, name);
    if (GetRollbackContext().GetObjExt(pThis).m_LinkParticleActiveFrame == -1)
    {
        GetRollbackContext().GetObjExt(pThis).m_LinkParticleActiveFrame = 0;
    }
}

void Obj_OnPreBackup(OBJ_CBase* pThis)
{
    GetRollbackContext().GetObjExt(pThis).m_RollbackData.PointLightId = 0;
    if (pThis->m_pLinkPSC)
    {
        GetRollbackContext().GetObjExt(pThis).m_RollbackData.bLinkParticleSet = true;
        GetRollbackContext().GetObjExt(pThis).m_RollbackData.PointLightId = pThis->m_pLinkPSC->PointLightId;
    }
    else
    {
        GetRollbackContext().GetObjExt(pThis).m_RollbackData.bLinkParticleSet = false;
    }

    GetRollbackContext().GetObjExt(pThis).m_RollbackData.LinkPawnSet = pThis->m_pPawn && !pThis->m_IsPlayerObj;

    if (pThis->m_pPawn)
    {
        strncpy(GetRollbackContext().GetObjExt(pThis).m_RollbackData.MeshSetName.m_Buf,
                pThis->m_pPawn->CurrentMeshSetName.ToFAnsiString().GetCharArray().GetData(), 0x20);
        strncpy(GetRollbackContext().GetObjExt(pThis).m_RollbackData.MaterialSetName.m_Buf,
                pThis->m_pPawn->CurrentMaterialSetName.ToFAnsiString().GetCharArray().GetData(), 0x20);
        GetRollbackContext().GetObjExt(pThis).m_RollbackData.LinkPawnPlayingCutSceneAnime = pThis->m_pPawn->bPlayingCutSceneAnime;
    }

    if (pThis->m_pPawn && !pThis->m_StepAnimeMode)
    {
        GetRollbackContext().GetObjExt(pThis).m_RollbackData.bPlayAnimeSet = true;
        GetRollbackContext().GetObjExt(pThis).m_RollbackData.PlayAnimeCurrentTime = pThis->m_pPawn->CutSceneAnimeTime;
        GetRollbackContext().GetObjExt(pThis).m_RollbackData.PlayAnimCutSceneStepFramePawn = pThis->m_pPawn->CutSceneStepFrame;
        GetRollbackContext().GetObjExt(pThis).m_RollbackData.PlayAnimCutSceneStepFrameAnimInst = 0;
    }
    else
    {
        GetRollbackContext().GetObjExt(pThis).m_RollbackData.bPlayAnimeSet = false;
        GetRollbackContext().GetObjExt(pThis).m_RollbackData.PlayAnimeCurrentTime = 0;
        GetRollbackContext().GetObjExt(pThis).m_RollbackData.PlayAnimCutSceneStepFramePawn = 0;
        GetRollbackContext().GetObjExt(pThis).m_RollbackData.PlayAnimCutSceneStepFrameAnimInst = 0;
    }
}

void PreBackup()
{
    for (int i = 0; i < GetRollbackContext().GameState->BattleObjectManager->m_ActiveObjectCount; i++)
    {
        Obj_OnPreBackup(GetRollbackContext().GameState->BattleObjectManager->m_SortedObjPtrVector[i]);
    }
}

void ClearPSCCache()
{
    for (const auto& val : GetRollbackContext().Particles.pscCache | std::views::values)
    {
        if (!val) continue;
        val->LinkObjPtr = nullptr;
        val->bAutoDestroy = true;
        SetComponentTickEnabled(val, true);
        DeactivateSystem(val);
        KillParticlesForced(val);
    }
    GetRollbackContext().Particles.pscCache.clear();
    Unreal::FMemory::Memzero(GetRollbackContext().Pawns.PawnQuickRestoreCache);
}

void ClearPawnCache()
{
    for (const auto& val : GetRollbackContext().Pawns.pawnCache | std::views::values)
    {
        if (!val) continue;
        if (val->IsA(AREDPawnEffect::StaticClass))
        {
            val->ObjPtr = nullptr;
            ReturnToEffectManager((AREDPawnEffect*)val);
        }
    }

    GetRollbackContext().Pawns.pawnCache.clear();
}

void PreRollback()
{
    ClearPSCCache();
    ClearPawnCache();
    for (int i = 0; i < GetRollbackContext().GameState->BattleObjectManager->m_ActiveObjectCount; i++)
    {
        OBJ_CBase* obj = GetRollbackContext().GameState->BattleObjectManager->m_SortedObjPtrVector[i];
        if (!obj)
            break;
        if (obj->m_pLinkPSC)
        {
            if (GetRollbackContext().GetObjExt(obj).m_LinkParticleActiveFrame <= 1)
            {
                if (AddLinkPSCToCache(obj))
                {
                    obj->m_pLinkPSC = nullptr;
                }
            }
            else
            {
                obj->m_pLinkPSC = nullptr;
            }
        }
        if (!obj->m_IsPlayerObj && obj->m_pPawn && !GetRollbackContext().GetObjExt(obj).m_RollbackData.LinkCommonPawn)
        {
            if (GetRollbackContext().GetObjExt(obj).m_RollbackData.LinkPawnStartFrame >= GetRollbackContext().GameFrame - 1)
            {
                if (AddLinkPawnToCache(obj))
                {
                    obj->m_pPawn = nullptr;
                }
            }
            else
            {
                AddLinkPawnForQuickRestore(obj);
                obj->m_pPawn = nullptr;
            }
        }
    }
}

void ClearUnusedUnlinkedPSC()
{
    for (auto& psc : GetRollbackContext().Particles.unlinkedPscCache)
    {
        if (!GetRollbackContext().Particles.unlinkedPscCacheUsed.contains(psc.first) && psc.second)
        {
            psc.second->bAutoDestroy = true;
            SetComponentTickEnabled(psc.second, true);
            DeactivateSystem(psc.second);
            KillParticlesForced(psc.second);
        }
    }

    GetRollbackContext().Particles.unlinkedPscCache.clear();
}

void Obj_OnPostRollback(OBJ_CBase* pThis)
{
    if (!pThis->m_IsPlayerObj)
    {
        if (GetRollbackContext().GetObjExt(pThis).m_RollbackData.bLinkParticleSet)
        {
            if (pThis->m_pLinkPSC)
            {
                pThis->m_pLinkPSC->PointLightId = GetRollbackContext().GetObjExt(pThis).m_RollbackData.PointLightId;
            }
        }

        if (GetRollbackContext().GetObjExt(pThis).m_RollbackData.LinkPawnSet)
        {
            if (GetRollbackContext().GetObjExt(pThis).m_RollbackData.LinkCommonPawn)
            {
                LinkCommonPawn(pThis);
            }
        }
    }
    else
    {
        SwitchMeshSet(pThis->m_pPawn, FName(to_wstring(GetRollbackContext().GetObjExt(pThis).m_RollbackData.MeshSetName.m_Buf)));
        SetupMaterials(pThis->m_pPawn, FName(to_wstring(GetRollbackContext().GetObjExt(pThis).m_RollbackData.MaterialSetName.m_Buf)), false);
    }

    if (GetRollbackContext().GetObjExt(pThis).m_RollbackData.bPlayAnimeSet && pThis->m_pPawn)
    {
        pThis->m_pPawn->CutSceneStepFrame = GetRollbackContext().GetObjExt(pThis).m_RollbackData.PlayAnimCutSceneStepFramePawn;
        pThis->m_pPawn->CutSceneAnimeTime = GetRollbackContext().GetObjExt(pThis).m_RollbackData.PlayAnimeCurrentTime;
    }

    if (pThis->m_pPawn)
    {
        pThis->m_pPawn->bPlayingCutSceneAnime = GetRollbackContext().GetObjExt(pThis).m_RollbackData.LinkPawnPlayingCutSceneAnime;
    }
}

void ClearOldUnlinkedPSC_PostRollback()
{
    GetRollbackContext().Particles.unlinkedPscCacheUsed.clear();

    std::vector<FPSCCacheKey> deleteTemp;

    for (auto& psc : GetRollbackContext().Particles.unlinkedPscCache)
    {
        if (psc.first.GameFrame < GetRollbackContext().GameFrame || !GetRollbackContext().GameState->PSCManager->GetComponentsByClass(
            UParticleSystemComponent::StaticClass).Contains(psc.second))
        {
            deleteTemp.push_back(psc.first);
        }
    }

    for (auto& key : deleteTemp)
    {
        GetRollbackContext().Particles.unlinkedPscCache.erase(key);
    }
}

void PostRollback()
{
    SetActorHiddenInGame(GetRollbackContext().GameState->BattleObjectManager->m_pCommonPlayerPawn, true);
    GetRollbackContext().GameState->BattleObjectManager->m_pCommonPlayerPawn->ObjPtr = nullptr;

    for (int i = 0; i < GetRollbackContext().GameState->BattleObjectManager->m_ActiveObjectCount; i++)
    {
        auto obj = GetRollbackContext().GameState->BattleObjectManager->m_SortedObjPtrVector[i];
        if (!obj->m_IsPlayerObj)
        {
            obj->m_pPawn = nullptr;
        }
    }

    ClearOldUnlinkedPSC_PostRollback();

    for (auto components = GetRollbackContext().GameState->PSCManager->GetComponentsByClass(UParticleSystemComponent::StaticClass); const
         auto component : components)
    {
        auto psc = reinterpret_cast<UParticleSystemComponent*>(component);
        if (!psc || !psc->LinkObjPtr)
            continue;

        if (PSC_GetElapsedFrames(psc) <= 0)
            continue;

        psc->LinkObjPtr->m_pLinkPSC = psc;
    }

    QuickRestoreLinkPawns();

    for (int i = 0; i < GetRollbackContext().GameState->BattleObjectManager->m_ActiveObjectCount; i++)
    {
        Obj_OnPostRollback(GetRollbackContext().GameState->BattleObjectManager->m_SortedObjPtrVector[i]);
    }
}

// Battle

void OBJ_CBase_Ctor_Hook(OBJ_CBase* pThis)
{
    OBJ_CBase_Ctor_Detour.call(pThis);
    GetRollbackContext().GetObjExt(pThis) = OBJ_CBaseExt();
}

void ObjectConstructor_Hook(OBJ_CBase* pThis)
{
    ObjectConstructor_Detour.call(pThis);
    GetRollbackContext().GetObjExt(pThis) = OBJ_CBaseExt();
}

bool AfterFrameStep_Hook(OBJ_CBase* pThis)
{
    if (pThis->m_pLinkPSC)
    {
        GetRollbackContext().GetObjExt(pThis).m_LinkParticleActiveFrame++;
    }
    return AfterFrameStep_Detour.call<bool>(pThis);
}

void SaveForRollback(RollbackData& Snapshot)
{
    auto& Ctx = GetRollbackContext();

    Ctx.bIsSave = true;
    Ctx.bIsFastForward = false;
    PreBackup();
    Snapshot.SaveState(Ctx);
    Ctx.bIsSave = false;
}

void LoadForRollback(RollbackData& Snapshot)
{
    auto& Ctx = GetRollbackContext();

    Ctx.bIsSave = false;
    Ctx.bIsFastForward = true;
    PreRollback();
    Snapshot.LoadState(Ctx);
    PostRollback();
    Ctx.bIsFastForward = false;
}

int32_t BattleChecksum(BATTLE_CObjectManager* ctx)
{
    int Checksum = 0;
    for (int i = 0; i < 6; i++)
    {
        const auto& Player = ctx->m_CharVector[i];
        Checksum += Player.m_ActionTime ^ Player.m_ActionTime << 16;
        Checksum += Player.m_PosX ^ Player.m_PosX << 16;
        Checksum += Player.m_PosY ^ Player.m_PosY << 16;
        Checksum += Player.m_HitPoint ^ Player.m_HitPoint << 16;
    }
    
    Checksum += ctx->m_TeamManager[0].m_TensionVal ^ ctx->m_TeamManager[0].m_TensionVal << 16;
    Checksum += ctx->m_TeamManager[1].m_TensionVal ^ ctx->m_TeamManager[1].m_TensionVal << 16;
    
    return Checksum;
}

void UpdateBattle_Hook(AREDGameState_Battle* pThis, float DeltaTime, bool bUpdateDraw)
{
    auto& Ctx = GetRollbackContext();

    if (Ctx.Random == nullptr)
    {
        Ctx.Random = *reinterpret_cast<AA_CRandMT**>(*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(
            GetModuleHandle(nullptr)) + 0x35916b8) + 0x140);
    }

    if (!Ctx.bRollbackTest || !Session || !ScratchState)
    {
        UpdateBattle_Detour.call(pThis, DeltaTime, bUpdateDraw);
        Ctx.GameFrame++;
        Ctx.bIsFastForward = false;
        return;
    }

    for (int i = 0; i < 2; i++)
    {
        auto GKFFlag = *(int32_t*)(*(uintptr_t*)(*(uintptr_t*)Ctx.SystemManager + 0x1F8 + Side2Pad(i, 0) * 8) + 0x78);
        auto RecFlag = GKF2RecFlag(Side2Pad(i, 0), GKFFlag, false);
        gekko_add_local_input(Session, i, &RecFlag);
    }

    int Count = 0;
    GekkoSessionEvent** Events = gekko_session_events(Session, &Count);
    for (int i = 0; i < Count; i++)
    {
        if (Events[i]->type != GekkoDesyncDetected)
            continue;

        const auto Desync = Events[i]->data.desynced;
        Output::send(std::format(
            STR("DESYNCED: frame: {}, remote handle: {}, local checksum: {}, remote checksum: {}"),
            Desync.frame, Desync.remote_handle, Desync.local_checksum, Desync.remote_checksum));
        throw std::runtime_error("Desync occured");
    }

    Count = 0;
    GekkoGameEvent** Updates = gekko_update_session(Session, &Count);

    int AdvancesRemaining = 0;
    int ReplayedAdvances = 0;
    for (int i = 0; i < Count; i++)
    {
        if (Updates[i]->type != GekkoAdvanceEvent)
            continue;

        AdvancesRemaining++;
        if (Updates[i]->data.adv.rolling_back)
            ReplayedAdvances++;
    }

    for (int i = 0; i < Count; i++)
    {
        GekkoGameEvent* Event = Updates[i];
        switch (Event->type)
        {
        case GekkoSaveEvent:
            {
                SaveForRollback(*ScratchState);

                *Event->data.save.checksum = BattleChecksum(pThis->BattleObjectManager);

                const size_t Written = ScratchState->Compress(Event->data.save.state, GekkoStateSize);
                if (Written == 0)
                {
                    Output::send(STR("[GBVSRollback] snapshot did not fit in GekkoStateSize\n"));
                    throw std::runtime_error("Rollback snapshot exceeded Gekko's state buffer");
                }

                *Event->data.save.state_len = static_cast<unsigned int>(Written);
                break;
            }

        case GekkoLoadEvent:
            {
                if (!ScratchState->Decompress(Event->data.load.state, Event->data.load.state_len))
                {
                    Output::send(STR("[GBVSRollback] could not decompress a saved frame\n"));
                    throw std::runtime_error("Rollback snapshot was malformed");
                }

                LoadForRollback(*ScratchState);

                PSC_RewindElapsedFrames(ReplayedAdvances + 1);
                break;
            }

        case GekkoAdvanceEvent:
            {
                const auto* FrameInputs = reinterpret_cast<const int32_t*>(Event->data.adv.inputs);
                Ctx.Inputs[0] = FrameInputs[0];
                Ctx.Inputs[1] = FrameInputs[1];

                Ctx.bIsFastForward = Event->data.adv.rolling_back;

                AdvancesRemaining--;
                Ctx.RollbackFramesRemaining = AdvancesRemaining;

                UpdateBattle_Detour.call(pThis, DeltaTime, bUpdateDraw);

                PSC_TickElapsedFrames();

                Ctx.bIsFastForward = false;
                Ctx.RollbackFramesRemaining = 0;
                Ctx.GameFrame++;
                break;
            }

        default:
            break;
        }
    }

    ClearPSCCache();
    ClearUnusedUnlinkedPSC();

    Ctx.bIsFastForward = false;
}

void LinkCommonPawnEffect(OBJ_CBase* ctx, CXXBYTE<32>* name)
{
    if (Rollback_ProcessCachedPawn(ctx, name, false, true))
    {
        return;
    }

    LinkCommonPawnEffect_Detour.call(ctx, name);
}

void CreateGekkoStressSession()
{
    Session = nullptr;
    gekko_create(&Session, GekkoStressSession);

    GekkoConfig Config{};

    Config.desync_detection = true;
    Config.input_size = 4;

    Config.state_size = GekkoStateSize;
    Config.num_players = 2;
    Config.check_distance = 10;

    Config.input_prediction_window = 8;

    Config.limited_saving = false;

    gekko_start(Session, &Config);

    for (int i = 0; i < 2; i++)
    {
        gekko_add_actor(Session, GekkoLocalPlayer, nullptr);
        gekko_set_local_delay(Session, i, 1);
    }
}

// Mod

class GBVSRollback : public CppUserModBase
{
public:
    GBVSRollback()
    {
        ModName = STR("GBVSRollback");
        ModVersion = STR("1.0");
        ModDescription = STR("GBVS Rollback");
        ModAuthors = STR("WistfulHopes");
    }

    ~GBVSRollback() override = default;

    auto on_update() -> void override
    {
    }

    auto on_unreal_init() -> void override
    {
        GetRollbackContext().SystemManager = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(GetModuleHandle(nullptr)) + 0x35916b8);
        UParticleSystemComponent::StaticClass = Unreal::UObjectGlobals::StaticFindObject<UClass*>(
            nullptr, nullptr, to_wstring("/Script/Engine.ParticleSystemComponent"));
        AREDPawnEffect::StaticClass = Unreal::UObjectGlobals::StaticFindObject<UClass*>(
            nullptr, nullptr, to_wstring("/Script/RED.REDPawnEffect"));

        Unreal::Hook::RegisterInitGameStatePreCallback([](Unreal::AGameModeBase* Context)
        {
            auto& Ctx = GetRollbackContext();
            Ctx.ResetForNewMatch();

            const auto Class = Unreal::UObjectGlobals::StaticFindObject<UClass*>(
                nullptr, nullptr, to_wstring("/Script/RED.REDGameMode_Battle"));

            if (Context->IsA(Class))
            {
                AREDGameState_Battle::instance = static_cast<AREDGameState_Battle*>(UObjectGlobals::FindFirstOf(
                    FName(STR("REDGameState_Battle"))));
                Ctx.GameState = AREDGameState_Battle::GetGameState();
                Ctx.bCanRollback = true;

                if (!ScratchState)
                    ScratchState = static_cast<RollbackData*>(Unreal::FMemory::Malloc(sizeof(RollbackData)));
                Unreal::FMemory::Memzero(ScratchState, sizeof(RollbackData));

                CreateGekkoStressSession();
            }
            else
            {
                gekko_destroy(&Session);
                Session = nullptr;
                Ctx.bCanRollback = false;

                if (ScratchState)
                {
                    Unreal::FMemory::Free(ScratchState);
                    ScratchState = nullptr;
                }
            }
        });

        const SignatureContainer obj_cbase_ctor{
            {
                {
                    "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8D 05 ? ? ? ? C7 41"
                }
            },
            [&](const SignatureContainer& self)
            {
                OBJ_CBase_Ctor_Detour = safetyhook::create_inline(self.get_match_address(), &OBJ_CBase_Ctor_Hook);
                return false;
            },
            [](SignatureContainer& self)
            {
            },
        };

        const SignatureContainer object_constructor{
            {
                {
                    "48 89 5C 24 ? 56 48 83 EC ? 33 F6 C7 41"
                }
            },
            [&](const SignatureContainer& self)
            {
                ObjectConstructor_Detour = safetyhook::create_inline(self.get_match_address(), &ObjectConstructor_Hook);
                return false;
            },
            [](SignatureContainer& self)
            {
            },
        };

        const SignatureContainer after_frame_step{
            {
                {
                    "40 53 48 83 EC 20 81 A1 ? ? ? ? 7F FF FF FF"
                }
            },
            [&](const SignatureContainer& self)
            {
                AfterFrameStep_Detour = safetyhook::create_inline(self.get_match_address(), &AfterFrameStep_Hook);
                return false;
            },
            [](SignatureContainer& self)
            {
            },
        };

        const SignatureContainer update_battle{
            {
                {
                    "48 83 EC ? 48 89 5C 24 ? 48 89 6C 24 ? 4C 89 74 24"
                }
            },
            [&](const SignatureContainer& self)
            {
                UpdateBattle_Detour = safetyhook::create_inline(self.get_match_address(), &UpdateBattle_Hook);
                return false;
            },
            [](SignatureContainer& self)
            {
            },
        };

        const SignatureContainer delete_link_psc{
            {
                {
                    "40 53 48 83 EC 20 48 8B 81 ? ? ? ? 48 8B D9 48 85 C0 74 ? C6 80 ? ? ? ? 00"
                }
            },
            [&](const SignatureContainer& self)
            {
                DeleteLinkPSC_Detour = safetyhook::create_inline(self.get_match_address(), &DeleteLinkPSC_Hook);
                return false;
            },
            [](SignatureContainer& self)
            {
            },
        };

        const SignatureContainer ue_particle_get{
            {
                {
                    "48 89 5C 24 ? 55 57 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 4C 89 B4 24"
                }
            },
            [&](const SignatureContainer& self)
            {
                UEParticleGet_Detour = safetyhook::create_inline(self.get_match_address(), &UEParticleGet_Hook);
                return false;
            },
            [](SignatureContainer& self)
            {
            },
        };

        const SignatureContainer create_particle_arg{
            {
                {
                    "48 89 5C 24 ? 55 56 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? F3 0F 10 0D ? ? ? ? 48 8B DA 33 D2 C7 45 ? ? ? ? ? 0F 28 C1 48 89 55 ? 0F 14 C1 8B C2 8B 45 ? 0F 57 C9 89 45 ? 41 8B F8"
                }
            },
            [&](const SignatureContainer& self)
            {
                CreateParticleArg_Detour = safetyhook::create_inline(self.get_match_address(), &CreateParticleArg_Hook);
                return false;
            },
            [](SignatureContainer& self)
            {
            },
        };

        const SignatureContainer create_particle_from_particle_data_asset{
            {
                {
                    "E8 ? ? ? ? 48 8B D8 48 85 C0 0F 84 ? ? ? ? 48 8B 4F ? 4C 89 74 24"
                }
            },
            [&](const SignatureContainer& self)
            {
                CreateParticleFromParticleDataAsset = safetyhook::create_mid(
                    self.get_match_address(), [](safetyhook::Context& ctx)
                    {
                        auto psTemplate = reinterpret_cast<UParticleSystem*>(ctx.rdx);
                        if (!psTemplate)
                            return;

                        const auto& Ctx = GetRollbackContext();

                        psTemplate->WarmupTime = Ctx.bIsFastForward
                            ? static_cast<float>(Ctx.RollbackFramesRemaining) / 60.0f
                            : 0.0f;

                        psTemplate->WarmupTickRate = 0.016666668f;
                    });
                return false;
            },
            [](SignatureContainer& self)
            {
            },
        };

        const SignatureContainer release_resource{
            {
                {
                    "44 88 B8 ? ? ? ? B2 01"
                }
            },
            [&](const SignatureContainer& self)
            {
                ReleaseResource = safetyhook::create_mid(
                    self.get_match_address(), [](safetyhook::Context& ctx)
                    {
                        GetRollbackContext().GetObjExt(reinterpret_cast<OBJ_CBase*>(ctx.rbx)).m_LinkParticleActiveFrame = -1;
                    });
                return false;
            },
            [](SignatureContainer& self)
            {
            },
        };

        const SignatureContainer set_component_tick_enabled{
            {
                {
                    "48 89 5C 24 ? 57 48 83 EC ? F6 41 ? ? 0F B6 FA 48 8B D9 74 ? BA ? ? ? ? E8 ? ? ? ? 84 C0 75 ? 48 8D 4B ? 40 0F B6 D7 E8 ? ? ? ? 48 8B 5C 24 ? 48 83 C4 ? 5F C3 CC CC CC CC 48 89 5C 24"
                }
            },
            [&](const SignatureContainer& self)
            {
                SetComponentTickEnabled = reinterpret_cast<SetComponentTickEnabled_Func>(self.get_match_address());
                return false;
            },
            [](SignatureContainer& self)
            {
            },
        };

        const SignatureContainer deactivate_system{
            {
                {
                    "40 53 41 54 41 55 48 83 EC ? 4C 8B A9"
                }
            },
            [&](const SignatureContainer& self)
            {
                DeactivateSystem = reinterpret_cast<DeactivateSystem_Func>(self.get_match_address());
                return false;
            },
            [](SignatureContainer& self)
            {
            },
        };

        const SignatureContainer kill_particles_forced{
            {
                {
                    "48 89 5C 24 ? 56 48 83 EC ? 48 83 B9 ? ? ? ? ? 48 8B F1"
                }
            },
            [&](const SignatureContainer& self)
            {
                KillParticlesForced = reinterpret_cast<KillParticlesForced_Func>(self.get_match_address());
                return false;
            },
            [](SignatureContainer& self)
            {
            },
        };

        const SignatureContainer link_particle_arg{
            {
                {
                    "48 89 5C 24 ? 55 56 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? F3 0F 10 0D ? ? ? ? 48 8B DA 33 D2 C7 45 ? ? ? ? ? 0F 28 C1 48 89 55 ? 0F 14 C1 8B C2 8B 45 ? 0F 57 C9 89 45 ? 41 8B F0"
                }
            },
            [&](const SignatureContainer& self)
            {
                LinkParticleArg_Detour = safetyhook::create_inline(self.get_match_address(), &LinkParticleArg_Hook);
                return false;
            },
            [](SignatureContainer& self)
            {
            },
        };

        const SignatureContainer control_battle_object_key_renewal{
            {
                {
                    "E8 ? ? ? ? 48 8B 45 ? 66 44 89 7C 44"
                }
            },
            [&](const SignatureContainer& self)
            {
                ControlBattleObject_KeyRenewal_Detour = safetyhook::create_mid(
                    self.get_match_address(), [](SafetyHookContext& ctx)
                    {
                        if (GetRollbackContext().bRollbackTest)
                        {
                            ctx.rdx = ctx.r12 == 0 ? GetRollbackContext().Inputs[0] : GetRollbackContext().Inputs[1];
                        }
                    });
                return false;
            },
            [](SignatureContainer& self)
            {
            },
        };

        const SignatureContainer get_battle_key_from_pad_id{
            {
                {
                    "83 FA 06 75 ? 48 8B 81"
                }
            },
            [&](const SignatureContainer& self)
            {
                GetBattleKeyFromPadID = reinterpret_cast<GetBattleKeyFromPadID_Func>(self.get_match_address());
                return false;
            },
            [](SignatureContainer& self)
            {
            },
        };

        const SignatureContainer side_2_pad{
            {
                {
                    "40 53 56 57 48 83 EC 20 48 8B 05"
                }
            },
            [&](const SignatureContainer& self)
            {
                Side2Pad = reinterpret_cast<Side2Pad_Func>(self.get_match_address());
                return false;
            },
            [](SignatureContainer& self)
            {
            },
        };

        const SignatureContainer gkf_2_rec_flag{
            {
                {
                    "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 54 41 55 41 56 41 57 48 83 EC 20 41 BD 08 00 00 00"
                }
            },
            [&](const SignatureContainer& self)
            {
                GKF2RecFlag = reinterpret_cast<GKF2RecFlag_Func>(self.get_match_address());
                return false;
            },
            [](SignatureContainer& self)
            {
            },
        };

        const SignatureContainer link_pawn_cache{
            {
                {
                    "33 D2 48 89 94 24 ? ? ? ? 48 85 FF"
                }
            },
            [&](const SignatureContainer& self)
            {
                LinkPawn_Cache_Detour = safetyhook::create_mid(self.get_match_address(), [](SafetyHookContext& ctx)
                {
                    if (Rollback_ProcessCachedPawn((OBJ_CBase*)ctx.rbx, (CXXBYTE<32>*)ctx.rdi, false, false))
                    {
                        ctx.rip = LinkPawn_Cache_Detour.target_address() + 0xB1;
                    }
                });
                return false;
            },
            [](SignatureContainer& self)
            {
            },
        };

        const SignatureContainer link_pawn_on_link{
            {
                {
                    "33 D2 48 89 94 24 ? ? ? ? 48 85 FF"
                }
            },
            [&](const SignatureContainer& self)
            {
                LinkPawn_OnLink_Detour = safetyhook::create_mid(self.get_match_address(), [](SafetyHookContext& ctx)
                {
                    auto obj = (OBJ_CBase*)ctx.rbx;
                    auto& Ext = GetRollbackContext().GetObjExt(obj);

                    if (ctx.rdx)
                        strncpy(Ext.m_RollbackData.LinkPawnName.m_Buf, (const char*)ctx.rdx, 0x20);
                    Ext.m_RollbackData.LinkCommonPawn = false;
                    Ext.m_RollbackData.LinkCommonPawnEffect = false;
                    Ext.m_RollbackData.LinkCommonPawnSelf = false;
                    Ext.m_RollbackData.LinkPawnStartFrame = GetRollbackContext().GameFrame;
                    Ext.m_RollbackData.LinkPawnActName = obj->m_CurActionName;
                });
                return false;
            },
            [](SignatureContainer& self)
            {
            },
        };

        const SignatureContainer link_common_pawn_effect{
            {
                {
                    "48 89 5C 24 ? 55 56 57 41 56 41 57 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 ? ? ? ? 4C 8B B4 24"
                }
            },
            [&](const SignatureContainer& self)
            {
                LinkCommonPawnEffect_Detour = safetyhook::create_inline(self.get_match_address(), LinkCommonPawnEffect);
                return false;
            },
            [](SignatureContainer& self)
            {
            },
        };

        const SignatureContainer link_common_pawn_effect_on_link{
            {
                {
                    "81 8B ? ? ? ? 00 00 10 00 48 8B 8C 24"
                }
            },
            [&](const SignatureContainer& self)
            {
                LinkCommonPawnEffect_OnLink_Detour = safetyhook::create_mid(
                    self.get_match_address(), [](SafetyHookContext& ctx)
                    {
                        auto obj = (OBJ_CBase*)ctx.rbx;
                        auto& Ext = GetRollbackContext().GetObjExt(obj);

                        if (ctx.rdx)
                            strncpy(Ext.m_RollbackData.LinkPawnName.m_Buf, (const char*)ctx.rdx, 0x20);
                        Ext.m_RollbackData.LinkCommonPawn = false;
                        Ext.m_RollbackData.LinkCommonPawnEffect = true;
                        Ext.m_RollbackData.LinkCommonPawnSelf = false;
                        Ext.m_RollbackData.LinkPawnStartFrame = GetRollbackContext().GameFrame;
                        Ext.m_RollbackData.LinkPawnActName = obj->m_CurActionName;
                    });
                return false;
            },
            [](SignatureContainer& self)
            {
            },
        };

        const SignatureContainer link_common_pawn_detour{
            {
                {
                    "C6 83 ? ? ? ? ? 48 8B 5C 24 ? 48 83 C4 ? 5F C3 CC CC CC CC CC CC CC CC CC CC 48 89 5C 24 ? 48 89 74 24"
                }
            },
            [&](const SignatureContainer& self)
            {
                LinkCommonPawn_Detour = safetyhook::create_mid(self.get_match_address(), [](SafetyHookContext& ctx)
                {
                    auto obj = (OBJ_CBase*)ctx.rdi;
                    auto& Ext = GetRollbackContext().GetObjExt(obj);

                    strncpy(Ext.m_RollbackData.LinkPawnName.m_Buf, "", 0x20);
                    Ext.m_RollbackData.LinkCommonPawn = true;
                    Ext.m_RollbackData.LinkCommonPawnEffect = false;
                    Ext.m_RollbackData.LinkCommonPawnSelf = false;
                    Ext.m_RollbackData.LinkPawnStartFrame = GetRollbackContext().GameFrame;
                    Ext.m_RollbackData.LinkPawnActName = obj->m_CurActionName;
                });
                return false;
            },
            [](SignatureContainer& self)
            {
            },
        };

        const SignatureContainer link_pawn_team_member{
            {
                {
                    "48 8B 9F ? ? ? ? 48 85 DB 0F 84 ? ? ? ? E8 ? ? ? ? 48 8B 53 ? 4C 8D 80 ? ? ? ? 49 63 40 ? 3B 82 ? ? ? ? 7F"
                }
            },
            [&](const SignatureContainer& self)
            {
                LinkPawnTeamMember_Detour = safetyhook::create_mid(self.get_match_address(), [](SafetyHookContext& ctx)
                {
                    auto obj = (OBJ_CBase*)ctx.rdi;
                    auto& Ext = GetRollbackContext().GetObjExt(obj);

                    if (ctx.rdx)
                        strncpy(Ext.m_RollbackData.LinkPawnName.m_Buf, (const char*)ctx.rdx, 0x20);
                    Ext.m_RollbackData.LinkCommonPawn = true;
                    Ext.m_RollbackData.LinkCommonPawnEffect = false;
                    Ext.m_RollbackData.LinkCommonPawnSelf = false;
                    Ext.m_RollbackData.LinkPawnStartFrame = GetRollbackContext().GameFrame;
                    Ext.m_RollbackData.LinkPawnActName = obj->m_CurActionName;
                });
                return false;
            },
            [](SignatureContainer& self)
            {
            },
        };

        const SignatureContainer clear_link_model{
            {
                {
                    "48 89 5C 24 ? 57 48 83 EC ? 48 8B 99 ? ? ? ? 48 8B F9 48 85 DB 0F 84 ? ? ? ? E8 ? ? ? ? 48 8B 53 ? 4C 8D 80 ? ? ? ? 49 63 40 ? 3B 82 ? ? ? ? 7F ? 48 8B C8 48 8B 82 ? ? ? ? 4C 39 04 C8 75 ? 48 C7 83"
                }
            },
            [&](const SignatureContainer& self)
            {
                ClearLinkModel = reinterpret_cast<ClearLinkModel_Func>(self.get_match_address());
                return false;
            },
            [](SignatureContainer& self)
            {
            },
        };

        const SignatureContainer set_actor_hidden_in_game{
            {
                {
                    "44 0F B6 81 ? ? ? ? 41 0F B6 C0 24 01 3A C2 74 ? 41 0F B6 C0 32 C2 24 01 41 32 C0 88 81 ? ? ? ? E9"
                }
            },
            [&](const SignatureContainer& self)
            {
                SetActorHiddenInGame = reinterpret_cast<SetActorHiddenInGame_Func>(self.get_match_address());
                return false;
            },
            [](SignatureContainer& self)
            {
            },
        };

        const SignatureContainer set_actor_tick_enabled{
            {
                {
                    "48 89 5C 24 ? 57 48 83 EC ? F6 41 ? ? 0F B6 FA 48 8B D9 74 ? BA ? ? ? ? E8 ? ? ? ? 84 C0 75 ? 48 8D 4B ? 40 0F B6 D7 E8 ? ? ? ? 48 8B 5C 24 ? 48 83 C4 ? 5F C3 CC CC CC CC F3 0F 11 49"
                }
            },
            [&](const SignatureContainer& self)
            {
                SetActorTickEnabled = reinterpret_cast<SetActorHiddenInGame_Func>(self.get_match_address());
                return false;
            },
            [](SignatureContainer& self)
            {
            },
        };

        const SignatureContainer return_to_effect_manager{
            {
                {
                    "48 89 5C 24 ? 57 48 83 EC ? 48 8B 99 ? ? ? ? 48 8B F9 48 85 DB 74 ? E8 ? ? ? ? 48 8B 53 ? 4C 8D 80 ? ? ? ? 49 63 40 ? 3B 82 ? ? ? ? 7F ? 48 8B C8 48 8B 82 ? ? ? ? 4C 39 04 C8 74 ? 33 DB"
                }
            },
            [&](const SignatureContainer& self)
            {
                ReturnToEffectManager = reinterpret_cast<ReturnToEffectManager_Func>(self.get_match_address());
                return false;
            },
            [](SignatureContainer& self)
            {
            },
        };

        const SignatureContainer link_common_pawn{
            {
                {
                    "48 89 5C 24 ? 57 48 83 EC ? 48 8B F9 E8 ? ? ? ? 48 8B 98"
                }
            },
            [&](const SignatureContainer& self)
            {
                LinkCommonPawn = reinterpret_cast<LinkCommonPawn_Func>(self.get_match_address());
                return false;
            },
            [](SignatureContainer& self)
            {
            },
        };

        const SignatureContainer switch_mesh_set{
            {
                {
                    "48 89 5C 24 ? 48 89 54 24 ? 57 48 83 EC 20 48 8B F9 48 8D 15"
                }
            },
            [&](const SignatureContainer& self)
            {
                SwitchMeshSet = reinterpret_cast<SwitchMeshSet_Func>(self.get_match_address());
                return false;
            },
            [](SignatureContainer& self)
            {
            },
        };

        const SignatureContainer setup_materials{
            {
                {
                    "48 89 54 24 ? 55 57 48 8B EC"
                }
            },
            [&](const SignatureContainer& self)
            {
                SetupMaterials = reinterpret_cast<SetupMaterials_Func>(self.get_match_address());
                return false;
            },
            [](SignatureContainer& self)
            {
            },
        };

        std::vector<SignatureContainer> signature_containers;
        signature_containers.push_back(obj_cbase_ctor);
        signature_containers.push_back(object_constructor);
        signature_containers.push_back(update_battle);
        signature_containers.push_back(after_frame_step);
        signature_containers.push_back(delete_link_psc);
        signature_containers.push_back(ue_particle_get);
        signature_containers.push_back(create_particle_arg);
        signature_containers.push_back(create_particle_from_particle_data_asset);
        signature_containers.push_back(release_resource);
        signature_containers.push_back(set_component_tick_enabled);
        signature_containers.push_back(deactivate_system);
        signature_containers.push_back(kill_particles_forced);
        signature_containers.push_back(link_particle_arg);
        signature_containers.push_back(control_battle_object_key_renewal);
        signature_containers.push_back(get_battle_key_from_pad_id);
        signature_containers.push_back(side_2_pad);
        signature_containers.push_back(gkf_2_rec_flag);
        signature_containers.push_back(link_pawn_cache);
        signature_containers.push_back(link_pawn_on_link);
        signature_containers.push_back(link_common_pawn_effect);
        signature_containers.push_back(link_common_pawn_effect_on_link);
        signature_containers.push_back(link_common_pawn_detour);
        signature_containers.push_back(link_pawn_team_member);
        signature_containers.push_back(clear_link_model);
        signature_containers.push_back(set_actor_hidden_in_game);
        signature_containers.push_back(set_actor_tick_enabled);
        signature_containers.push_back(return_to_effect_manager);
        signature_containers.push_back(link_common_pawn);
        signature_containers.push_back(switch_mesh_set);
        signature_containers.push_back(setup_materials);

        SinglePassScanner::SignatureContainerMap signature_containers_map;
        signature_containers_map.emplace(ScanTarget::MainExe, signature_containers);

        SinglePassScanner::start_scan(signature_containers_map);

        UE4SSProgram* Program = &UE4SSProgram::get_program();
        Program->register_keydown_event(Input::Key::F2, []
        {
            GetRollbackContext().bRollbackTest = !GetRollbackContext().bRollbackTest;
        });
    }
};

#define GBVS_ROLLBACK_API __declspec(dllexport)

extern "C" {
GBVS_ROLLBACK_API CppUserModBase* start_mod()
{
    return new GBVSRollback();
}

GBVS_ROLLBACK_API void uninstall_mod(CppUserModBase* mod)
{
    delete mod;
}
}
