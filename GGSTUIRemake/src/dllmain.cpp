#include <DynamicOutput/Output.hpp>
#include <Mod/CppUserModBase.hpp>
#include <polyhook2/Exceptions/AVehHook.hpp>
#include <Unreal/UObjectGlobals.hpp>

#include "AActor.hpp"
#include "BPMacros.hpp"
#include "REDGameMode_CharaSelectRE.h"
#include "REDGameState.h"
#include "REDGameState_CharaSelectRE.h"
#include "UE4SSProgram.hpp"
#include "UFunction.hpp"
#include "SigScanner/SinglePassSigScanner.hpp"

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

        auto get_signatures()
        {
            const SignatureContainer OnStartedLobbyTraining{
                {{"40 53 48 83 EC 20 48 81 C1 90 01 00 00 E8"}},
                [&](const SignatureContainer& self)
                {
                    AREDGameState_CharaSelectRE::OnStartedLobbyTraining_Func.assign_address(self.get_match_address());
                    return true;
                },
                [](SignatureContainer& self)
                {
                },
            };
            const SignatureContainer AREDHUD_FadeIn{
                {{"48 89 5C 24 ? 57 48 83 EC 20 8B FA 48 8B D9 E8 ? ? ? ? 48 85 C0"}},
                [&](const SignatureContainer& self)
                {
                    AREDGameState_CharaSelectRE::AREDHUD_FadeIn_Func.assign_address(self.get_match_address());
                    return true;
                },
                [](SignatureContainer& self)
                {
                },
            };

            std::vector<SignatureContainer> signature_containers;
            signature_containers.push_back(OnStartedLobbyTraining);
            signature_containers.push_back(AREDHUD_FadeIn);

            SinglePassScanner::SignatureContainerMap signature_containers_map;
            signature_containers_map.emplace(ScanTarget::MainExe, signature_containers);

            SinglePassScanner::start_scan(signature_containers_map);
        }
        
        auto on_unreal_init() -> void override
        {
            get_signatures();
            
            AREDGameState_CharaSelectRE::InitializeClass();
            AREDGameMode_CharaSelectRE::InitializeClass();
            FCharaSelectPlayerParam::InitializeStruct();

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
                    return;
                }
                typedef int (*InsertStruct_t)(FString, FDynamicScriptStruct*);
                if (const auto InsertStruct = reinterpret_cast<InsertStruct_t>(GetProcAddress(Suzie, "InsertStruct")))
                {
                    InsertStruct(FString(STR("/Script/REDExtend.CharaSelectPlayerParam")), &FCharaSelectPlayerParam::Struct);
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
