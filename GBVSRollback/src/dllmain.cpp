#include "safetyhook.hpp"
#include <BattleState.hpp>
#include <gekkonet.h>
#include <Particles.hpp>
#include <Unreal.hpp>
#include <UE4SSProgram.hpp>
#include <UnrealDef.hpp>
#include <Mod/CppUserModBase.hpp>
#include <polyhook2/Exceptions/AVehHook.hpp>
#include <Unreal/AGameModeBase.hpp>

#include "SigScanner/SinglePassSigScanner.hpp"

AREDGameState_Battle* GameState{};
bool bCanRollback = false;
bool bRollbackTest = false;
Particle_RollbackData rollbackData{};
std::unordered_map<OBJ_CBase*, OBJ_CBaseExt> objData;
bool bIsSave = false;
bool bIsRollback = false;
GekkoSession* session = nullptr;
void* g_SystemManager;
int32_t g_Inputs[2];

SafetyHookInline DeleteLinkPSC_Detour{};
SafetyHookInline UEParticleGet_Detour{};
SafetyHookInline CreateParticleArg_Detour{};
SafetyHookInline LinkParticleArg_Detour{};
SafetyHookInline OBJ_CBase_Ctor_Detour{};
SafetyHookInline ObjectConstructor_Detour{};
SafetyHookInline AfterFrameStep_Detour{};
SafetyHookInline UpdateBattle_Detour{};

SafetyHookMid CreateParticleFromParticleDataAsset{};
SafetyHookMid ReleaseResource{};
SafetyHookMid ControlBattleObject_KeyRenewal_Detour{};

// Particles

UParticleSystemComponent* pscToCache;

void DeleteLinkPSC_Hook(OBJ_CBase* pThis)
{
    DeleteLinkPSC_Detour.call(pThis);
    objData[pThis].m_LinkParticleActiveFrame = -1;
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
    LinkParticleArg_Detour.call<void>(pThis, name, objType);
    Rollback_OnLinkParticle(pThis, name);
    if (objData[pThis].m_LinkParticleActiveFrame == -1)
    {
        objData[pThis].m_LinkParticleActiveFrame = 0;
    }
}

void Obj_OnPreBackup(OBJ_CBase* pThis)
{
    if (pThis->m_pLinkPSC)
    {
        objData[pThis].m_RollbackData.bLinkParticleSet = true;
        objData[pThis].m_RollbackData.PointLightId = pThis->m_pLinkPSC->PointLightId;
    }
    else
    {
        objData[pThis].m_RollbackData.bLinkParticleSet = false;
    }
}

void PreBackup()
{
    for (int i = 0; i < GameState->BattleObjectManager->m_ActiveObjectCount; i++)
    {
        Obj_OnPreBackup(GameState->BattleObjectManager->m_SortedObjPtrVector[i]);
    }
}

void ClearPSCCache()
{
    for (const auto val : rollbackData.pscCache | std::views::values)
    {
        if (!val) continue;
        val->LinkObjPtr = nullptr;
        val->bAutoDestroy = true;
        SetComponentTickEnabled(val, true);
        DeactivateSystem(val);
        KillParticlesForced(val);
    }
    rollbackData.pscCache.clear();
}

void PreRollback()
{
    ClearPSCCache();
    for (int i = 0; i < GameState->BattleObjectManager->m_ActiveObjectCount; i++)
    {
        OBJ_CBase* obj = GameState->BattleObjectManager->m_SortedObjPtrVector[i];
        if (!obj)
            break;
        if (!obj->m_pLinkPSC) continue;
        if (objData[obj].m_LinkParticleActiveFrame < 0 && objData[obj].m_LinkParticleActiveFrame >= 0)
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
}

void ClearUnusedUnlinkedPSC()
{
    for (auto& psc : rollbackData.unlinkedPscCache)
    {
        if (!rollbackData.unlinkedPscCacheUsed.contains(psc.first) && psc.second)
        {
            psc.second->bAutoDestroy = true;
            SetComponentTickEnabled(psc.second, true);
            DeactivateSystem(psc.second);
            KillParticlesForced(psc.second);
        }
    }

    rollbackData.unlinkedPscCache.clear();
}

void Obj_OnPostRollback(OBJ_CBase* pThis)
{
    if (!pThis->m_IsPlayerObj)
    {
        if (objData[pThis].m_RollbackData.bLinkParticleSet)
        {
            if (pThis->m_pLinkPSC)
            {
                pThis->m_pLinkPSC->PointLightId = objData[pThis].m_RollbackData.PointLightId;
            }
        }
    }
}

void ClearOldUnlinkedPSC_PostRollback()
{
    rollbackData.unlinkedPscCacheUsed.clear();

    std::vector<FPSCCacheKey> deleteTemp;

    for (auto& psc : rollbackData.unlinkedPscCache)
    {
        if (psc.first.GameFrame < GameFrame || !GameState->PSCManager->GetComponentsByClass(
            UParticleSystemComponent::StaticClass).Contains(psc.second))
        {
            deleteTemp.push_back(psc.first);
        }
    }

    for (auto& key : deleteTemp)
    {
        rollbackData.unlinkedPscCache.erase(key);
    }
}

void PostRollback()
{
    ClearOldUnlinkedPSC_PostRollback();

    for (auto components = GameState->PSCManager->GetComponentsByClass(UParticleSystemComponent::StaticClass); const
         auto component : components)
    {
        if (auto psc = reinterpret_cast<UParticleSystemComponent*>(component); psc->LinkObjPtr)
            psc->LinkObjPtr->m_pLinkPSC = psc;
    }

    for (int i = 0; i < GameState->BattleObjectManager->m_ActiveObjectCount; i++)
    {
        Obj_OnPostRollback(GameState->BattleObjectManager->m_SortedObjPtrVector[i]);
    }
}

// Battle

void OBJ_CBase_Ctor_Hook(OBJ_CBase* pThis)
{
    OBJ_CBase_Ctor_Detour.call(pThis);
    OBJ_CBaseExt data{};
    objData.insert({pThis, data});
}

void ObjectConstructor_Hook(OBJ_CBase* pThis)
{
    ObjectConstructor_Detour.call(pThis);
    objData[pThis] = OBJ_CBaseExt();
}

bool AfterFrameStep_Hook(OBJ_CBase* pThis)
{
    if (pThis->m_pLinkPSC)
    {
        objData[pThis].m_LinkParticleActiveFrame++;
    }
    return AfterFrameStep_Detour.call<bool>(pThis);
}

void SaveForRollback(RollbackData* Data)
{
    bIsSave = true;
    bIsRollback = false;
    PreBackup();
    Data->SaveState(GameState);
    bIsSave = false;
}

void LoadForRollback(RollbackData* Data)
{
    bIsSave = false;
    bIsRollback = true;
    PreRollback();
    Data->LoadState(GameState);
    PostRollback();
    bIsRollback = false;
}

void UpdateBattle_Hook(AREDGameState_Battle* pThis, float DeltaTime, bool bUpdateDraw)
{
    if (Random == nullptr)
    {
        Random = *reinterpret_cast<AA_CRandMT**>(*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(
            GetModuleHandle(nullptr)) + 0x35916b8) + 0x140);
    }
    if (bRollbackTest)
    {
        for (int i = 0; i < 2; i++)
        {
            auto GKFFlag = *(int32_t*)(*(uintptr_t*)(*(uintptr_t*)g_SystemManager + 0x1F8 + Side2Pad(i, 0) * 8) + 0x78);
            auto RecFlag = GKF2RecFlag(Side2Pad(i, 0), GKFFlag, false);

            gekko_add_local_input(session, i, &RecFlag);
        }

        int evtCountSession = 0;
        GekkoSessionEvent** events = gekko_session_events(session, &evtCountSession);
        for (int i = 0; i < evtCountSession; i++)
        {
            GekkoSessionEvent* event = events[i];
            switch (event->type)
            {
            case DesyncDetected:
                auto desync = event->data.desynced;
                Output::send(std::format(
                        STR("DESYNC!!! f:{}, rh:{}, lc:{}, rc:{}\n"), desync.frame, desync.remote_handle,
                        desync.local_checksum, desync.remote_checksum)
                );
                assert(false);
                break;
            }
        }

        int evtCountGame = 0;
        GekkoGameEvent** updates = gekko_update_session(session, &evtCountGame);
        for (int i = 0; i < evtCountGame; i++)
        {
            GekkoGameEvent* event = updates[i];
            switch (event->type)
            {
            case SaveEvent:
                {
                    *event->data.save.state_len = sizeof(RollbackData);
                    auto Data = reinterpret_cast<RollbackData*>(event->data.save.state);
                    new (Data) RollbackData;
                    for (int j = 0; j < 416; j++)
                    {
                        auto Obj = GameState->BattleObjectManager->m_SortedObjPtrVector[j];
                        Data->StoredObjData.insert({Obj, OBJ_CBaseExt()});
                    }
                    SaveForRollback(Data);
                }
                break;

            case LoadEvent:
                LoadForRollback(reinterpret_cast<RollbackData*>(event->data.save.state));
                break;

            case AdvanceEvent:
                for (int j = 0; j < 2; j++)
                {
                    g_Inputs[j] = ((int32_t*)(event->data.adv.inputs))[j];
                }
                printf("\n");
                UpdateBattle_Detour.call(pThis, DeltaTime, bUpdateDraw);
                GameFrame++;
                break;
            }
        }
        ClearPSCCache();
        ClearUnusedUnlinkedPSC();
    }
    else
    {
        UpdateBattle_Detour.call(pThis, DeltaTime, bUpdateDraw);
        GameFrame++;
    }

    bIsRollback = false;
}

void InitGekko()
{
    session = nullptr;

    gekko_create_stress(&session);

    GekkoConfig config{};

    config.desync_detection = true;
    config.input_size = sizeof(int32_t);
    config.state_size = sizeof(RollbackData);
    config.max_spectators = 0;
    config.input_prediction_window = 10;
    config.num_players = 2;

    gekko_start(session, &config);
    gekko_add_actor(session, LocalPlayer, nullptr);
    gekko_set_local_delay(session, 0, 1);
    gekko_add_actor(session, LocalPlayer, nullptr);
    gekko_set_local_delay(session, 1, 1);
}

void EndGekko()
{
    gekko_destroy(&session);
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
        // Do not change this unless you want to target a UE4SS version
        // other than the one you're currently building with somehow.
        //ModIntendedSDKVersion = STR("2.6");
    }

    ~GBVSRollback() override = default;

    auto on_update() -> void override
    {
    }

    auto on_unreal_init() -> void override
    {
        g_SystemManager = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(GetModuleHandle(nullptr)) + 0x35916b8);
        UParticleSystemComponent::StaticClass = Unreal::UObjectGlobals::StaticFindObject<UClass*>(
            nullptr, nullptr, to_wstring("/Script/Engine.ParticleSystemComponent"));

        Unreal::Hook::RegisterInitGameStatePreCallback([](Unreal::AGameModeBase* Context)
        {
            EndGekko();
            rollbackData = Particle_RollbackData();
            objData.clear();
            const auto Class = Unreal::UObjectGlobals::StaticFindObject<UClass*>(
                nullptr, nullptr, to_wstring("/Script/RED.REDGameMode_Battle"));

            if (Context->IsA(Class))
            {
                AREDGameState_Battle::instance = static_cast<AREDGameState_Battle*>(UObjectGlobals::FindFirstOf(
                    FName(STR("REDGameState_Battle"))));
                GameState = AREDGameState_Battle::GetGameState();
                InitGekko();
                bCanRollback = true;
                GameFrame = 0;
                Random = nullptr;
            }
            else
            {
                bCanRollback = false;
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
                        if (bIsRollback)
                        {
                            if (psTemplate)
                            {
                                psTemplate->WarmupTime += 1.f / 60.f;
                            }
                        }
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
                        objData[reinterpret_cast<OBJ_CBase*>(ctx.rbx)].m_LinkParticleActiveFrame = -1;
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
                        if (bRollbackTest)
                        {
                            ctx.rdx = ctx.r12 == 0 ? g_Inputs[0] : g_Inputs[1];
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

        SinglePassScanner::SignatureContainerMap signature_containers_map;
        signature_containers_map.emplace(ScanTarget::MainExe, signature_containers);

        SinglePassScanner::start_scan(signature_containers_map);

        UE4SSProgram* Program = &UE4SSProgram::get_program();
        Program->register_keydown_event(Input::Key::F2, []
        {
            if (!bCanRollback)
            {
                bRollbackTest = !bRollbackTest;
            }
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
