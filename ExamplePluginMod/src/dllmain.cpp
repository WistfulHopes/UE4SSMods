#include <Mod/CppUserModBase.hpp>
#include <Unreal/UObjectGlobals.hpp>
#include <Unreal/UObject.hpp>
#include "Hooks.hpp"
#include "UE4SSProgram.hpp"
#include "ModuleManager.hpp"

namespace ExamplePlugin
{
    using namespace ModuleManager;

    /**
    * ExamplePluginMod: UE4SS c++ mod class defintion
    */
    class ExamplePluginMod : public CppUserModBase
    {
    public:
        
        // constructor
        ExamplePluginMod() {
            ModVersion = STR("0.1");
            ModName = STR("ExamplePluginMod");
            ModAuthors = STR("UE4SS");
            ModDescription = STR("A basic template C++ mod");
            // Do not change this unless you want to target a UE4SS version
            // other than the one you're currently building with somehow.
            //ModIntendedSDKVersion = STR("2.6");
        }
        
        // destructor
        ~ExamplePluginMod() override {
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
            Unreal::Hook::RegisterProcessEventPreCallback(&process_event_hook);
        }

        static inline bool bModuleLoaded;
        
        auto static process_event_hook([[maybe_unused]] Unreal::UObject* Context, [[maybe_unused]] Unreal::UFunction* Function, [[maybe_unused]] void* Parms) -> void
        {
            if (!FModuleManager::InitializeWrapper() || bModuleLoaded) return;
            bModuleLoaded = true;

            const auto module_directory = std::filesystem::path(UE4SSProgram::get_program().get_module_directory());
            const auto plugin_directory = module_directory.parent_path().parent_path().parent_path() / "Plugins" / "ExamplePlugin" / "Binaries" / "Win64";

            if (!exists(plugin_directory)) return;
            
            SetDllDirectory(plugin_directory.c_str());

            const auto dll_path = plugin_directory.wstring() + STR("\\RED-ExamplePlugin-Win64-Shipping.dll");

            FModuleManager::Get().AddModule(Unreal::FName(STR("ExamplePlugin"), Unreal::FNAME_Add), Unreal::FString(dll_path.c_str()));
            FModuleManager::Get().LoadModule(Unreal::FName(STR("ExamplePlugin"), Unreal::FNAME_Add));
        }
    };//class
}

/**
* export the start_mod() and uninstall_mod() functions to
* be used by the core ue4ss system to load in our dll mod
*/
#define EXAMPLEPLUGIN_API __declspec(dllexport) 
extern "C" {
    EXAMPLEPLUGIN_API CppUserModBase* start_mod(){ return new ExamplePlugin::ExamplePluginMod(); }
    EXAMPLEPLUGIN_API void uninstall_mod(CppUserModBase* mod) { delete mod; }
}