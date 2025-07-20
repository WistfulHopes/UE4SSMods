#include "REDGameState_CharaSelectRE.h"

#include "ObjectConstructor.h"

IMPLEMENT_EXTERNAL_OBJECT_CLASS(AGameStateBase);
IMPLEMENT_EXTERNAL_OBJECT_CLASS(AREDGameState);
IMPLEMENT_EXTERNAL_OBJECT_CLASS(AREDGameState_CharaSelectRE);

FDynamicClass AREDGameState_CharaSelectRE::Class = {
    .Ctor = &AREDGameState_CharaSelectRE::AREDGameState_CharaSelectRE_Ctor,
};

void AREDGameState_CharaSelectRE::AREDGameState_CharaSelectRE_Ctor(const FObjectInitializer* ObjectInitializer)
{
    auto Super = StaticClass()->GetSuperClass();
    uintptr_t CtorAddr = (uintptr_t)Super->GetClassConstructor() + 0xE;
    CtorAddr = *(int*)(CtorAddr + 0x1) + (CtorAddr + 0x5);
    auto Ctor = reinterpret_cast<void(*)(UObject*, const FObjectInitializer*)>(CtorAddr);
    Ctor(ObjectInitializer->Obj, ObjectInitializer);
}
