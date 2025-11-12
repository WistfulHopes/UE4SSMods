#include "REDGameMode_CharaSelectRE.h"

#include "REDGameState_CharaSelectRE.h"
#include "../../SuzieMod/include/MiscMacros.hpp"

IMPLEMENT_EXTERNAL_OBJECT_CLASS(AREDGameMode);
IMPLEMENT_EXTERNAL_OBJECT_CLASS(AREDGameMode_CharaSelectRE);

FDynamicClass AREDGameMode_CharaSelectRE::Data = {
    .Ctor = &AREDGameMode_CharaSelectRE::AREDGameMode_CharaSelectRE_Ctor,
};

void AREDGameMode_CharaSelectRE::AREDGameMode_CharaSelectRE_Ctor(const FObjectInitializer* ObjectInitializer)
{
    CALL_BASE_CTOR()

    auto CastContext = static_cast<AREDGameMode_CharaSelectRE*>(ObjectInitializer->Obj);
    
    CastContext->GameStateClass = AREDGameState_CharaSelectRE::StaticClass();
    CastContext->PlayerControllerClass = UObjectGlobals::FindObject<UClass>(nullptr, STR("/Script/RED.REDPlayerController_NoMove"));
    CastContext->HUDClass = UObjectGlobals::FindObject<UClass>(nullptr, STR("/Script/RED.REDHUD"));
    CastContext->DefaultPawnClass = UObjectGlobals::FindObject<UClass>(nullptr, STR("/Script/RED.REDPawnCamera"));
}

void AREDGameMode_CharaSelectRE::InitializeClass()
{
    Data.ClassDefaultObjectPath = FString(STR("/Script/REDExtend.Default__REDGameMode_CharaSelectRE"));
    Data.SuperStruct = FString(STR("/Script/RED.REDGameMode"));
}