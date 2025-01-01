#include <ModManager.hpp>

#include <ranges>
#include <vector>
#include <unordered_map>

#include <DynamicOutput/DynamicOutput.hpp>
#include <Helpers/String.hpp>
#include <SigScanner/SinglePassSigScanner.hpp>

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <glaze/json/read.hpp>
#include <glaze/json/write.hpp>
#include <misc/cpp/imgui_stdlib.h>

#include "FString.hpp"
#include "Mod/Mod.hpp"

namespace RC::GUI::ModManager
{
    using namespace RC::Unreal;

    struct V425_FPakFile
    {
        void* idk1;
        FString PakFilename;
    };

    struct FPakFile
    {
        FString PakFilename()
        {
            return std::bit_cast<V425_FPakFile*>(this)->PakFilename;
        }
    };

    struct FPakListEntry
    {
        uint32_t ReadOrder;
        FPakFile* PakFile;
    };

    struct FPakPlatformFile
    {
        void* vftable;
        void* LowerLevel;
        TArray<FPakListEntry> PakFiles;
    };

    struct V425_DelegateMount
    {
        void* idk1;
        FPakPlatformFile* pak;
        bool (*fn)(FPakPlatformFile*, FString&, uint32_t, void*);
    };

    struct DelegateMount
    {
        FPakPlatformFile* Pak()
        {
            return std::bit_cast<V425_DelegateMount*>(this)->pak;
        }

        bool Call(FString& str, uint32_t order)
        {
            return std::bit_cast<V425_DelegateMount*>(this)->fn(this->Pak(), str, order, nullptr);
        }
    };

    struct V425_DelegateUnmount
    {
        void* idk1;
        FPakPlatformFile* pak;
        bool (*fn)(FPakPlatformFile*, FString&);
    };

    struct DelegateUnmount
    {
        FPakPlatformFile* Pak()
        {
            return std::bit_cast<V425_DelegateUnmount*>(this)->pak;
        }

        bool Call(FString& str)
        {
            return std::bit_cast<V425_DelegateUnmount*>(this)->fn(this->Pak(), str);
        }
    };

    DelegateMount* MountPak = nullptr;
    DelegateUnmount* OnUnmountPak = nullptr;

    ModManager::ModManager()
    {
    }

    ModManager::~ModManager()
    {
    }

    auto ModManager::render() -> void
    {
        static bool start_pos_set = false;

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGuiWindowFlags window_flags = 0;
        ImGui::SetNextWindowSize(ImVec2(450, 600), ImGuiCond_FirstUseEver);
        if (!start_pos_set)
        {
            ImGui::SetNextWindowPos(ImVec2(25, 25));
            start_pos_set = true;
        }

        ImGui::GetIO().MouseDrawCursor = m_is_open;

        if (OnUnmountPak != nullptr && MountPak != nullptr && m_is_open)
        {
            mods_folder_popup();

            ImGui::Begin("Guilty Gear -Strive- Mod Manager", &m_is_open, window_flags);

            ImGuiStyle& style = ImGui::GetStyle();

            ImVec2 windowPosition = ImGui::GetWindowPos();
            ImVec2 cursorPosition = ImGui::GetCursorPos();
            float scroll = ImGui::GetScrollY();

            // this is not a pixel perfect position
            // you can try to make it more accurate by adding some offset
            ImVec2 itemPosition(
                windowPosition.x + cursorPosition.x,
                windowPosition.y + cursorPosition.y - style.ItemSpacing.y
            );

            for (int i = 0; i < m_config.m_mods.size(); i++)
            {
                ImGui::PushID(i);

                ImGui::Checkbox("##mod-enabled", &m_config.m_mods[i].enabled);

                ImGui::SameLine();
                ImGui::Selectable((m_config.m_mods[i].m_name + "##" + std::to_string(i)).c_str());

                if (ImGui::IsItemActive())
                {
                    m_active_item = i;
                }
                else if (m_active_item == i)
                {
                    int n_next = floorf((ImGui::GetMousePos().y + scroll - itemPosition.y) / 31);

                    if (n_next != i && n_next >= 0 && n_next < m_config.m_mods.size())
                    {
                        std::swap(m_config.m_mods.at(i), m_config.m_mods.at(n_next));
                    }

                    m_active_item = -1;
                }

                ImGui::Separator();
            }

            if (ImGui::Button("Change Mod Folder"))
            {
                m_config.m_mods_folder = "";
            }

            if (ImGui::Button("Reload Mods"))
            {
                m_mods_loaded = false;
            }

            if (ImGui::Button("Save Changes"))
            {
                save_config();
            }

            ImGui::End();
        }

        ImGui::Render();
    }

    auto ModManager::mods_folder_popup() -> void
    {
        if (!m_config.m_mods_folder.empty() && !ImGui::IsPopupOpen("Select Mods Folder"))
        {
            load_mods();
            return;
        }

        if (!ImGui::IsPopupOpen("Select Mods Folder"))
            ImGui::OpenPopup("Select Mods Folder");

        if (ImGui::BeginPopupModal("Select Mods Folder", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Select the folder you wish to use to load and save mods.");

            ImVec2 button_size(ImGui::GetFontSize() * 7.0f, 0.0f);

            ImGui::InputText("##mods-folder", &m_config.m_mods_folder);
            ImGui::SameLine();

            if (ImGui::Button("Confirm", button_size) && !m_config.m_mods_folder.empty())
            {
                m_mods_loaded = false;
                load_mods();
                ImGui::CloseCurrentPopup();
            }
        }
    }

    auto ModManager::init() -> void
    {
        load_config();
        SignatureContainer fplatformfilepak_dtor = [&]() -> SignatureContainer
        {
            return {
                {
                    {
                        .signature = "40 56 48 83 EC 30 48 89 5C 24 ?? 48 8D 05 ?? ?? ?? ?? 48 89 7C 24",
                    },
                },
                [&](SignatureContainer& self)
                {
                    Output::send<LogLevel::Warning>(
                        STR("[ModManager]: found match: {}\n"), (void*)self.get_match_address());

                    int num = 0;

                    uint8_t* data = self.get_match_address();
                    void* last = nullptr;
                    for (uint8_t* i : std::views::iota(data) | std::views::take(3000))
                    {
                        // look for 'mov rcx,[rel address]'
                        if (i[0] == 0x48 && i[1] == 0x8b && i[2] == 0x0d)
                        {
                            // mov found, resolve RIP
                            int32_t rip;
                            memcpy(&rip, i + 3, sizeof(rip));

                            void* ptr = i + 3 + 4 + rip;

                            if (ptr != last)
                            {
                                last = ptr;

                                Output::send<LogLevel::Warning>(
                                    STR("[ModManager]: found delegate: {} ({})\n"), ptr, num);

                                switch (num)
                                {
                                case 1:
                                    MountPak = *(DelegateMount**)ptr;
                                    break;
                                case 2:
                                    OnUnmountPak = *(DelegateUnmount**)ptr;
                                    self.get_did_succeed() = true;
                                    return true;
                                default:
                                    break;
                                }

                                num += 1;
                            }
                        }
                    }

                    return false;
                },
                [&](const SignatureContainer& self)
                {
                    if (!self.get_did_succeed())
                    {
                        Output::send<LogLevel::Warning>(STR("[ModManager]: Pak delegates not found\n"));
                    }
                }
            };
        }();

        SinglePassScanner::SignatureContainerMap container_map;
        std::vector<SignatureContainer> container;
        container.emplace_back(fplatformfilepak_dtor);
        container_map.emplace(ScanTarget::Core, container);
        SinglePassScanner::start_scan(container_map);
    }

    auto ModManager::toggle_open() -> void
    {
        m_is_open = !m_is_open;
    }

    auto ModManager::load_config() -> void
    {
        auto json = glz::read_file_json(m_config, "./mod_manager.json", std::string{});
    }

    auto ModManager::save_config() -> void
    {
        auto json = glz::write_file_json(m_config, "./mod_manager.json", std::string{});
    }

    auto ModManager::load_mods() -> void
    {
        if (m_mods_loaded || m_config.m_mods_folder.empty()) return;

        save_config();

        std::vector<ModEntry> enabled_mods;
        std::vector<ModEntry> disabled_mods;

        for (auto entry : m_config.m_mods)
        {
            if (entry.enabled) enabled_mods.emplace_back(entry);
            else disabled_mods.emplace_back(entry);
        }
        
        m_config.m_mods.clear();
        m_config.m_mods.insert(m_config.m_mods.end(), enabled_mods.begin(), enabled_mods.end());
        m_config.m_mods.insert(m_config.m_mods.end(), disabled_mods.begin(), disabled_mods.end());

        using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;

        std::vector<std::string> existing_mod_entries;

        for (const auto& dir_entry : recursive_directory_iterator(m_config.m_mods_folder))
        {
            if (dir_entry.path().extension() == ".pak")
            {
                existing_mod_entries.push_back(dir_entry.path().string());

                auto it = std::ranges::find_if(m_config.m_mods, [&](const ModEntry& mod)
                {
                    return mod.m_path == dir_entry.path();
                });

                if (it == m_config.m_mods.end())
                {
                    m_config.m_mods.emplace_back(ModEntry{
                        .enabled = true, .m_path = dir_entry.path().string(),
                        .m_name = dir_entry.path().filename().string()
                    });
                }
            }
        }

        auto it = std::ranges::begin(m_config.m_mods);
        while (it != m_config.m_mods.end())
        {
            if (std::ranges::find(existing_mod_entries, it->m_path) == existing_mod_entries.end())
            {
                it = m_config.m_mods.erase(it);
            }
            else ++it;
        }

        for (auto& m_mod : std::ranges::reverse_view(m_config.m_mods))
        {
            auto path = FString(to_wstring(m_mod.m_path).c_str());
            OnUnmountPak->Call(path);
            
            if (!m_mod.enabled) continue;
            
            MountPak->Call(path, MountPak->Pak()->PakFiles.Num() * 100);
        }

        m_mods_loaded = true;
    }
}
