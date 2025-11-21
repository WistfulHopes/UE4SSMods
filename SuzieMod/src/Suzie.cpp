#include "Suzie.hpp"

#include <Unreal/Property/FEnumProperty.hpp>
#include <Unreal/Property/FFieldPathProperty.hpp>
#include "UInterface.hpp"
#include "UPackage.hpp"

using namespace RC;

// Note that new objects can be created from other threads, but we only touch this map when creating dynamic classes,
// so we do not need an explicit mutex to guard the access to it during class initialization
namespace
{
    TMap<UClass*, FDynamicClassConstructionData> DynamicClassConstructionData;
}

// Note that new objects can be created from other threads, but we only touch this map when creating dynamic classes,
// so we do not need an explicit mutex to guard the access to it during class initialization
UClass* Suzie::FindOrCreateClass(FDynamicClassGenerationContext& Context, const FString& ClassPath)
{
    // Return existing class if exists
    UClass* NewClass = UObjectGlobals::FindObject<UClass>(nullptr, *ClassPath);

    // If class already exists and is not pending constructed, we do not need to do anything
    if (NewClass && !Context.ClassesPendingConstruction.Contains(NewClass))
    {
        return NewClass;
    }

    // If we have not created the class yet, create it now
    if (NewClass == nullptr)
    {
        NewClass = FindOrCreateUnregisteredClass(Context, ClassPath);
        if (NewClass == nullptr)
        {
            Output::send<LogLevel::Error>(std::format(STR("Failed to create dynamic class: {}\n"), *ClassPath));
            return nullptr;
        }
    }

    auto Class = DynamicClasses[ClassPath];

    // Remove the class from the pending construction set to prevent possible re-entry
    Context.ClassesPendingConstruction.Remove(NewClass);

    TArray<const FProperty*> PropertiesWithDestructor;
    TArray<const FProperty*> PropertiesWithConstructor;
    uintptr_t* EmptyPropertyLinkArchive = (uintptr_t*)FMemory::Malloc(0x90);

    struct FArchive_vtbl
    {
        char _padding[0x170];
        void* Preload;
    };

    FArchive_vtbl* vtbl = (FArchive_vtbl*)FMemory::Malloc(0x178);

    vtbl->Preload = []
    {
    };
    *EmptyPropertyLinkArchive = (uintptr_t)vtbl;

    // Add properties to the class
    for (auto& Property : Class->Properties)
    {
        // We want all properties to be editable, visible and blueprint assignable
        const EPropertyFlags ExtraPropertyFlags = CPF_Edit | CPF_BlueprintVisible | CPF_BlueprintAssignable;
        if (FProperty* CreatedProperty = AddPropertyToStruct(Context, NewClass, Property, ExtraPropertyFlags))
        {
            // Because this is a native class, we have to link the property offset manually here rather than expecting StaticLink to do it for us
            CreatedProperty->Link(*(FArchive*)EmptyPropertyLinkArchive);
            NewClass->GetMinAlignment() = FMath::Max(NewClass->GetMinAlignment(), CreatedProperty->GetMinAlignment());

            // Add property into the constructor/destructor lists based on its flags
            if (!CreatedProperty->HasAnyPropertyFlags(CPF_IsPlainOldData | CPF_NoDestructor))
            {
                PropertiesWithDestructor.Add(CreatedProperty);
            }
            if (!CreatedProperty->HasAnyPropertyFlags(CPF_ZeroConstructor))
            {
                PropertiesWithConstructor.Add(CreatedProperty);
            }
        }
    }

    // Add functions to the class
    for (const auto& Function : Class->Functions)
    {
        AddFunctionToClass(Context, NewClass, Function);
    }

    // Bind parent class to this class and link properties to calculate their runtime derived data
    NewClass->Bind();
    NewClass->Link(*(FArchive*)EmptyPropertyLinkArchive, true);
    if (Unreal::Version::IsAtLeast(4, 24))
    {
        NewClass->GetSparseClassDataStruct() = GetSparseClassDataArchetypeStruct(NewClass);
    }

    FMemory::Free(EmptyPropertyLinkArchive);
    FMemory::Free(vtbl);

    // Stash the properties that need to be constructed on the class data so polymorphic constructor can access them easily
    FDynamicClassConstructionData& ClassConstructionData = DynamicClassConstructionData.FindOrAdd(NewClass);
    ClassConstructionData.PropertiesToConstruct = PropertiesWithConstructor;

    const FString ClassDefaultObjectPath = Class->ClassDefaultObjectPath;

    // Class default object can be created at this point
    Context.ClassesPendingFinalization.Add(NewClass, ClassDefaultObjectPath);

    return NewClass;
}

UScriptStruct* Suzie::FindOrCreateScriptStruct(FDynamicClassGenerationContext& Context, const FString& StructPath)
{
    // Check if we have already created this struct
    if (UScriptStruct* ExistingScriptStruct = UObjectGlobals::FindObject<UScriptStruct>(nullptr, *StructPath))
    {
        return ExistingScriptStruct;
    }

    const auto Struct = DynamicStructs[StructPath];

    // Resolve parent struct for this struct before we attempt to create this struct
    UScriptStruct* SuperScriptStruct = nullptr;
    FString ParentStructPath = Struct->SuperStruct;
    if (ParentStructPath.Len())
    {
        SuperScriptStruct = FindOrCreateScriptStruct(Context, ParentStructPath);
        if (SuperScriptStruct == nullptr)
        {
            return nullptr;
        }
    }

    FString PackageName;
    FString ObjectName;
    ParseObjectPath(StructPath, PackageName, ObjectName);

    // Create a package for the struct or reuse the existing package. Make sure it's marked as Native package
    UPackage* Package = FindOrCreatePackage(PackageName);

    UScriptStruct* NewStruct = UObjectGlobals::NewObject<UScriptStruct>(Package, FName(*ObjectName, FNAME_Add),
                                                                        (EObjectFlags)(RF_Public | RF_MarkAsRootSet));

    // Set super script struct and copy inheritable flags first if this struct has a parent (most structs do not)
    if (SuperScriptStruct != nullptr)
    {
        NewStruct->SetSuperStruct(SuperScriptStruct);
        NewStruct->GetStructFlags() = (EStructFlags)((int32)NewStruct->GetStructFlags() | (SuperScriptStruct->
            GetStructFlags() & STRUCT_Inherit) | Struct->StructFlags);
    }

    for (const auto& Property : Struct->Properties)
    {
        // We want all properties to be editable, visible and blueprint assignable
        const EPropertyFlags ExtraPropertyFlags = CPF_Edit | CPF_BlueprintVisible | CPF_BlueprintAssignable;
        AddPropertyToStruct(Context, NewStruct, Property, ExtraPropertyFlags);
    }

    uintptr_t* EmptyPropertyLinkArchive = (uintptr_t*)FMemory::Malloc(0x90);

    struct FArchive_vtbl
    {
        char _padding[0x170];
        void* Preload;
    };

    FArchive_vtbl* vtbl = (FArchive_vtbl*)FMemory::Malloc(0x178);

    vtbl->Preload = []
    {
    };
    *EmptyPropertyLinkArchive = (uintptr_t)vtbl;

    // Bind the newly created struct and link it to assign property offsets and calculate the size
    NewStruct->Bind();
    NewStruct->Link(*(FArchive*)EmptyPropertyLinkArchive, true);

    FMemory::Free(EmptyPropertyLinkArchive);
    FMemory::Free(vtbl);

    // The engine does not gracefully handle empty structs, so force the struct size to be at least one byte
    if (NewStruct->GetPropertiesSize() == 0)
    {
        NewStruct->GetMinAlignment() = 1;
        NewStruct->GetPropertiesSize() = 1;
    }

    // Struct properties using this struct can be created at this point
    return NewStruct;
}

UEnum* Suzie::FindOrCreateEnum(FDynamicClassGenerationContext& Context, const FString& EnumPath)
{
    // Check if we have already created this enum
    if (UEnum* ExistingEnum = UObjectGlobals::FindObject<UEnum>(nullptr, *EnumPath))
    {
        return ExistingEnum;
    }

    if (!DynamicEnums.Contains(EnumPath)) return nullptr;
    const auto EnumDefinition = DynamicEnums[EnumPath];

    FString PackageName;
    FString ObjectName;
    ParseObjectPath(EnumPath, PackageName, ObjectName);

    // Create a package for the struct or reuse the existing package. Make sure it's marked as Native package
    UPackage* Package = FindOrCreatePackage(PackageName);

    UEnum* NewEnum = UObjectGlobals::NewObject<UEnum>(Package, FName(*ObjectName, FNAME_Add),
                                                      EObjectFlags(RF_Public | RF_MarkAsRootSet));

    // Set CppType. It is generally not used by the engine, but is useful to determine whenever enum is namespaced or not for CppForm deduction
    NewEnum->GetCppType() = EnumDefinition->CppType;

    TArray<TPair<FName, int64>> EnumNames;
    bool bContainsFullyQualifiedNames = false;

    // Parse enum constant names and values
    for (const auto& Pair : EnumDefinition->Pairs)
    {
        const FString EnumConstantName = Pair.Key;
        const int64 EnumConstantValue = Pair.Value;

        EnumNames.Add({FName(*EnumConstantName, FNAME_Add), EnumConstantValue});
        bContainsFullyQualifiedNames |= std::wstring(EnumConstantName.GetCharArray().GetData()).contains(TEXT("::"));
    }

    // TODO: CppForm and Flags are not currently dumped, but we can assume flags None for most enums and guess CppForm based on names and CppType
    const bool bCppTypeIsNamespaced = std::wstring(NewEnum->GetCppType().GetCharArray().GetData()).contains(TEXT("::"));
    const UEnum::ECppForm EnumCppForm = bContainsFullyQualifiedNames
                                            ? (bCppTypeIsNamespaced
                                                   ? UEnum::ECppForm::Namespaced
                                                   : UEnum::ECppForm::EnumClass)
                                            : UEnum::ECppForm::Regular;

    // We do not need to generate _MAX key because it will always be present in the enum definition
    NewEnum->GetEnumNames() = EnumNames;
    NewEnum->GetCppForm() = EnumCppForm;

    return NewEnum;
}

UFunction* Suzie::FindOrCreateFunction(FDynamicClassGenerationContext& Context, const FDynamicFunction& Function)
{
    // Check if the function already exists
    if (UFunction* ExistingFunction = UObjectGlobals::FindObject<UFunction>(nullptr, *Function.Path))
    {
        return ExistingFunction;
    }

    FString ClassPathOrPackageName;
    FString ObjectName;
    ParseObjectPath(Function.Path, ClassPathOrPackageName, ObjectName);

    // Function can be outered either to a class or to a package, we can decide based on whenever there is a separator in the path
    UObject* FunctionOuterObject;
    if (std::wstring(ClassPathOrPackageName.GetCharArray().GetData()).contains('.'))
    {
        // This is a class path because it is at least two levels deep. We do not need our outer to be registered, just to exist
        FunctionOuterObject = FindOrCreateUnregisteredClass(Context, ClassPathOrPackageName);
    }
    else
    {
        // This is a package and this function is a top level function (most likely a delegate signature)
        FunctionOuterObject = FindOrCreatePackage(ClassPathOrPackageName);
    }

    // Check if the function already exists in its parent object
    if (UFunction* ExistingFunction = UObjectGlobals::FindObject<UFunction>(FunctionOuterObject, *ObjectName))
    {
        return ExistingFunction;
    }

    // Convert struct flag names to the struct flags bitmask
    EFunctionFlags FunctionFlags = Function.Flags;

    // Have to temporarily mark the function as RF_ArchetypeObject to be able to create functions with UPackage as outer
    UFunction* NewFunction = UObjectGlobals::NewObject<UFunction>(FunctionOuterObject, FName(*ObjectName, FNAME_Add),
                                                                  (EObjectFlags)(RF_Public | RF_MarkAsRootSet |
                                                                      RF_ArchetypeObject));
    NewFunction->ClearFlags(RF_ArchetypeObject);
    NewFunction->GetFunctionFlags() |= FunctionFlags;

    // Update native function
    NewFunction->GetFunc() = reinterpret_cast<std::function<void(UObject*, FFrame*, void*)>*>(Function.Func);

    // Create function parameter properties (and function return value property)
    for (const auto& Property : Function.Properties)
    {
        AddPropertyToStruct(Context, NewFunction, Property);
    }

    // This function will always be linked as a last element of the list, so it has no next element
    NewFunction->GetNext() = nullptr;

    uintptr_t* EmptyPropertyLinkArchive = (uintptr_t*)FMemory::Malloc(0x90);

    struct FArchive_vtbl
    {
        char _padding[0x170];
        void* Preload;
    };

    FArchive_vtbl* vtbl = (FArchive_vtbl*)FMemory::Malloc(0x178);

    vtbl->Preload = []
    {
    };
    *EmptyPropertyLinkArchive = (uintptr_t)vtbl;

    // Bind the function and calculate property layout and function locals size
    NewFunction->Bind();
    NewFunction->Link(*(FArchive*)EmptyPropertyLinkArchive, true);

    FMemory::Free(EmptyPropertyLinkArchive);
    FMemory::Free(vtbl);

    Output::send<LogLevel::Verbose>(
        std::format(STR("Created function {} in outer {}\n"), *ObjectName, *FunctionOuterObject->GetName().c_str()));

    return NewFunction;
}

UFunction* Suzie::FindOrCreateFunction(FDynamicClassGenerationContext& Context, const FString& FunctionPath)
{
    // Check if the function already exists
    if (UFunction* ExistingFunction = UObjectGlobals::FindObject<UFunction>(nullptr, *FunctionPath))
    {
        return ExistingFunction;
    }

    FString ClassPathOrPackageName;
    FString ObjectName;
    ParseObjectPath(FunctionPath, ClassPathOrPackageName, ObjectName);

    // Function can be outered either to a class or to a package, we can decide based on whenever there is a separator in the path
    UObject* FunctionOuterObject;
    if (std::wstring(ClassPathOrPackageName.GetCharArray().GetData()).contains('.'))
    {
        // This is a class path because it is at least two levels deep. We do not need our outer to be registered, just to exist
        FunctionOuterObject = FindOrCreateUnregisteredClass(Context, ClassPathOrPackageName);
    }
    else
    {
        // This is a package and this function is a top level function (most likely a delegate signature)
        FunctionOuterObject = FindOrCreatePackage(ClassPathOrPackageName);
    }

    // Check if the function already exists in its parent object
    if (UFunction* ExistingFunction = UObjectGlobals::FindObject<UFunction>(FunctionOuterObject, *ObjectName))
    {
        return ExistingFunction;
    }

    const auto Function = DynamicFunctions[FunctionPath];

    // Convert struct flag names to the struct flags bitmask
    EFunctionFlags FunctionFlags = Function->Flags;

    // Have to temporarily mark the function as RF_ArchetypeObject to be able to create functions with UPackage as outer
    UFunction* NewFunction = UObjectGlobals::NewObject<UFunction>(FunctionOuterObject, FName(*ObjectName, FNAME_Add),
                                                                  (EObjectFlags)(RF_Public | RF_Transient |
                                                                      RF_MarkAsRootSet | RF_ArchetypeObject));
    NewFunction->ClearFlags(RF_ArchetypeObject);
    NewFunction->GetFunctionFlags() |= FunctionFlags;

    // Update native function
    NewFunction->GetFunc() = reinterpret_cast<std::function<void(UObject*, FFrame*, void*)>*>(Function->Func);

    // Create function parameter properties (and function return value property)
    for (const auto& Property : Function->Properties)
    {
        AddPropertyToStruct(Context, NewFunction, Property);
    }

    // This function will always be linked as a last element of the list, so it has no next element
    NewFunction->GetNext() = nullptr;

    uintptr_t* EmptyPropertyLinkArchive = (uintptr_t*)FMemory::Malloc(0x90);

    struct FArchive_vtbl
    {
        char _padding[0x170];
        void* Preload;
    };

    FArchive_vtbl* vtbl = (FArchive_vtbl*)FMemory::Malloc(0x178);

    vtbl->Preload = []
    {
    };
    *EmptyPropertyLinkArchive = (uintptr_t)vtbl;

    // Bind the function and calculate property layout and function locals size
    NewFunction->Bind();
    NewFunction->Link(*(FArchive*)EmptyPropertyLinkArchive, true);

    FMemory::Free(EmptyPropertyLinkArchive);
    FMemory::Free(vtbl);

    Output::send<LogLevel::Verbose>(
        std::format(STR("Created function {} in outer {}\n"), *ObjectName, *FunctionOuterObject->GetName().c_str()));

    return NewFunction;
}

void Suzie::Create()
{
    // Create class generation context
    FDynamicClassGenerationContext ClassGenerationContext;

    // Create classes, script structs and global delegate functions
    for (const auto& Class : DynamicClasses)
    {
        FindOrCreateClass(ClassGenerationContext, Class.Key);
    }
    for (const auto& Struct : DynamicStructs)
    {
        FindOrCreateScriptStruct(ClassGenerationContext, Struct.Key);
    }
    for (const auto& Enum : DynamicEnums)
    {
        FindOrCreateEnum(ClassGenerationContext, Enum.Key);
    }
    for (const auto& Function : DynamicFunctions)
    {
        FindOrCreateFunction(ClassGenerationContext, Function.Key);
    }

    // Finalize all classes that we have created now. This includes assembling reference streams, creating default subobjects and populating them with data
    TArray<UClass*> ClassesPendingFinalization;
    ClassGenerationContext.ClassesPendingFinalization.GenerateKeyArray(ClassesPendingFinalization);
    for (UClass* ClassPendingFinalization : ClassesPendingFinalization)
    {
        FinalizeClass(ClassGenerationContext, ClassPendingFinalization);
    }
}

void Suzie::InsertClass(const FString& Path, FDynamicClass* Class)
{
    DynamicClasses.Add(Path, Class);
}

void Suzie::InsertStruct(const FString& Path, FDynamicScriptStruct* Struct)
{
    DynamicStructs.Add(Path, Struct);
}

void Suzie::InsertEnum(const FString& Path, FDynamicEnum* Enum)
{
    DynamicEnums.Add(Path, Enum);
}

void Suzie::InsertFunction(const FString& Path, FDynamicFunction* Function)
{
    DynamicFunctions.Add(Path, Function);
}

void Suzie::StaticInitialize()
{
    Instance = new Suzie;
}

Suzie* Suzie::GetInstance()
{
    return Instance;
}

void Suzie::Initialize()
{
    Create();
}

UPackage* Suzie::FindOrCreatePackage(const FString& PackageName)
{
    UPackage* Package = UObjectGlobals::NewObject<UPackage>(nullptr, FName(*PackageName, FNAME_Add));
    *reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(Package) + 0x50) = 0x10;
    return Package;
}

UClass* Suzie::UClass_Ctor(UClass* ParentClass, EClassFlags ClassFlags)
{
    EClassFlags ClassFlags = CLASS_Native | CLASS_Intrinsic | ClassFlags;

    auto ConstructedClassObject = static_cast<UClass*>(UObjectBase::AllocateUObject(
        UClass::StaticClass()->GetStructureSize(), UClass::StaticClass()->GetMinAlignment(), true));

    *(uintptr_t*)ConstructedClassObject = *(uintptr_t*)UClass::StaticClass();
    ConstructedClassObject->GetPropertiesSize() = ParentClass->GetStructureSize();
    ConstructedClassObject->GetMinAlignment() = ParentClass->GetMinAlignment();
    ConstructedClassObject->GetClassFlags() = ClassFlags;
    ConstructedClassObject->GetClassCastFlags() = CASTCLASS_None;
    ConstructedClassObject->GetClassConfigName() = FName(STR("Engine"), FNAME_Add);
    ConstructedClassObject->GetObjectFlags() = static_cast<EObjectFlags>(RF_Public | RF_MarkAsNative | RF_MarkAsRootSet);
    std::function<void(const void*)>* Ctor = new std::function((void(*)(const void*))Class->Ctor);
    ConstructedClassObject->GetClassConstructor() = Ctor;
    std::function<UObject* (void*)>* VTableHelperCtorCaller = new std::function([](void*) -> UObject* { return nullptr; });
    ConstructedClassObject->GetClassVTableHelperCtorCaller() = VTableHelperCtorCaller;

    if (Unreal::Version::IsAtLeast(4, 24))
    {
        ConstructedClassObject->GetSparseClassData() = nullptr;
        ConstructedClassObject->GetSparseClassDataStruct() = nullptr;
    }

    if (Unreal::Version::IsAtLeast(4, 25))
    {
        ConstructedClassObject->GetUnresolvedScriptProperties() = nullptr;
    }

    if (Unreal::Version::IsBelow(5, 0))
    {
        ConstructedClassObject->GetClassGeneratedBy() = nullptr;
    }
    else
    {
        ConstructedClassObject->GetbLayoutChanging() = false;
    }

    if (Unreal::Version::IsBelow(5, 1))
    {
        std::function<void(UObject*, void*)>* ClassAddReferencedObjects = new std::function([](UObject*, void*) {});
        ConstructedClassObject->GetClassAddReferencedObjects() = ClassAddReferencedObjects;
    }

    ConstructedClassObject->GetClassUnique() = 0;
    ConstructedClassObject->GetbCooked() = false;
    ConstructedClassObject->GetNetFields() = {};
    ConstructedClassObject->GetClassDefaultObject() = nullptr;
    ConstructedClassObject->GetChildren() = nullptr;
    ConstructedClassObject->GetChildProperties() = nullptr;
    ConstructedClassObject->GetPropertyLink() = nullptr;
    ConstructedClassObject->GetRefLink() = nullptr;
    ConstructedClassObject->GetDestructorLink() = nullptr;
    ConstructedClassObject->GetPostConstructLink() = nullptr;
    ConstructedClassObject->GetNext() = nullptr;

    return ConstructedClassObject;
}

UClass* Suzie::FindOrCreateUnregisteredClass(FDynamicClassGenerationContext& Context, const FString& ClassPath)
{
    // Attempt to find an existing class first
    if (UClass* ExistingClass = UObjectGlobals::StaticFindObject<UClass*>(nullptr, nullptr, *ClassPath))
    {
        return ExistingClass;
    }

    const auto Class = DynamicClasses[ClassPath];

    const FString ParentClassPath = Class->SuperStruct;
    UClass* ParentClass = FindOrCreateClass(Context, ParentClassPath);
    if (!ParentClass)
    {
        return nullptr;
    }

    FString PackageName;
    FString ClassName;
    ParseObjectPath(ClassPath, PackageName, ClassName);

    // DeferredRegister for UClass will automatically find the package by name, but we should still prime it before that
    FindOrCreatePackage(PackageName);

    EClassFlags ClassFlags = CLASS_Native | CLASS_Intrinsic | Class->ClassFlags;

    UClass* ConstructedClassObject = UClass_Ctor(ParentClass, ClassFlags);

    //Set super structure and ClassWithin (they are required prior to registering)
    ConstructedClassObject->SetSuperStruct(ParentClass);
    ConstructedClassObject->GetClassWithin() = UObject::StaticClass();

    //Register pending object, apply class flags, set static type info and link it
    ConstructedClassObject->RegisterDependencies();
    ConstructedClassObject->DeferredRegister(UClass::StaticClass(), *PackageName, *ClassName);

    Context.ClassesPendingConstruction.Add(ConstructedClassObject, ClassPath);

    return ConstructedClassObject;
}

bool Suzie::ParseObjectConstructionData(const FDynamicClassGenerationContext& Context, const FString& ObjectPath,
                                        FDynamicObjectConstructionData& ObjectConstructionData)
{
    // Retrieve the data for the object
    const auto ObjectDefinition = DynamicClasses[ObjectPath];

    FString OuterObjectPath;
    FString ObjectName;
    ParseObjectPath(ObjectPath, OuterObjectPath, ObjectName);
    ObjectConstructionData.ObjectName = FName(*ObjectName);

    // Find the class of this object
    const FString ObjectClassPath = ObjectDefinition->Path;
    ObjectConstructionData.ObjectClass = UObjectGlobals::FindObject<UClass>(nullptr, *ObjectClassPath);
    if (ObjectConstructionData.ObjectClass == nullptr)
    {
        return false;
    }

    ObjectConstructionData.ObjectFlags = (EObjectFlags)(ObjectConstructionData.ObjectFlags | ObjectDefinition->Flags);
    return true;
}

void Suzie::CollectNestedDefaultSubobjectTypeOverrides(FDynamicClassGenerationContext& Context,
                                                       TArray<FName> SubobjectNameStack, const FString& SubobjectPath,
                                                       TArray<FNestedDefaultSubobjectOverrideData>&
                                                       OutSubobjectOverrideData)
{
    const auto ObjectDefinition = DynamicClasses[SubobjectPath];

    // Parse construction data for this object first. Skip if this is not a subobject
    FDynamicObjectConstructionData ObjectConstructionData;
    if (!ParseObjectConstructionData(Context, SubobjectPath, ObjectConstructionData) || !EnumHasAnyFlags(
        ObjectConstructionData.ObjectFlags, RF_DefaultSubObject))
    {
        return;
    }
    // Class of the overriden default subobject might not have been finalized yet, in which case we have to finalize it now to have its archetype with correct values
    if (Context.ClassesPendingFinalization.Contains(ObjectConstructionData.ObjectClass))
    {
        FinalizeClass(Context, ObjectConstructionData.ObjectClass);
    }

    // Add the name of this object to the stack. If this is not a top level subobject, add it to the override list
    SubobjectNameStack.Add(ObjectConstructionData.ObjectName);
    if (SubobjectNameStack.Num() > 1)
    {
        OutSubobjectOverrideData.Add({SubobjectNameStack, ObjectConstructionData.ObjectClass});
    }

    // Iterate over children and collect nested default subobject overrides for them
    for (const auto& Child : ObjectDefinition->Children)
    {
        // CollectNestedDefaultSubobjectTypeOverrides will discard children that are not actually subobjects
        const FString ChildPath = Child.Path;
        CollectNestedDefaultSubobjectTypeOverrides(Context, SubobjectNameStack, ChildPath, OutSubobjectOverrideData);
    }
}

void Suzie::FinalizeClass(FDynamicClassGenerationContext& Context, UClass* Class)
{
    // Skip this class if it has already been finalized as a dependency of its child class
    if (!Context.ClassesPendingFinalization.Contains(Class))
    {
        return;
    }

    // Find the definition for the class default object
    const FString ClassDefaultObjectPath = Context.ClassesPendingFinalization.FindAndRemoveChecked(Class);

    // Finalize our parent class first since we require parent class CDO to be populated before CDO for this class can be created
    UClass* ParentClass = Class->GetSuperClass();
    if (ParentClass && Context.ClassesPendingFinalization.Contains(ParentClass))
    {
        FinalizeClass(Context, ParentClass);
    }

    // Assemble reference token stream for garbage collector
    Class->AssembleReferenceTokenStream(true);
    // Create class default object now that we have class object construction data
    Class->CreateDefaultObject();
}

void Suzie::ParseObjectPath(const FString& ObjectPath, FString& OutOuterObjectPath, FString& OutObjectName)
{
    auto ObjectPathWString = std::wstring(ObjectPath.GetCharArray().GetData());
    if (size_t ObjectNameSeparatorIndex = ObjectPathWString.find(STR(":")); ObjectNameSeparatorIndex !=
        ObjectPathWString.npos)
    {
        // There is a sub-object separator in the path name, string past it is the object name
        OutOuterObjectPath = FString(ObjectPathWString.substr(0, ObjectNameSeparatorIndex).c_str());
        OutObjectName = FString(ObjectPathWString.substr(ObjectNameSeparatorIndex + 1).c_str());
    }
    else if (ObjectNameSeparatorIndex = ObjectPathWString.find(STR(".")); ObjectNameSeparatorIndex != ObjectPathWString.
        npos)
    {
        // This is a top level object (or this is a legacy path), string past the asset name separator is the object name
        OutOuterObjectPath = FString(ObjectPathWString.substr(0, ObjectNameSeparatorIndex).c_str());
        OutObjectName = FString(ObjectPathWString.substr(ObjectNameSeparatorIndex + 1).c_str());
    }
    else
    {
        // This is a top level object (UPackage) name
        OutOuterObjectPath = FString(STR(""));
        OutObjectName = ObjectPath;
    }
}

FProperty* Suzie::AddPropertyToStruct(FDynamicClassGenerationContext& Context, UStruct* Struct,
                                      const FDynamicProperty& Property, EPropertyFlags ExtraPropertyFlags)
{
    if (FProperty* NewProperty = BuildProperty(Context, Struct, Property, ExtraPropertyFlags))
    {
        // This property will always be linked as a last element of the list, so it has no next element
        NewProperty->SetNext((FField*)nullptr);

        // Link new property to the end of the linked property list
        if (Struct->GetChildProperties() != nullptr)
        {
            FField* CurrentProperty = Struct->GetChildProperties();
            while (CurrentProperty->GetNextFieldAsProperty())
            {
                CurrentProperty = CurrentProperty->GetNextFieldAsProperty();
            }
            CurrentProperty->SetNext(NewProperty);
        }
        else
        {
            // This is the first property in the struct, assign it as a head of the linked property list
            Struct->GetChildProperties() = NewProperty;
        }
        Output::send<LogLevel::Verbose>(
            std::format(STR("Added property {} to struct {}\n"), NewProperty->GetName(), Struct->GetName()));
        return NewProperty;
    }
    return nullptr;
}

void Suzie::AddFunctionToClass(FDynamicClassGenerationContext& Context, UClass* Class, const FDynamicFunction& Function,
                               EFunctionFlags ExtraFunctionFlags)
{
    if (UFunction* NewFunction = FindOrCreateFunction(Context, Function))
    {
        // Append additional flags to the function
        NewFunction->GetFunctionFlags() |= ExtraFunctionFlags;

        // This function will always be linked as a last element of the list, so it has no next element
        NewFunction->GetNext() = nullptr;

        // Link new function to the end of the linked property list
        if (Class->GetChildren() != nullptr)
        {
            UField* CurrentFunction = Class->GetChildren();
            while (CurrentFunction->GetNext())
            {
                CurrentFunction = CurrentFunction->GetNext();
            }
            CurrentFunction->GetNext() = NewFunction;
        }
        else
        {
            // This is the first function in the class, assign it as a head of the linked function list
            Class->GetChildren() = NewFunction;
        }

        // Add the function to the function lookup for the class
        Class->GetFuncMap().Add(FName(NewFunction->GetName(), FNAME_Add), TObjectPtr(NewFunction));

        Output::send<LogLevel::Verbose>(
            std::format(STR("Added function {} to class {}\n"), NewFunction->GetName(), Class->GetName()));
    }
}

FProperty* Suzie::BuildProperty(FDynamicClassGenerationContext& Context, FFieldVariant Owner,
                                const FDynamicProperty& Property, EPropertyFlags ExtraPropertyFlags)
{
    // Note that only flags that are set manually (e.g. non-computed flags) should be listed here
    // Convert struct flag names to the struct flags bitmask
    EPropertyFlags PropertyFlags = ExtraPropertyFlags | Property.Flags;

    const FString PropertyName = Property.Name;
    const FString PropertyType = Property.Type;

    FProperty* NewProperty = CastField<FProperty>(
        FField::Construct(FName(*PropertyType, FNAME_Add), Owner, FName(*PropertyName, FNAME_Add), RF_Public));
    if (NewProperty == nullptr)
    {
        Output::send<LogLevel::Error>(
            std::format(STR("Failed to create property of type {}: not supported\n"), *PropertyName));
        return nullptr;
    }

    NewProperty->GetArrayDim() = Property.ArrayDim;
    NewProperty->GetPropertyFlags() |= PropertyFlags;

    if (FObjectPropertyBase* ObjectPropertyBase = CastField<FObjectPropertyBase>(NewProperty))
    {
        UClass* PropertyClass = FindOrCreateUnregisteredClass(Context, Property.Class);
        // Fall back to UObject class if property class could not be found
        ObjectPropertyBase->GetPropertyClass() = PropertyClass ? PropertyClass : UObject::StaticClass();

        // Class properties additionally define MetaClass value
        if (FClassProperty* ClassProperty = CastField<FClassProperty>(NewProperty))
        {
            UClass* MetaClass = FindOrCreateUnregisteredClass(Context, Property.MetaClass);
            // Fall back to UObject meta-class if meta-class could not be found
            ClassProperty->GetMetaClass() = MetaClass ? MetaClass : UObject::StaticClass();
        }
        else if (FSoftClassProperty* SoftClassProperty = CastField<FSoftClassProperty>(NewProperty))
        {
            UClass* MetaClass = FindOrCreateUnregisteredClass(Context, Property.MetaClass);
            // Fall back to UObject meta-class if meta-class could not be found
            SoftClassProperty->GetMetaClass() = MetaClass ? MetaClass : UObject::StaticClass();
        }
    }
    else if (FInterfaceProperty* InterfaceProperty = CastField<FInterfaceProperty>(NewProperty))
    {
        UClass* InterfaceClass = FindOrCreateUnregisteredClass(Context, Property.Class);
        // Fall back to UInterface if interface class could not be found
        InterfaceProperty->SetInterfaceClass(InterfaceClass ? InterfaceClass : UInterface::StaticClass());
    }
    else if (FStructProperty* StructProperty = CastField<FStructProperty>(NewProperty))
    {
        UScriptStruct* Struct = FindOrCreateScriptStruct(Context, Property.Struct);
        StructProperty->GetStruct() = Struct ? Struct : nullptr;
    }
    else if (FEnumProperty* EnumProperty = CastField<FEnumProperty>(NewProperty))
    {
        UEnum* Enum = FindOrCreateEnum(Context, Property.Enum);
        // Fall back to EMovementMode if enum class could not be found
        EnumProperty->GetEnum() = Enum ? Enum : nullptr;

        FProperty* UnderlyingProp = BuildProperty(Context, EnumProperty, *Property.Container);
        EnumProperty->AddCppProperty(UnderlyingProp);
    }
    else if (FByteProperty* ByteProperty = CastField<FByteProperty>(NewProperty))
    {
        UEnum* Enum = FindOrCreateEnum(Context, Property.Enum);
        // Fall back to EMovementMode if enum class could not be found
        ByteProperty->GetEnum() = Enum ? Enum : nullptr;
    }
    else if (FDelegateProperty* DelegateProperty = CastField<FDelegateProperty>(NewProperty))
    {
        UFunction* SignatureFunction = FindOrCreateFunction(Context, *Property.SignatureFunction);
        // Fall back to FOnTimelineEvent delegate signature in the engine if real delegate signature could not be found
        DelegateProperty->GetSignatureFunction() = SignatureFunction
                                                       ? SignatureFunction
                                                       : UObjectGlobals::FindObject<UFunction>(
                                                           nullptr, TEXT(
                                                               "/Script/Engine.OnTimelineEvent__DelegateSignature"));
    }
    else if (FMulticastDelegateProperty* MulticastDelegateProperty = CastField<FMulticastDelegateProperty>(NewProperty))
    {
        UFunction* SignatureFunction = FindOrCreateFunction(Context, *Property.SignatureFunction);
        // Fall back to FOnTimelineEvent delegate signature in the engine if real delegate signature could not be found
        MulticastDelegateProperty->GetSignatureFunction() = SignatureFunction
                                                                ? SignatureFunction
                                                                : UObjectGlobals::FindObject<UFunction>(
                                                                    nullptr, TEXT(
                                                                        "/Script/Engine.OnTimelineEvent__DelegateSignature"));
    }
    else if (FFieldPathProperty* FieldPathProperty = CastField<FFieldPathProperty>(NewProperty))
    {
        FFieldClass* const* PropertyClassPtr = FFieldClass::GetNameToFieldClassMap().Find(
            FName(STR("property_class"), FNAME_Add));
        // Fall back to FProperty if property class could not be found
        FieldPathProperty->GetPropertyClass() = PropertyClassPtr ? *PropertyClassPtr : nullptr;
    }
    else
    {
        // TODO: These can be handled together without special casing them by dumping array of FField::GetInnerFields instead of individual fields
        if (FArrayProperty* ArrayProperty = CastField<FArrayProperty>(NewProperty))
        {
            FProperty* Inner = BuildProperty(Context, NewProperty, *Property.Inner);
            ArrayProperty->AddCppProperty(Inner);
        }
        else if (FSetProperty* SetProperty = CastField<FSetProperty>(NewProperty))
        {
            FProperty* KeyProp = BuildProperty(Context, NewProperty, *Property.Key);
            SetProperty->AddCppProperty(KeyProp);
        }
        else if (FMapProperty* MapProperty = CastField<FMapProperty>(NewProperty))
        {
            FProperty* KeyProp = BuildProperty(Context, NewProperty, *Property.Key);
            FProperty* ValueProp = BuildProperty(Context, NewProperty, *Property.Value);

            MapProperty->AddCppProperty(KeyProp);
            MapProperty->AddCppProperty(ValueProp);
        }
    }

    return NewProperty;
}

UScriptStruct* Suzie::GetSparseClassDataArchetypeStruct(UClass* Context)
{
    UClass* SuperClass = Context->GetSuperClass();
    return SuperClass ? SuperClass->GetSparseClassDataStruct() : nullptr;
}
