#include "REDGameMode_CharaSelectRE.h"

#include "REDGameState_CharaSelectRE.h"

IMPLEMENT_EXTERNAL_OBJECT_CLASS(AREDGameMode);
IMPLEMENT_EXTERNAL_OBJECT_CLASS(AREDGameMode_CharaSelectRE);

FDynamicClass AREDGameMode_CharaSelectRE::Class = {
    .Ctor = &AREDGameMode_CharaSelectRE::AREDGameMode_CharaSelectRE_Ctor,
};

void AREDGameMode_CharaSelectRE::AREDGameMode_CharaSelectRE_Ctor(const FObjectInitializer* ObjectInitializer)
{
    auto Super = StaticClass()->GetSuperClass();
    uintptr_t CtorAddr = reinterpret_cast<uintptr_t>(Super->GetClassConstructor()) + 0xE;
    CtorAddr = *reinterpret_cast<int*>(CtorAddr + 0x1) + (CtorAddr + 0x5);
    auto Ctor = reinterpret_cast<void(*)(UObject*, const FObjectInitializer*)>(CtorAddr);
    Ctor(ObjectInitializer->Obj, ObjectInitializer);

    auto CastContext = static_cast<AREDGameMode_CharaSelectRE*>(ObjectInitializer->Obj);
    
    CastContext->GameStateClass = AREDGameState_CharaSelectRE::StaticClass();
    CastContext->PlayerControllerClass = UObjectGlobals::FindObject<UClass>(nullptr, STR("/Script/RED.REDPlayerController_NoMove"));
    CastContext->HUDClass = UObjectGlobals::FindObject<UClass>(nullptr, STR("/Script/RED.REDHUD"));
    CastContext->DefaultPawnClass = UObjectGlobals::FindObject<UClass>(nullptr, STR("/Script/RED.REDPawnCamera"));
}

void AREDGameMode_CharaSelectRE::InitializeClass()
{
    Class.ClassDefaultObjectPath = FString(STR("/Script/REDExtend.Default__REDGameMode_CharaSelectRE"));
    Class.SuperStruct = FString(STR("/Script/RED.REDGameMode"));
}