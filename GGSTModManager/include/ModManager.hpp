#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace RC::GUI::ModManager
{
    enum class Error
    {
        Success,
        InvalidPath,
    };
    
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
        bool m_is_open{true};
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
        auto error_popup() -> void;
        auto load_sig_path() -> void;
        
        ModsConfig m_config;
        Error m_error;
        bool m_mods_loaded{false};
        int m_active_item{-1};
        std::filesystem::path m_sig{};
    };
}
