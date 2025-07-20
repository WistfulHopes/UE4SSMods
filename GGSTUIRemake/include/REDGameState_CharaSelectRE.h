#pragma once

#include "AActor.hpp"
#include "ObjectConstructor.h"
#include "struct_util.h"
#include "../../SuzieMod/include/Suzie.hpp"

using namespace RC::Unreal;

using CharType = wchar_t;
using StringType = std::basic_string<CharType>;

class AGameStateBase : public AActor
{
    DECLARE_EXTERNAL_OBJECT_CLASS(AGameStateBase, RED)

    char _padding[0x278];
};

class AREDGameState : public AGameStateBase
{
    DECLARE_EXTERNAL_OBJECT_CLASS(AREDGameState, RED)

    char _padding[0xAE8 - 0x278];
};

class AREDGameState_CharaSelectRE : public AREDGameState
{
    DECLARE_EXTERNAL_OBJECT_CLASS(AREDGameState_CharaSelectRE, REDExtend)
    
public:
    static void AREDGameState_CharaSelectRE_Ctor(const FObjectInitializer* ObjectInitializer);

    static FDynamicClass Class;
};