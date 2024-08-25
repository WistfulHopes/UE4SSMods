#include "safetyhook.hpp"
#include <BattleState.h>
#include <Particles.h>
#include <Unreal.h>
#include <UE4SSProgram.hpp>
#include <UnrealDef.hpp>
#include <Mod/CppUserModBase.hpp>
#include <Unreal/AGameModeBase.hpp>

#include "SigScanner/SinglePassSigScanner.hpp"

AREDGameState_Battle* GameState;
bool bCanRollback = false;
bool bRollbackTest = false;
RollbackData Data = RollbackData();
Particle_RollbackData rollbackData;
std::unordered_map<OBJ_CBase*, Obj_RollbackData> objRollbackData;

SafetyHookInline UpdateBattle_Detour{};
SafetyHookInline UEParticleGet_Detour{};
SafetyHookInline CreateParticleArg_Detour{};
SafetyHookInline LinkParticleEx_Detour{};
SafetyHookInline LinkParticleArg_Detour{};

// Particles

UParticleSystemComponent* pscToCache;

void UEParticleGet_Hook(void* pThis, UParticleSystemComponent* UEParticle, void* arg)
{
    pscToCache = UEParticle;
    UEParticleGet_Detour.call(pThis, UEParticle, arg);
}

typedef void (*SetComponentTickEnabled_Func)(Unreal::UActorComponent*, bool bEnabled);
SetComponentTickEnabled_Func SetComponentTickEnabled;

typedef void (*DeactivateSystem_Func)(UParticleSystemComponent*);
DeactivateSystem_Func DeactivateSystem;

typedef void (*KillParticlesForced_Func)(UParticleSystemComponent*);
KillParticlesForced_Func KillParticlesForced;

void CreateParticleArg_Hook(OBJ_CBase* pThis, void* arg, CXXBYTE<32>* name, int pos)
{
    CreateParticleArg_Detour.call<void>(pThis, arg, name, pos);
    FPSCCacheKey UnlinkedPSCKey = MakeUnlinkedPSCKey(GameState, pThis, name);
    rollbackData.unlinkedPscCache.insert({UnlinkedPSCKey, pscToCache});
}

void LinkParticleEx_Hook(OBJ_CBase* pThis, void* arg, const CXXBYTE<32>* name, int objType)
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

    if (Rollback_ProcessCachedPSC(pThis, name, objType, false)) return;
    LinkParticleEx_Detour.call<void>(pThis, arg, name, objType);
    Rollback_OnLinkParticle(pThis, name, objType, false);
}

void LinkParticleArg_Hook(OBJ_CBase* pThis, const CXXBYTE<32>* name, int objType)
{
    if (Rollback_ProcessCachedPSC(pThis, name, objType, true))
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
        return;
    }
    LinkParticleArg_Detour.call<void>(pThis, name, objType);
    Rollback_OnLinkParticle(pThis, name, objType, true);
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
}

void PreRollback()
{
    for (auto i : rollbackData.pscCache)
    {
        if (i.second != nullptr && i.first.obj->m_ActiveState != 1)
        {
            SetComponentTickEnabled(i.second, true);
            DeactivateSystem(i.second);
            KillParticlesForced(i.second);
        }
    }
    rollbackData.pscCache.clear();
    for (int i = 0; i < GameState->BattleObjectManager->m_ActiveObjectCount; i++)
    {
        OBJ_CBase* obj = GameState->BattleObjectManager->m_SortedObjPtrVector[i];
        if (!obj)
            break;
        AddLinkPSCToCache(obj);
        obj->m_pLinkPSC = nullptr;
    }
}

void PostRollback()
{
    const auto Class = Unreal::UObjectGlobals::StaticFindObject<UClass*>(
    nullptr, nullptr, to_wstring("/Script/Engine.ParticleSystemComponent"));

    auto components = GameState->PSCManager->GetComponentsByClass(Class);

    for (auto component : components)
    {
        auto psc = reinterpret_cast<UParticleSystemComponent*>(component);

        if (psc->LinkObjPtr)
            psc->LinkObjPtr->m_pLinkPSC = psc;
    }
}

// Battle

void UpdateBattle_Hook(AREDGameState_Battle* pThis, float DeltaTime, bool bUpdateDraw)
{
    if (bRollbackTest)
    {
        PreRollback();
        Data.LoadState(pThis);
        PostRollback();
        UpdateBattle_Detour.call(pThis, DeltaTime, bUpdateDraw);
        Data.SaveState(pThis);
        UpdateBattle_Detour.call(pThis, DeltaTime, bUpdateDraw);
    }
    else
        UpdateBattle_Detour.call(pThis, DeltaTime, bUpdateDraw);
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
        Unreal::Hook::RegisterInitGameStatePreCallback([](Unreal::AGameModeBase* Context)
        {
            bRollbackTest = false;
            rollbackData = Particle_RollbackData();
            objRollbackData.clear();
            Data = RollbackData();
            const auto Class = Unreal::UObjectGlobals::StaticFindObject<UClass*>(
                nullptr, nullptr, to_wstring("/Script/RED.REDGameMode_Battle"));

            if (Context->IsA(Class))
            {
                AREDGameState_Battle::instance = static_cast<AREDGameState_Battle*>(UObjectGlobals::FindFirstOf(
                    FName(STR("REDGameState_Battle"))));
                GameState = AREDGameState_Battle::GetGameState();
                bCanRollback = true;
            }
            else
                bCanRollback = false;
        });

        const SignatureContainer update_battle{
            {
                {
                    "48 83 EC ? 48 89 5C 24 ? 48 89 6C 24 ? 4C 89 74 24"
                }
            },
            [&](const SignatureContainer& self)
            {
                UpdateBattle_Detour = safetyhook::create_inline(self.get_match_address(), &UpdateBattle_Hook);
                return true;
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
                return true;
            },
            [](SignatureContainer& self)
            {
            },
        };

        const SignatureContainer create_particle_arg{
            {
                {
                    "48 89 5C 24 ? 57 48 83 EC ? 48 8B FA 48 8B D9 48 85 D2 75 ? 33 C0"
                }
            },
            [&](const SignatureContainer& self)
            {
                CreateParticleArg_Detour = safetyhook::create_inline(self.get_match_address(), &CreateParticleArg_Hook);
                return true;
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
                return true;
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
                return true;
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
                return true;
            },
            [](SignatureContainer& self)
            {
            },
        };
        
        const SignatureContainer link_particle_ex{
            {
                {
                    "48 85 D2 0F 84 ? ? ? ? 55 41 56 41 57"
                }
            },
            [&](const SignatureContainer& self)
            {
                LinkParticleEx_Detour = safetyhook::create_inline(self.get_match_address(), &LinkParticleEx_Hook);
                return true;
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
                return true;
            },
            [](SignatureContainer& self)
            {
            },
        };
        
        std::vector<SignatureContainer> signature_containers;
        signature_containers.push_back(update_battle);
        signature_containers.push_back(ue_particle_get);
        signature_containers.push_back(create_particle_arg);
        signature_containers.push_back(set_component_tick_enabled);
        signature_containers.push_back(deactivate_system);
        signature_containers.push_back(kill_particles_forced);
        signature_containers.push_back(link_particle_ex);
        signature_containers.push_back(link_particle_arg);
      
        SinglePassScanner::SignatureContainerMap signature_containers_map;
        signature_containers_map.emplace(ScanTarget::MainExe, signature_containers);
            
        SinglePassScanner::start_scan(signature_containers_map);
        
        UE4SSProgram* Program = &UE4SSProgram::get_program();
        Program->register_keydown_event(Input::Key::F2, []
        {
            bRollbackTest = !bRollbackTest;
        });
        Program->register_keydown_event(Input::Key::F3, []
        {
            if (bCanRollback)
                Data.SaveState(GameState);
        });
        Program->register_keydown_event(Input::Key::F4, []
        {
            if (bCanRollback)
            {
                PreRollback();
                Data.LoadState(GameState);
                PostRollback();
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
