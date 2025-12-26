#include <Mod/CppUserModBase.hpp>

#include "UE4SSProgram.hpp"

class GGSTLocalizeTextMod : public CppUserModBase
{
public:
    GGSTLocalizeTextMod()
    {
        ModName = STR("GGST Localize Text Mod");
        ModVersion = STR("1.0");
        ModDescription = STR("Mod to add localized text.");
        ModAuthors = STR("UE4SS Team");
        // Do not change this unless you want to target a UE4SS version
        // other than the one you're currently building with somehow.
        //ModIntendedSDKVersion = STR("2.6");
    }

    ~GGSTLocalizeTextMod() override
    {
    }

    auto on_unreal_init() -> void override
    {
        const SignatureContainer AddStaticLocalizeText{
            {{"40 55 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 ? 4C 8B C2"}},
            [&](const SignatureContainer& self)
            {
                Function<Unreal::int32(const void*, SIZE_T)> Func;
                Func.assign_address(self.get_match_address());
                
                auto path = std::filesystem::path(UE4SSProgram::get_program().get_game_executable_directory());
                auto text_directory = path.parent_path().parent_path() / "Localization";
                if (!std::filesystem::exists(text_directory))
                {
                    Output::send(STR("No text directory found...\n"));
                }
                for (const auto& entry : std::filesystem::directory_iterator(text_directory))
                {
                    if (entry.path().extension() != ".loc")
                        continue;
                    
                    std::ifstream file(entry.path(), std::ios::in);

                    if (file.fail())
                        continue;
                    
                    file.seekg(0, file.end);
                    const size_t size = file.tellg();
                    file.seekg(0, file.beg);

                    const auto buffer = new char[size];
                    
                    file.read(buffer, size);
                    file.close();
                    
                    std::wstring wide = to_wstring(std::string(buffer));
                    
                    Unreal::TArray data((Unreal::uint8*)wide.c_str(), wide.size() * 2);
                    data.Insert({0xFF, 0xFE}, 0);

                    Func(data.GetData(), data.Num());

                    Output::send(std::format(STR("Read text at {}.\n"), to_wstring(entry.path().string())));
                }
                return true;
            },
            [](SignatureContainer& self)
            {
                
            },
        };
        
        std::vector<SignatureContainer> signature_containers;
        signature_containers.push_back(AddStaticLocalizeText);
        
        SinglePassScanner::SignatureContainerMap signature_containers_map;
        signature_containers_map.emplace(ScanTarget::MainExe, signature_containers);

        SinglePassScanner::start_scan(signature_containers_map);
    }

    auto on_update() -> void override
    {
    }
};

#define GGST_LOCALIZE_TEXT_MOD_API __declspec(dllexport)
extern "C"
{
    GGST_LOCALIZE_TEXT_MOD_API CppUserModBase* start_mod()
    {
        return new GGSTLocalizeTextMod();
    }

    GGST_LOCALIZE_TEXT_MOD_API void uninstall_mod(CppUserModBase* mod)
    {
        delete mod;
    }
}
