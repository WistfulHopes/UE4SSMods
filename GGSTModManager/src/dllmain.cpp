#include <string>

#include <Mod/CppUserModBase.hpp>
#include <UE4SSProgram.hpp>
#include <ModManager.hpp>

#include "d3d11hook.hpp"

class GGSTModManager : public CppUserModBase
{
    GUI::ModManager::ModManager m_pak_reloader{};
    std::thread d3d11_thread;

public:
    GGSTModManager()
    {
        ModName = STR("GGST Mod Manager");
        ModVersion = STR("1.0");
        ModDescription = STR("In-game mod manager for Guilty Gear -Strive-");
        ModAuthors = STR("WistfulHopes");
    }

    auto init_d3d11()
    {
        ImplHookDX11_Init(GetModuleHandle(NULL), FindWindow(STR("UnrealWindow"), NULL));
    }
    
    auto on_unreal_init() -> void override
    {
        m_pak_reloader.init();
        SetModManager(&m_pak_reloader);
        d3d11_thread = std::thread(&GGSTModManager::init_d3d11, this);

        register_keydown_event(Input::F1, [&]
        {
            m_pak_reloader.toggle_open();
        });
    }
    
    ~GGSTModManager() override
    {
        ImplHookDX11_Shutdown();
    }
};

#define GGSTMODMANAGER_API __declspec(dllexport)
extern "C"
{
    GGSTMODMANAGER_API CppUserModBase* start_mod()
    {
        return new GGSTModManager();
    }

    GGSTMODMANAGER_API void uninstall_mod(CppUserModBase* mod)
    {
        delete mod;
    }
}

