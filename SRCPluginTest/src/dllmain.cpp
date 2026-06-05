#include <DynamicOutput/Output.hpp>
#include <Mod/CppUserModBase.hpp>
#include <polyhook2/Exceptions/AVehHook.hpp>
#include <Unreal/UObjectGlobals.hpp>

#include "BPMacros.hpp"
#include "TestObject.h"
#include "UE4SSProgram.hpp"
#include "SigScanner/SinglePassSigScanner.hpp"

namespace SRCMods
{
    using namespace RC;
    using namespace Unreal;

    /**
    * SRCPluginTest: UE4SS c++ mod class defintion
    */
    class SRCPluginTest : public CppUserModBase
    {
    public:
        // constructor
        SRCPluginTest()
        {
            ModVersion = STR("0.1");
            ModName = STR("SRCPluginTest");
            ModAuthors = STR("UE4SS");
            ModDescription = STR("Plugin test for Sonic Racing Crossworlds");
        }

        // destructor
        ~SRCPluginTest() override
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
            UTestObject::InitializeClass();

            if (const auto Suzie = LoadLibrary(STR("./ue4ss/Mods/SuzieMod/dlls/main.dll")))
            {
                typedef int (*InsertClass_t)(FString, FDynamicClass*);
                if (const auto InsertClass = reinterpret_cast<InsertClass_t>(GetProcAddress(Suzie, "InsertClass")))
                {
                    InsertClass(FString(STR("/Script/UNIONExtend.TestObject")),
                                &UTestObject::Data);
                }
                else
                {
                    Output::send<LogLevel::Error>(STR("Failed to find InsertClass!"));
                    return;
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
#define SRCPLUGINTEST_EXPORT __declspec(dllexport)

extern "C" {
SRCPLUGINTEST_EXPORT RC::CppUserModBase* start_mod()
{
    return new SRCMods::SRCPluginTest();
}

SRCPLUGINTEST_EXPORT void uninstall_mod(RC::CppUserModBase* mod)
{
    delete mod;
}
}
