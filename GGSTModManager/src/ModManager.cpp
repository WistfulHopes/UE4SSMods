#include <ModManager.hpp>

#include <ranges>
#include <vector>
#include <unordered_map>

#include <DynamicOutput/DynamicOutput.hpp>
#include <Helpers/String.hpp>
#include <SigScanner/SinglePassSigScanner.hpp>

#include <imgui.h>
#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_win32.h>
#include <glaze/json/read.hpp>
#include <misc/cpp/imgui_stdlib.h>

#include "FString.hpp"
#include "UE4SSProgram.hpp"

#include <ShObjIdl_core.h>
#include <glaze/json/write.hpp>

namespace RC::GUI::ModManager
{
    using namespace RC::Unreal;

    struct V425_FPakFile
    {
        void* idk1;
        FString PakFilename;
    };

    struct V427_FPakFile
    {
        void* idk1;
        void* idk2;
        void* idk3;
        FString PakFilename;
    };

    struct FPakFile
    {
        FString PakFilename()
        {
            if (Version::IsBelow(4, 27))
                return std::bit_cast<V425_FPakFile*>(this)->PakFilename;
            else
                return std::bit_cast<V427_FPakFile*>(this)->PakFilename;
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

    struct V427_DelegateMount
    {
        void* idk1;
        void* idk2;
        void* idk3;
        FPakPlatformFile* pak;
        bool (*fn)(FPakPlatformFile*, FString&, uint32_t);
    };

    struct DelegateMount
    {
        FPakPlatformFile* Pak()
        {
            if (Version::IsBelow(4, 27))
                return std::bit_cast<V425_DelegateMount*>(this)->pak;
            else
                return std::bit_cast<V427_DelegateMount*>(this)->pak;
        }

        bool Call(FString& str, uint32_t order)
        {
            if (Version::IsBelow(4, 27))
                return std::bit_cast<V425_DelegateMount*>(this)->fn(this->Pak(), str, order, nullptr);
            else
                return std::bit_cast<V427_DelegateMount*>(this)->fn(this->Pak(), str, order);
        }
    };

    struct V425_DelegateUnmount
    {
        void* idk1;
        FPakPlatformFile* pak;
        bool (*fn)(FPakPlatformFile*, FString&);
    };

    struct V427_DelegateUnmount
    {
        void* idk1;
        void* idk2;
        void* idk3;
        FPakPlatformFile* pak;
        bool (*fn)(FPakPlatformFile*, FString&);
    };

    struct DelegateUnmount
    {
        FPakPlatformFile* Pak()
        {
            if (Version::IsBelow(4, 27))
                return std::bit_cast<V425_DelegateUnmount*>(this)->pak;
            else
                return std::bit_cast<V427_DelegateUnmount*>(this)->pak;
        }

        bool Call(FString& str)
        {
            if (Version::IsBelow(4, 27))
                return std::bit_cast<V425_DelegateUnmount*>(this)->fn(this->Pak(), str);
            else
                return std::bit_cast<V427_DelegateUnmount*>(this)->fn(this->Pak(), str);
        }
    };

    namespace
    {
        DelegateMount* MountPak = nullptr;
        DelegateUnmount* OnUnmountPak = nullptr;
    }

    ModManager::ModManager() = default;

    ModManager::~ModManager() = default;

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

        ImGui::GetIO().MouseDrawCursor = m_config.m_is_open;

        if (OnUnmountPak != nullptr && MountPak != nullptr && m_config.m_is_open)
        {
            mods_folder_popup();
            error_popup();

            ImGui::Begin("Guilty Gear -Strive- Mod Manager", &m_config.m_is_open, window_flags);

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

                ImGui::PopID();
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

        if (!ImGui::IsPopupOpen("Select Mods Folder") && !ImGui::IsPopupOpen("Error"))
            ImGui::OpenPopup("Select Mods Folder");

        if (ImGui::BeginPopupModal("Select Mods Folder", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Select the folder you wish to use to load and save mods.");

            ImGui::InputText("##mods-folder", &m_config.m_mods_folder);
            ImGui::SameLine();

            if (ImGui::Button("Browse"))
            {
                IFileDialog* pfd;
                if (SUCCEEDED(CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd))))
                {
                    DWORD dwOptions;
                    if (SUCCEEDED(pfd->GetOptions(&dwOptions)))
                    {
                        pfd->SetOptions(dwOptions | FOS_PICKFOLDERS);
                    }
                    if (SUCCEEDED(pfd->Show(NULL)))
                    {
                        IShellItem* psi;
                        if (SUCCEEDED(pfd->GetResult(&psi)))
                        {
                            LPWSTR szFolder{};
                            psi->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &szFolder);
                            m_config.m_mods_folder = to_string(szFolder);
                            psi->Release();
                        }
                    }
                    pfd->Release();
                }
            }

            if (ImGui::Button("Confirm") && !m_config.m_mods_folder.empty())
            {
                if (std::filesystem::exists(m_config.m_mods_folder))
                {
                    m_mods_loaded = false;
                    load_mods();
                    ImGui::CloseCurrentPopup();
                }
                else
                {
                    m_error = Error::InvalidPath;
                    ImGui::CloseCurrentPopup();
                }
            }

            ImGui::EndPopup();
        }
    }

    auto ModManager::error_popup() -> void
    {
        if (m_error == Error::Success) return;
        
        if (!ImGui::IsPopupOpen("Error"))
            ImGui::OpenPopup("Error");

        if (ImGui::BeginPopupModal("Error", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            switch (m_error)
            {
            case Error::InvalidPath:
                ImGui::Text(
                    "The path you entered is not valid. Please enter the full path to your mods folder.");
                break;
            default:
                ImGui::Text(
                    "An unknown error occurred.");
                break;
            }

            if (ImGui::Button("Ok"))
            {
                m_error = Error::Success;
            }
        }
    }

    auto ModManager::load_sig_path() -> void
    {
        auto path = std::filesystem::path(UE4SSProgram::get_program().get_game_executable_directory());
        m_sig = path.parent_path().parent_path() / "Content" / "Paks" / "pakchunk0-WindowsNoEditor.sig";
    }

    auto ModManager::init() -> void
    {
        load_config();
        load_sig_path();
        enum DtorPattern
        {
            V425,
            V427,
        };
        SignatureContainer fplatformfilepak_dtor = [&]() -> SignatureContainer
        {
            return {
                {
                    {
                        .signature = "40 56 48 83 EC 30 48 89 5C 24 ?? 48 8D 05 ?? ?? ?? ?? 48 89 7C 24",
                        .custom_data = V425,
                    },
                    {
                        .signature = "48 89 5C 24 ? 56 57 41 56 48 83 EC ? 48 8D 05 ? ? ? ? 4C 89 7C 24",
                        .custom_data = V427,
                    },
                },
                [&](SignatureContainer& self)
                {
                    Output::send<LogLevel::Warning>(
                        STR("[ModManager]: found match: {}\n"), (void*)self.get_match_address());

                    int num = 0;
                    const auto signature_identifier = static_cast<const DtorPattern>(self.get_signatures()[self.
                        get_index_into_signatures()].custom_data);
                    switch (signature_identifier)
                    {
                    case DtorPattern::V425:
                        num = 1;
                        break;
                    case DtorPattern::V427:
                        num = 0;
                        break;
                    }

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
                                case 2:
                                    MountPak = *(DelegateMount**)ptr;
                                    break;
                                case 3:
                                    OnUnmountPak = *(DelegateUnmount**)ptr;
                                    self.get_did_succeed() = true;
                                    load_mods();
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
        m_config.m_is_open = !m_config.m_is_open;
        save_config();
    }

    auto ModManager::load_config() -> void
    {
        std::ignore = glz::read_file_json(m_config, "./mod_manager.json", std::string{});
    }

    auto ModManager::save_config() -> void
    {
        std::ignore = glz::write_file_json(m_config, "./mod_manager.json", std::string{});
    }

    auto ModManager::load_mods() -> void
    {
        if (std::error_code ec; !std::filesystem::exists(m_config.m_mods_folder, ec))
        {
            m_config.m_mods_folder = "";
        }
        if (m_mods_loaded || m_config.m_mods_folder.empty()) return;

        save_config();

        std::vector<ModEntry> enabled_mods;
        std::vector<ModEntry> disabled_mods;

        for (const auto& entry : m_config.m_mods)
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
                if (auto sig_name = (dir_entry.path().parent_path() / dir_entry.path().stem()).string() + ".sig";
                    !std::filesystem::exists(sig_name))
                {
                    try
                    {
                        copy(m_sig, sig_name);
                    }
                    catch (const std::filesystem::filesystem_error& e)
                    {
                        Output::send<LogLevel::Error>(
                            std::format(
                                STR("Failed to copy sig to {}: {}\n"), to_wstring(sig_name), to_wstring(e.what())));
                    }
                }

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
