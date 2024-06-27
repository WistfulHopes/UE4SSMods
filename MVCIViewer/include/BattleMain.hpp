#pragma once
#include "AActor.hpp"
#include "struct_util.hpp"
#include "UnrealDef.hpp"

namespace nBattle
{
    struct sWork;
    struct sPlayer;
}

inline bool IsValidUObject(RC::Unreal::UObject* Object) {
    //return Object && !Object->IsUnreachable(); 
    return Object && RC::Unreal::UObjectArray::IsValid(Object->GetObjectItem(), false);
}

class ABattleMain : public Unreal::AActor
{
    static inline UClass* Class = nullptr;
    static inline ABattleMain* Instance = nullptr;

public:
    static ABattleMain* Get()
    {
        if (Class == nullptr)
            Class = RC::UObjectGlobals::StaticFindObject<UClass*>(nullptr, nullptr, STR("/Script/Duck.BattleMain"));
	
        if (!IsValidUObject(Instance))
        {
            static auto game_state_name = Unreal::FName(STR("BattleMain"), FNAME_Add);
            Instance = (ABattleMain*)Unreal::UObjectGlobals::FindFirstOf(game_state_name);
        }

        if (!IsValidUObject(Instance)) return nullptr;
        if (!Instance->IsA(Class)) return nullptr;
        
        return Instance;
    }
    static void Reset()
    {
        Instance = nullptr;
    }

    FIELD(0x3A0, nBattle::sPlayer*, Player);
    FIELD(0x3B0, nBattle::sWork*, Work);
    ARRAY_FIELD(0x0428, UCharacterAsset*[6], mpCharacterAsset);
};
