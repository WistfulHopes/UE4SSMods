#define NOMINMAX

#include <polyhook2/Detour/x64Detour.hpp>
#include <polyhook2/Exceptions/AVehHook.hpp>
#include <BattleState.h>
#include <Particles.h>
#include <PatternFinder.h>
#include <Unreal.h>
#include <UE4SSProgram.hpp>
#include <UnrealDef.hpp>
#include <Mod/CppUserModBase.hpp>
#include <Unreal/AGameModeBase.hpp>

AREDGameState_Battle* GameState;
bool bCanRollback = false;
bool bRollbackTest = false;
RollbackData Data = RollbackData();
Particle_RollbackData rollbackData;
std::unordered_map<OBJ_CBase*, Obj_RollbackData> objRollbackData;

// Particles

UParticleSystemComponent* pscToCache;

void(*UEParticleGet_Orig)(void*, UParticleSystemComponent*, void*);
void UEParticleGet_Hook(void* pThis, UParticleSystemComponent* UEParticle, void* arg)
{
    pscToCache = UEParticle;
    UEParticleGet_Orig(pThis, UEParticle, arg);
}

typedef void(*DeactivateSystem_Func)(UParticleSystemComponent*);
DeactivateSystem_Func DeactivateSystem;

typedef void(*KillParticlesForced_Func)(UParticleSystemComponent*);
KillParticlesForced_Func KillParticlesForced;

void(*CreateParticleArg_Orig)(OBJ_CBase*, void*, CXXBYTE<32>*, int);
void CreateParticleArg_Hook(OBJ_CBase* pThis, void* arg, CXXBYTE<32>* name, int pos)
{
    CreateParticleArg_Orig(pThis, arg, name, pos);
    FPSCCacheKey UnlinkedPSCKey = MakeUnlinkedPSCKey(GameState, pThis, name);
    rollbackData.unlinkedPscCache.insert({ UnlinkedPSCKey, pscToCache });
}

void(*LinkParticleEx_Orig)(OBJ_CBase*, void*, const CXXBYTE<32>*, int);
void LinkParticleEx_Hook(OBJ_CBase* pThis, void* arg, const CXXBYTE<32>* name, int objType)
{
    if (Rollback_ProcessCachedPSC(pThis, name, 0x17, true))
    {
        pThis->m_CreateArg.m_CreateArg_SocketName.m_Buf[0] = 0;
        pThis->m_CreateArg.m_CreateArg_Angle = 0i64;
        pThis->m_CreateArg.m_CreateArg_OffsetPosX = 0i64;
        pThis->m_CreateArg.m_CreateArg_OffsetPosZ = 0;
        pThis->m_CreateArg.m_CreateArg_ScaleX = 1000;
        pThis->m_CreateArg.m_CreateArg_ScaleY = 1000;
        pThis->m_CreateArg.m_CreateArg_ScaleZ = 1000i64;
        pThis->m_CreateArg.m_CreateArg_MltColor = -1;
        pThis->m_CreateArg.m_CreateArg_TransPriority = 0i64;
        return;
    }
    LinkParticleEx_Orig(pThis, arg, name, objType);
    Rollback_OnLinkParticle(pThis, name, 0x17, true);
}

void PreRollback()
{
    for (auto i : rollbackData.pscCache)
    {
        if (i.second != nullptr && i.first.obj->m_ActiveState != 1)
        {
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
        reinterpret_cast<int*>(obj)[0xA7A8] = 0;
        reinterpret_cast<int*>(obj)[0xA7AC] = 0;
    }
}

void PostRollback() {

}

// Battle

void(*UpdateBattle_Orig)(AREDGameState_Battle*, float, bool);

void UpdateBattle_Hook(AREDGameState_Battle* pThis, float DeltaTime, bool bUpdateDraw)
{
    if (bRollbackTest)
    {
        PreRollback();
        Data.LoadState(pThis);
        UpdateBattle_Orig(pThis, DeltaTime, bUpdateDraw);
        Data.SaveState(pThis);
        UpdateBattle_Orig(pThis, DeltaTime, bUpdateDraw);
    }
    else
        UpdateBattle_Orig(pThis, DeltaTime, bUpdateDraw);
}

// Mod

class GBVSRollback : public CppUserModBase
{
public:
    PLH::x64Detour* UpdateBattle_Detour {};
    PLH::x64Detour* UEParticleGet_Detour {};
    PLH::x64Detour* CreateParticleArg_Detour {};
    PLH::x64Detour* LinkParticleEx_Detour {};

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

    auto on_update() -> void override {}

    auto on_unreal_init() -> void override
    {
        Unreal::Hook::RegisterInitGameStatePreCallback([] (Unreal::AGameModeBase* Context)
        {
            bRollbackTest = false;
            rollbackData = Particle_RollbackData();
            objRollbackData.clear();
            Data = RollbackData();
            const auto Class = Unreal::UObjectGlobals::StaticFindObject<UClass*>(
                nullptr, nullptr, to_wstring("/Script/RED.REDGameMode_Battle"));

            if (Context->IsA(Class))
            {
                AREDGameState_Battle::instance = static_cast<AREDGameState_Battle*>(UObjectGlobals::FindFirstOf(FName(STR("REDGameState_Battle"))));
                GameState = AREDGameState_Battle::GetGameState();
                bCanRollback = true;
            }
            else
                bCanRollback = false;
        });
        
        HMODULE BaseModule = GetModuleHandleW(NULL);

        uintptr_t UpdateBattle_Addr = FindPattern(BaseModule, PBYTE("\x48\x83\xEC\x58\x48\x89\x5C\x24\x60\x48\x89\x6C\x24\x68\x4C\x89\x74\x24\x48"), "xxxxxxxxxxxxxxxxxxx");
        UpdateBattle_Detour = new PLH::x64Detour(
            UpdateBattle_Addr, reinterpret_cast<uint64_t>(&UpdateBattle_Hook),
            reinterpret_cast<uint64_t*>(&UpdateBattle_Orig));
        UpdateBattle_Detour->hook();

        uintptr_t UEParticleGet_Addr = FindPattern(BaseModule, PBYTE("\x48\x89\x5C\x24\x18\x55\x57\x41\x57\x48\x8D\x6C\x24\xA0"), "xxxxxxxxxxxxxx");
        UEParticleGet_Detour = new PLH::x64Detour(
            UEParticleGet_Addr, reinterpret_cast<uint64_t>(&UEParticleGet_Hook),
            reinterpret_cast<uint64_t*>(&UEParticleGet_Orig));
        UEParticleGet_Detour->hook();

        uintptr_t CreateParticleArg_Addr = FindPattern(BaseModule, PBYTE("\x48\x85\xD2\x0F\x84\x00\x00\x00\x00\x4C\x8B\xDC\x55\x56\x41\x55"), "xxxxx????xxxxxxx");
        CreateParticleArg_Detour = new PLH::x64Detour(
            CreateParticleArg_Addr, reinterpret_cast<uint64_t>(&CreateParticleArg_Hook),
            reinterpret_cast<uint64_t*>(&CreateParticleArg_Orig));
        CreateParticleArg_Detour->hook();

        uintptr_t DeactivateSystem_Addr = FindPattern(BaseModule, PBYTE("\x40\x53\x41\x54\x41\x55\x48\x83\xEC\x50"), "xxxxxxxxxx");
        DeactivateSystem = reinterpret_cast<DeactivateSystem_Func>(DeactivateSystem_Addr);

        uintptr_t KillParticlesForced_Addr = FindPattern(BaseModule, PBYTE("\x48\x89\x5C\x24\x10\x56\x48\x83\xEC\x20\x48\x83\xB9\xA0\x07\x00\x00\x00"), "xxxxxxxxxxxxxxxxxx");
        KillParticlesForced = reinterpret_cast<KillParticlesForced_Func>(KillParticlesForced_Addr);

        uintptr_t LinkParticleEx_Addr = FindPattern(BaseModule, PBYTE("\x48\x85\xD2\x0F\x84\x00\x00\x00\x00\x55\x41\x56\x41\x57"), "xxxxx????xxxxx");
        LinkParticleEx_Detour = new PLH::x64Detour(
            LinkParticleEx_Addr, reinterpret_cast<uint64_t>(&LinkParticleEx_Hook),
            reinterpret_cast<uint64_t*>(&LinkParticleEx_Orig));
        LinkParticleEx_Detour->hook();

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
            }
        });
    }
};

#define GBVS_ROLLBACK_API __declspec(dllexport)
extern "C"
{
    GBVS_ROLLBACK_API CppUserModBase* start_mod()
    {
        return new GBVSRollback();
    }

    GBVS_ROLLBACK_API void uninstall_mod(CppUserModBase* mod)
    {
        delete mod;
    }
}