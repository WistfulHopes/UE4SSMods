#pragma once

#include "AGameModeBase.hpp"
#include "struct_util.h"
#include "../../SuzieMod/include/Suzie.hpp"

using CharType = wchar_t;
using StringType = std::basic_string<CharType>;

class AREDGameMode : public AGameModeBase
{
    DECLARE_EXTERNAL_OBJECT_CLASS(AREDGameMode, RED)
    
private:
    char __padding[0x310]{};

public:
    FIELD(0x240, UClass*, GameStateClass);
    FIELD(0x248, UClass*, PlayerControllerClass);
    FIELD(0x258, UClass*, HUDClass);
    FIELD(0x260, UClass*, DefaultPawnClass);
};

class AREDGameMode_CharaSelectRE : public AREDGameMode
{
    DECLARE_EXTERNAL_OBJECT_CLASS(AREDGameMode_CharaSelectRE, REDExtend)

public:
    static void AREDGameMode_CharaSelectRE_Ctor(const FObjectInitializer* ObjectInitializer);
    static void InitializeClass();
    
    static FDynamicClass Class;
};