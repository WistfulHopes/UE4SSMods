#pragma once

#include "AActor.hpp"
#include "../../SuzieMod/include/Suzie.hpp"
#include "struct_util.h"

using namespace RC::Unreal;

using CharType = wchar_t;
using StringType = std::basic_string<CharType>;

class AGameStateBase : public AActor
{
    DECLARE_EXTERNAL_OBJECT_CLASS(AGameStateBase, RED)

private:
    char _padding[0x278];
};

class AREDGameState : public AGameStateBase
{
    DECLARE_EXTERNAL_OBJECT_CLASS(AREDGameState, RED)

private:
    char _padding[0xAE8 - 0x278];

protected:
    FIELD(0x378, AActor*, PrimaryPC);
    
public:
    static inline RC::Function<void(AREDGameState*)> BeginPlay_Func;
    static inline RC::Function<void(AREDGameState*)> HandleMatchIsWaitingToStart_Func;
    static inline RC::Function<void(AREDGameState*)> SceneInitialize_Func;
    static inline RC::Function<void(AActor*, int32)> AREDHUD_FadeIn_Func;
};