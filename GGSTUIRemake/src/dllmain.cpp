#include <DynamicOutput/Output.hpp>
#include <Mod/CppUserModBase.hpp>
#include <polyhook2/Exceptions/AVehHook.hpp>
#include <Unreal/UObjectGlobals.hpp>

#include "AActor.hpp"
#include "BPMacros.hpp"
#include "REDGameMode_CharaSelectRE.h"
#include "REDGameState_CharaSelectRE.h"
#include "UE4SSProgram.hpp"
#include "UFunction.hpp"

namespace GGSTMods
{
    using namespace RC;
    using namespace Unreal;

    /**
    * GGSTUIRemake: UE4SS c++ mod class defintion
    */
    class GGSTUIRemake : public CppUserModBase
    {
    public:
        // constructor
        GGSTUIRemake()
        {
            ModVersion = STR("0.1");
            ModName = STR("GGSTUIRemake");
            ModAuthors = STR("UE4SS");
            ModDescription = STR("Code behind for GGST UI remake");
        }

        // destructor
        ~GGSTUIRemake() override
        {
            // fill when required
        }

        auto on_program_start() -> void override
        {
        }

        auto on_dll_load(std::wstring_view dll_name) -> void override
        {
        }

        auto on_unreal_init() -> void override
        {
            AREDGameMode_CharaSelectRE::Class.ClassDefaultObjectPath = FString(STR("/Script/REDExtend.Default__REDGameMode_CharaSelectRE"));
            AREDGameMode_CharaSelectRE::Class.SuperStruct = FString(STR("/Script/RED.REDGameMode"));
            AREDGameState_CharaSelectRE::Class.ClassDefaultObjectPath = FString(STR("/Script/REDExtend.Default__REDGameState_CharaSelectRE"));
            AREDGameState_CharaSelectRE::Class.SuperStruct = FString(STR("/Script/RED.REDGameState"));

            if (const auto Suzie = LoadLibrary(STR("./ue4ss/Mods/SuzieMod/dlls/main.dll")))
            {
                typedef int (*InsertClass_t)(FString, FDynamicClass*);
                if (const auto InsertClass = reinterpret_cast<InsertClass_t>(GetProcAddress(Suzie, "InsertClass")))
                {
                    InsertClass(FString(STR("/Script/REDExtend.REDGameState_CharaSelectRE")), &AREDGameState_CharaSelectRE::Class);
                    InsertClass(FString(STR("/Script/REDExtend.REDGameMode_CharaSelectRE")), &AREDGameMode_CharaSelectRE::Class);
                }
                else
                {
                    Output::send<LogLevel::Error>(STR("Failed to find InsertClass!"));
                }
            }
            else
            {
                Output::send<LogLevel::Error>(STR("Failed to load Suzie!"));
            }
        }
    }; //class
}

/**
* export the start_mod() and uninstall_mod() functions to
* be used by the core ue4ss system to load in our dll mod
*/
#define GGSTUIREMAKE_EXPORT __declspec(dllexport)

extern "C" {
    GGSTUIREMAKE_EXPORT RC::CppUserModBase* start_mod()
    {
        return new GGSTMods::GGSTUIRemake();
    }

    GGSTUIREMAKE_EXPORT void uninstall_mod(RC::CppUserModBase* mod)
    {
        delete mod;
    }
}
