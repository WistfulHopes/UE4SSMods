#include <Mod/CppUserModBase.hpp>
#include <UE4SSProgram.hpp>
#include <DotNetLibrary.hpp>

using namespace RC::Unreal;

class CSharpLoaderProxy : public CppUserModBase
{
public:
    DotNetLibrary::Runtime* m_runtime;
    
    CSharpLoaderProxy()
    {
        ModName = STR("CSharpLoaderProxy");
        ModVersion = STR("1.0");
        ModDescription = STR("Replaces DBM values");
        ModAuthors = STR("WistfulHopes");
        auto runtime_path = std::wstring(UE4SSProgram::get_program().get_mods_directory()) + L"\\DotNetRuntime";
        m_runtime = new DotNetLibrary::Runtime(runtime_path);
        // Do not change this unless you want to target a UE4SS version
        // other than the one you're currently building with somehow.
        //ModIntendedSDKVersion = STR("2.6");
    }

    ~CSharpLoaderProxy() override
    {
        m_runtime->unload_assemblies();
        delete m_runtime;
    }

    auto on_dll_load(std::wstring_view dll_name) -> void override
    {
        auto runtime_path = std::wstring(UE4SSProgram::get_program().get_mods_directory()) + L"\\DotNetRuntime";
        LoadLibrary((runtime_path + L"\\CSharpLoader.dll").c_str());
        m_runtime->initialize();
    }
    
    auto on_program_start() -> void override
    {
        m_runtime->fire_program_start();
    }

    auto on_unreal_init() -> void override
    {
        m_runtime->fire_unreal_init();
    }

    auto on_update() -> void override
    {
        m_runtime->fire_update();
    }
};

#define CSHARPLOADERPROXY_API __declspec(dllexport)
extern "C"
{
    CSHARPLOADERPROXY_API CppUserModBase* start_mod()
    {
        return new CSharpLoaderProxy();
    }

    CSHARPLOADERPROXY_API void uninstall_mod(CppUserModBase* mod)
    {
        delete mod;
    }
}