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
        
        wchar_t path[MAX_PATH];
        HMODULE hm = NULL;

        if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | 
        GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCWSTR>(&empty), &hm) == 0)
        {
            Output::send<LogLevel::Error>(STR("GetModuleHandle failed, error = {}\n"), GetLastError());
            return;
        }
        if (GetModuleFileName(hm, path, sizeof(path)) == 0)
        {
            Output::send<LogLevel::Error>(STR("GetModuleFileName failed, error = {}\n"), GetLastError());
            return;
        }

        const auto module_path = std::filesystem::path(path).parent_path();
        if (!LoadLibrary((std::wstring(module_path.c_str()) + L"\\CSharpLoader.dll").c_str()))
        {
            Output::send<LogLevel::Error>(STR("LoadLibrary failed, error = {}\n"), GetLastError());
            return;
        }

        std::filesystem::path runtime_path(module_path);
        runtime_path = runtime_path.parent_path() / "DotNetRuntime";
        
        m_runtime = new DotNetLibrary::Runtime(runtime_path);
        m_runtime->initialize();
        
        // Do not change this unless you want to target a UE4SS version
        // other than the one you're currently building with somehow.
        //ModIntendedSDKVersion = STR("2.6");
    }

    ~CSharpLoaderProxy() override
    {
        m_runtime->unload_assemblies();
        delete m_runtime;
    }

    static auto empty() -> void
    {
        
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