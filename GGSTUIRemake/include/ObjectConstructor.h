#pragma once

#include "UPackage.hpp"
#include "UObjectGlobals.hpp"
#include "../../SuzieMod/include/Suzie.hpp"

#define DEFINE_DEFAULT_CONSTRUCTOR_CALL(TClass) \
	inline void* operator new(const size_t InSize, EInternal InInternalOnly, UObject* InOuter = (UObject*)GetTransientPackage(), FName InName = NAME_None, EObjectFlags InSetFlags = RF_NoFlags) \
	{ \
		return UObjectGlobals::StaticAllocateObject(StaticClass(), InOuter, InName, InSetFlags); \
	} \
	inline void* operator new( const size_t InSize, EInternal* InMem ) \
	{ \
		return (void*)InMem; \
	} \
	static void __DefaultConstructor(const FObjectInitializer* X) { new((EInternal*)X->Obj)TClass(X); }

template<class T>
void InternalConstructor( const FObjectInitializer* X )
{ 
	T::__DefaultConstructor(X);
}
