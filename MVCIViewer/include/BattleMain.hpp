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

class ABattleMain : public RC::Unreal::AActor
{
    static inline RC::UClass* Class = nullptr;
    static inline ABattleMain* Instance = nullptr;

public:
    static ABattleMain* Get()
    {
        if (Class == nullptr)
            Class = RC::UObjectGlobals::StaticFindObject<RC::UClass*>(nullptr, nullptr, STR("/Script/Duck.BattleMain"));
	
        if (!IsValidUObject(Instance))
        {
            static auto game_state_name = RC::Unreal::FName(STR("BattleMain"), RC::Unreal::FNAME_Add);
            Instance = (ABattleMain*)RC::Unreal::UObjectGlobals::FindFirstOf(game_state_name);
        }

        if (Instance == nullptr) return nullptr;
        if (!Instance->IsA(Class)) return nullptr;
        
        return Instance;
    }

    FIELD(0x3A0, nBattle::sPlayer*, Player);
    FIELD(0x3B0, nBattle::sWork*, Work);
    ARRAY_FIELD(0x0428, UCharacterAsset*[6], mpCharacterAsset);
};
