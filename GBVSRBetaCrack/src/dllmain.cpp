#include <polyhook2/Detour/x64Detour.hpp>
#include <polyhook2/Exceptions/AVehHook.hpp>

#include "Mod/CppUserModBase.hpp"
char(*LoginResult_Orig)();
char LoginResult_New()
{
    return 2;
}

char(*LoginFunc1)();
char(*LoginFunc2)();
char(*LoginFunc3)();
char(*LoginFunc4)();
char LoginFunc_New()
{
    return 0;
}
char LoginFunc4_New()
{
    return 1;
}

PLH::x64Detour* LoginResult_Detour;
PLH::x64Detour* LoginFunc1_Detour;
PLH::x64Detour* LoginFunc2_Detour;
PLH::x64Detour* LoginFunc3_Detour;
PLH::x64Detour* LoginFunc4_Detour;

class GBVSRBetaCrack : public CppUserModBase
{
public:
    
    GBVSRBetaCrack()
    {
        ModName = STR("GBVSRBetaCrack");
        ModVersion = STR("1.0");
        ModDescription = STR("Hello cracker");
        ModAuthors = STR("WistfulHopes");
        // Do not change this unless you want to target a UE4SS version
        // other than the one you're currently building with somehow.
        //ModIntendedSDKVersion = STR("2.6");
    }

    ~GBVSRBetaCrack() override = default;

    auto on_update() -> void override {}

    auto on_unreal_init() -> void override
    {
        HMODULE BaseModule = GetModuleHandleW(NULL);

        LoginResult_Detour = new PLH::x64Detour(
            (uintptr_t)BaseModule + 0x1383600,
            reinterpret_cast<uint64_t>(&LoginResult_New),
            reinterpret_cast<uint64_t*>(&LoginResult_Orig));
        LoginResult_Detour->hook();
        LoginFunc1_Detour = new PLH::x64Detour(
            (uintptr_t)BaseModule + 0x10D7360,
            reinterpret_cast<uint64_t>(&LoginFunc_New),
            reinterpret_cast<uint64_t*>(&LoginFunc1));
        LoginFunc1_Detour->hook();
        LoginFunc2_Detour = new PLH::x64Detour(
            (uintptr_t)BaseModule + 0x10D73A0,
            reinterpret_cast<uint64_t>(&LoginFunc_New),
            reinterpret_cast<uint64_t*>(&LoginFunc2));
        LoginFunc2_Detour->hook();
        LoginFunc3_Detour = new PLH::x64Detour(
            (uintptr_t)BaseModule + 0x10D7380,
            reinterpret_cast<uint64_t>(&LoginFunc_New),
            reinterpret_cast<uint64_t*>(&LoginFunc3));
        LoginFunc3_Detour->hook();
        LoginFunc4_Detour = new PLH::x64Detour(
            (uintptr_t)BaseModule + 0x10D7520,
            reinterpret_cast<uint64_t>(&LoginFunc4_New),
            reinterpret_cast<uint64_t*>(&LoginFunc4));
        LoginFunc4_Detour->hook();
        
        DWORD Old1;
        uintptr_t Patch1 = reinterpret_cast<uintptr_t>(BaseModule) + 0x1591513;
        PBYTE Patch1Bytes = (PBYTE)"\xC7\x46\x44\x0A";
        
        bool Success1 = VirtualProtect((LPVOID)Patch1, 0x4, PAGE_READWRITE, &Old1);

        if (Success1)
        {
            memcpy((void*)Patch1, Patch1Bytes, 0x4);

            VirtualProtect((LPVOID)Patch1, 0x4, Old1, &Old1);
        }
    }
};

#define GBVSRBETACRACK_API __declspec(dllexport)
extern "C"
{
    GBVSRBETACRACK_API CppUserModBase* start_mod()
    {
        return new GBVSRBetaCrack();
    }

    GBVSRBETACRACK_API void uninstall_mod(CppUserModBase* mod)
    {
        delete mod;
    }
}