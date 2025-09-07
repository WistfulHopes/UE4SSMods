#include <string>

#include <Mod/CppUserModBase.hpp>
#include <UE4SSProgram.hpp>

#include "Hooks.hpp"
#include "Suzie.hpp"

class SuzieMod : public RC::CppUserModBase
{
private:
    static inline bool bInitialized = false;
    
public:
    SuzieMod() : CppUserModBase()
    {
        ModName = STR("Suzie");
        ModVersion = STR("1.0");
        ModDescription = STR("Dynamic class injection");
        ModAuthors = STR("WistfulHopes");
        
        Suzie::StaticInitialize();
    }

    ~SuzieMod() override = default;

    auto on_unreal_init() -> void override
    {
        Unreal::Hook::RegisterProcessEventPreCallback(&process_event_hook);
    }
    
    static auto process_event_hook([[maybe_unused]] UObject* Context, [[maybe_unused]] UFunction* Function, [[maybe_unused]] void* Parms) -> void
    {
        if (bInitialized) return;
        bInitialized = true;
        Suzie::GetInstance()->Initialize();
    }
};

#define SUZIE_MOD_API __declspec(dllexport)
extern "C"
{
    SUZIE_MOD_API RC::CppUserModBase* start_mod()
    {
        return new SuzieMod();
    }

    SUZIE_MOD_API void uninstall_mod(RC::CppUserModBase* mod)
    {
        delete mod;
    }

    EXTERN_C SUZIE_MOD_API void InsertClass(const FString& Path, FDynamicClass* Class)
    {
        Suzie::GetInstance()->InsertClass(Path, Class);
    }

    EXTERN_C SUZIE_MOD_API void InsertStruct(const FString& Path, FDynamicScriptStruct* Struct)
    {
        Suzie::GetInstance()->InsertStruct(Path, Struct);
    }

    EXTERN_C SUZIE_MOD_API void InsertEnum(const FString& Path, FDynamicEnum* Enum)
    {
        Suzie::GetInstance()->InsertEnum(Path, Enum);
    }

    EXTERN_C SUZIE_MOD_API void InsertFunction(const FString& Path, FDynamicFunction* Function)
    {
        Suzie::GetInstance()->InsertFunction(Path, Function);
    }
}

