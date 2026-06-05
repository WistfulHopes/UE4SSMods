#include "TestObject.h"
#include "MiscMacros.hpp"

IMPLEMENT_EXTERNAL_OBJECT_CLASS(UTestObject);

FDynamicClass UTestObject::Data = {
    .Ctor = &UTestObject::UTestObject_Ctor,
};

void UTestObject::UTestObject_Ctor(const SuzieNamespace::FObjectInitializer* ObjectInitializer)
{
    CALL_BASE_CTOR()
}

void UTestObject::InitializeClass()
{
    Data.ClassDefaultObjectPath = FString(STR("/Script/UNIONExtend.Default__TestObject"));
    Data.SuperStruct = FString(STR("/Script/CoreUObject.Object"));
}