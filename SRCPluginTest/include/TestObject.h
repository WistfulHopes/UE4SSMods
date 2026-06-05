#pragma once

#include "../../SuzieMod/include/Suzie.hpp"

using CharType = wchar_t;
using StringType = std::basic_string<CharType>;

class UTestObject : public UObject
{
    DECLARE_EXTERNAL_OBJECT_CLASS(UTestObject, UNIONExtend)
    
private:
    char __padding[0x28]{};

public:
    static void UTestObject_Ctor(const SuzieNamespace::FObjectInitializer* ObjectInitializer);
    static void InitializeClass();
    
    static FDynamicClass Data;
};