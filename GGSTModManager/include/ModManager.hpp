#pragma once

#include <string>
#include <vector>

namespace RC::GUI::ModManager
{
    struct ModEntry
    {
        bool enabled;
        std::string m_path;
        std::string m_name;
    };

    struct ModsConfig
    {
        std::vector<ModEntry> m_mods;
        std::string m_mods_folder{};
    };
    
    class ModManager
    {
    public:
        ModManager();
        ~ModManager();
        
        auto render() -> void;
        auto init() -> void;

        auto toggle_open() -> void;
        
    private:
        auto load_config() -> void;
        auto save_config() -> void;
        auto load_mods() -> void;
        auto mods_folder_popup() -> void;
        
        ModsConfig m_config;
        bool m_is_open{true};
        bool m_mods_loaded{false};
        int m_active_item{-1};
    };
}
