#include <Mod/CppUserModBase.hpp>
#include <polyhook2/Detour/x64Detour.hpp>
#include <polyhook2/Exceptions/AVehHook.hpp>
#include <SigScanner/SinglePassSigScanner.hpp>

using namespace RC::Unreal;

#define MVCICHARAENABLE_API __declspec(dllexport)

bool(*IsValidCharID0_Orig)();
bool(*IsValidCharID1_Orig)();
bool(*IsValidCharID2_Orig)();
bool(*IsValidCharID3_Orig)();
bool(*IsValidCharID4_Orig)();
bool(*IsValidCharID5_Orig)();
bool IsValidCharID_New()
{
	return true;
}
bool IsValidCharID_Test(int charaId, uint8 flag)
{
	Output::send(std::format(STR("{}"), charaId));
	return true;
}

PLH::x64Detour* IsValidCharID0_Detour;
PLH::x64Detour* IsValidCharID1_Detour;
PLH::x64Detour* IsValidCharID2_Detour;
PLH::x64Detour* IsValidCharID3_Detour;
PLH::x64Detour* IsValidCharID4_Detour;
PLH::x64Detour* IsValidCharID5_Detour;

/* Mod Definition */
class MVCICharaEnable : public CppUserModBase 
{
public:
	MVCICharaEnable() {
		ModName = STR("MVCI Chara Enabler");
		ModVersion = STR("1.0");
		ModDescription = STR("Enables invalid character slots (for custom characters).");
		ModAuthors = STR("WistfulHopes");
		// Do not change this unless you want to target a UE4SS version
		// other than the one you're currently building with somehow.
		// ModIntendedSDKVersion = STR("2.6");
	}

	~MVCICharaEnable() override {}

	auto on_update() -> void override {
	}

	auto patch_exe_bytes(uintptr_t address, PBYTE patch, int size) -> void
	{
		DWORD old;        
		bool success = VirtualProtect((LPVOID)address, size, PAGE_READWRITE, &old);
		if (success)
		{
			memcpy((void*)address, patch, size);

			VirtualProtect((LPVOID)address, size, old, &old);
		}
	}
	
	auto on_unreal_init() -> void override {
		HMODULE BaseModule = GetModuleHandleW(NULL);
		IsValidCharID0_Detour = new PLH::x64Detour(
			(uint64_t)BaseModule + 0x448380,
			reinterpret_cast<uint64_t>(&IsValidCharID_Test),
			reinterpret_cast<uint64_t*>(&IsValidCharID0_Orig));
		IsValidCharID0_Detour->hook();
		IsValidCharID1_Detour = new PLH::x64Detour(
			(uint64_t)BaseModule + 0x4424C0,
			reinterpret_cast<uint64_t>(&IsValidCharID_New),
			reinterpret_cast<uint64_t*>(&IsValidCharID1_Orig));
		IsValidCharID1_Detour->hook();
		IsValidCharID2_Detour = new PLH::x64Detour(
			(uint64_t)BaseModule + 0x4479D0,
			reinterpret_cast<uint64_t>(&IsValidCharID_New),
			reinterpret_cast<uint64_t*>(&IsValidCharID2_Orig));
		IsValidCharID2_Detour->hook();
		IsValidCharID3_Detour = new PLH::x64Detour(
			(uint64_t)BaseModule + 0x447A20,
			reinterpret_cast<uint64_t>(&IsValidCharID_New),
			reinterpret_cast<uint64_t*>(&IsValidCharID3_Orig));
		IsValidCharID3_Detour->hook();
		IsValidCharID4_Detour = new PLH::x64Detour(
			(uint64_t)BaseModule + 0x447B20,
			reinterpret_cast<uint64_t>(&IsValidCharID_New),
			reinterpret_cast<uint64_t*>(&IsValidCharID4_Orig));
		IsValidCharID4_Detour->hook();
		IsValidCharID5_Detour = new PLH::x64Detour(
			(uint64_t)BaseModule + 0x448400,
			reinterpret_cast<uint64_t>(&IsValidCharID_New),
			reinterpret_cast<uint64_t*>(&IsValidCharID5_Orig));
		IsValidCharID5_Detour->hook();

		patch_exe_bytes((uint64_t)BaseModule + 0x31B151, (PBYTE)"\xFD", 1);
		patch_exe_bytes((uint64_t)BaseModule + 0x31B257, (PBYTE)"\xFD", 1);
		patch_exe_bytes((uint64_t)BaseModule + 0x2FB10B, (PBYTE)"\xFE", 1);
		patch_exe_bytes((uint64_t)BaseModule + 0x306701, (PBYTE)"\xFE", 1);
		patch_exe_bytes((uint64_t)BaseModule + 0x30163E, (PBYTE)"\xFE", 1);
		patch_exe_bytes((uint64_t)BaseModule + 0x32FFCC, (PBYTE)"\xFD", 1);
		patch_exe_bytes((uint64_t)BaseModule + 0x33911B, (PBYTE)"\xFE", 1);
		patch_exe_bytes((uint64_t)BaseModule + 0x3FB216, (PBYTE)"\xFE", 1);
		patch_exe_bytes((uint64_t)BaseModule + 0x3FB9D6, (PBYTE)"\xFE", 1);
		patch_exe_bytes((uint64_t)BaseModule + 0x44843F, (PBYTE)"\xFC", 1);
		patch_exe_bytes((uint64_t)BaseModule + 0x448A17, (PBYTE)"\xFC", 1);
		patch_exe_bytes((uint64_t)BaseModule + 0x310B21, (PBYTE)"\xFE", 1);
		patch_exe_bytes((uint64_t)BaseModule + 0x2AD0C9, (PBYTE)"\xFE", 1);
		patch_exe_bytes((uint64_t)BaseModule + 0x2AE57D, (PBYTE)"\xFE", 1);
		patch_exe_bytes((uint64_t)BaseModule + 0x316F84, (PBYTE)"\xFD", 1);
		patch_exe_bytes((uint64_t)BaseModule + 0x316C84, (PBYTE)"\xFD", 1);
		patch_exe_bytes((uint64_t)BaseModule + 0x317064, (PBYTE)"\xFD", 1);
		patch_exe_bytes((uint64_t)BaseModule + 0x3173A4, (PBYTE)"\xFD", 1);
		patch_exe_bytes((uint64_t)BaseModule + 0x3175AD, (PBYTE)"\xFD", 1);
		patch_exe_bytes((uint64_t)BaseModule + 0x3179AE, (PBYTE)"\xFD", 1);
		patch_exe_bytes((uint64_t)BaseModule + 0x3179AE, (PBYTE)"\xFD", 1);
		patch_exe_bytes((uint64_t)BaseModule + 0x317CC4, (PBYTE)"\xFD", 1);
		patch_exe_bytes((uint64_t)BaseModule + 0x318604, (PBYTE)"\xFD", 1);
		patch_exe_bytes((uint64_t)BaseModule + 0x3186B4, (PBYTE)"\xFD", 1);
		patch_exe_bytes((uint64_t)BaseModule + 0x318764, (PBYTE)"\xFD", 1);
		patch_exe_bytes((uint64_t)BaseModule + 0x319168, (PBYTE)"\xFD", 1);
		patch_exe_bytes((uint64_t)BaseModule + 0x319FB4, (PBYTE)"\xFD", 1);
		patch_exe_bytes((uint64_t)BaseModule + 0x31A09B, (PBYTE)"\xFD", 1);
		patch_exe_bytes((uint64_t)BaseModule + 0x31A16B, (PBYTE)"\xFD", 1);
		patch_exe_bytes((uint64_t)BaseModule + 0x31A344, (PBYTE)"\xFD", 1);
		patch_exe_bytes((uint64_t)BaseModule + 0x31A404, (PBYTE)"\xFD", 1);
		patch_exe_bytes((uint64_t)BaseModule + 0x31A4E4, (PBYTE)"\xFD", 1);
		patch_exe_bytes((uint64_t)BaseModule + 0x31B094, (PBYTE)"\xFD", 1);
	}
};

extern "C" {
	MVCICHARAENABLE_API CppUserModBase* start_mod() {
	  return new MVCICharaEnable();
	}

	MVCICHARAENABLE_API void uninstall_mod(CppUserModBase* mod) {
	  delete mod;
	}
}