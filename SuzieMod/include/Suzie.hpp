#pragma once

#include <Unreal/CoreUObject/UObject/Class.hpp>

using namespace RC::Unreal;

struct FDynamicProperty
{
    EPropertyFlags Flags {};
    FString Name {};
    FString Type {};
    int32 ArrayDim {};
    int32 Offset {};
    FString Class {};
    FString MetaClass {};
    FString Struct {};
    FString Enum {};
    std::shared_ptr<FDynamicProperty> Container {};
    struct FDynamicFunction* SignatureFunction {};
    std::shared_ptr<FDynamicProperty> Inner {};
    std::shared_ptr<FDynamicProperty> Key {};
    std::shared_ptr<FDynamicProperty> Value {};
};

struct FDynamicFunction
{
    FString Path {};
    void(* Func)(UObject*, FFrame&, void*) {};
    EFunctionFlags Flags {};
    TArray<FDynamicProperty> Properties {};
};

struct FDynamicObject
{
    FString Path {};
    EObjectFlags Flags {};
};

namespace SuzieNamespace
{
    class FObjectInitializer
    {
    public:
        UObject* Obj {};
    };
}

struct FDynamicClass : FDynamicObject
{
    FString ClassDefaultObjectPath {};
    FString SuperStruct {};
    TArray<FDynamicProperty> Properties {};
    TArray<FDynamicFunction> Functions {};
    TArray<FDynamicObject> Children {};
    EClassFlags ClassFlags {};
    void (*Ctor)(const SuzieNamespace::FObjectInitializer*) {};
};

struct FDynamicScriptStruct : FDynamicObject
{
    FString SuperStruct {};
    EStructFlags StructFlags {};
    TArray<FDynamicProperty> Properties {};
};

struct FDynamicEnum : FDynamicObject
{
    FString CppType {};
    TMap<FString, int64> Pairs {};
};

struct FDynamicClassGenerationContext
{
    TMap<UClass*, FString> ClassesPendingConstruction {};
    TMap<UClass*, FString> ClassesPendingFinalization {};
};

struct FDynamicObjectConstructionData
{
    FName ObjectName {};
    UClass* ObjectClass{};
    EObjectFlags ObjectFlags{};
};

struct FNestedDefaultSubobjectOverrideData
{
    TArray<FName> SubobjectPath;
    UClass* OverridenClass{};
};

struct FDynamicClassConstructionData
{
    // List of properties (not including super class properties) that must be constructed with InitializeValue call
    TArray<const FProperty*> PropertiesToConstruct {};
    // Names of default subobjects that our native parent class defines but that we do not want to be created
    TArray<FName> SuppressedDefaultSubobjects {};
    // Note that this will also contain all subobjects defined in parent classes
    TArray<FDynamicObjectConstructionData> DefaultSubobjects {};
    // Overrides for nested default subobjects. Note that top level subobjects will not be included here
    TArray<FNestedDefaultSubobjectOverrideData> DefaultSubobjectOverrides {};
    // Archetype to use for constructing the object when no archetype has been provided or the provided archetype was a CDO
    UObject* DefaultObjectArchetype{};
};

struct FDynamicClassConstructionIntermediates
{
    UObject* ConstructedObject{};
    const FDynamicClassConstructionData* ConstructionData{};
    UObject* ArchetypeObject{};
    TMap<UObject*, UObject*> TemplateToSubobjectMap;
};

class Suzie
{
public:    
    void InsertClass(const FString& Path, FDynamicClass* Class);
    void InsertStruct(const FString& Path, FDynamicScriptStruct* Struct);
    void InsertEnum(const FString& Path, FDynamicEnum* Enum);
    void InsertFunction(const FString& Path, FDynamicFunction* Function);

    static void StaticInitialize();
    static Suzie* GetInstance();

    void Initialize();

private:
    TMap<FString, FDynamicClass*> DynamicClasses {};
    TMap<FString, FDynamicScriptStruct*> DynamicStructs {};
    TMap<FString, FDynamicEnum*> DynamicEnums {};
    TMap<FString, FDynamicFunction*> DynamicFunctions {};
    
    UPackage* FindOrCreatePackage(const FString& PackageName);
    UClass* FindOrCreateUnregisteredClass(FDynamicClassGenerationContext& Context, const FString& ClassPath);
    UClass* FindOrCreateClass(FDynamicClassGenerationContext& Context, const FString& ClassPath);
    UScriptStruct* FindOrCreateScriptStruct(FDynamicClassGenerationContext& Context, const FString& StructPath);
    UEnum* FindOrCreateEnum(FDynamicClassGenerationContext& Context, const FString& EnumPath);
    UFunction* FindOrCreateFunction(FDynamicClassGenerationContext& Context, const FDynamicFunction& Function);
    UFunction* FindOrCreateFunction(FDynamicClassGenerationContext& Context, const FString& FunctionPath);

    UClass* UClass_Ctor(UClass* ParentClass, EClassFlags ClassFlags);
    
    bool ParseObjectConstructionData(const FDynamicClassGenerationContext& Context, const FString& ObjectPath, FDynamicObjectConstructionData& ObjectConstructionData);
    void CollectNestedDefaultSubobjectTypeOverrides(FDynamicClassGenerationContext& Context,
                                                    TArray<FName> SubobjectNameStack, const FString& SubobjectPath,
                                                    TArray<FNestedDefaultSubobjectOverrideData>&
                                                    OutSubobjectOverrideData);
    void FinalizeClass(FDynamicClassGenerationContext& Context, UClass* Class);

    static void ParseObjectPath(const FString& ObjectPath, FString& OutOuterObjectPath, FString& OutObjectName);

    FProperty* AddPropertyToStruct(FDynamicClassGenerationContext& Context, UStruct* Struct,
                                   const FDynamicProperty& Property, EPropertyFlags ExtraPropertyFlags = CPF_None);
    void AddFunctionToClass(FDynamicClassGenerationContext& Context, UClass* Class, const FDynamicFunction& Function,
                            EFunctionFlags ExtraFunctionFlags = FUNC_None);

    FProperty* BuildProperty(FDynamicClassGenerationContext& Context, FFieldVariant Owner,
                             const FDynamicProperty& Property, EPropertyFlags ExtraPropertyFlags = CPF_None);

    UScriptStruct* GetSparseClassDataArchetypeStruct(UClass* Context);

    void Create();
    
    static inline Suzie* Instance;
};
