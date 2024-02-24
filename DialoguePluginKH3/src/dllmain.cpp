#include <DynamicOutput/Output.hpp>
#include <Mod/CppUserModBase.hpp>
#include <Modules/ModuleManager.hpp>
#include <polyhook2/Exceptions/AVehHook.hpp>
#include <SigScanner/SinglePassSigScanner.hpp>
#include <Unreal/UObjectGlobals.hpp>
#include <Unreal/UObject.hpp>

#include "FProperty.hpp"
#include "FText.hpp"
#include "Hooks.hpp"
#include "UClass.hpp"
#include "UE4SSProgram.hpp"
#include "UnrealCoreStructs.hpp"
#include "Property/FObjectProperty.hpp"

namespace DialoguePlugin
{
    using namespace RC;
    using namespace Unreal;
    
    /**
    * DialoguePluginKH3: UE4SS c++ mod class defintion
    */
    class DialoguePluginKH3 : public CppUserModBase
    {
    public:
        
        // constructor
        DialoguePluginKH3() {
            ModVersion = STR("0.1");
            ModName = STR("DialoguePluginKH3");
            ModAuthors = STR("UE4SS");
            ModDescription = STR("A basic template C++ mod");
            // Do not change this unless you want to target a UE4SS version
            // other than the one you're currently building with somehow.
            //ModIntendedSDKVersion = STR("2.6");
        }
        
        // destructor
        ~DialoguePluginKH3() override {
            // fill when required
        }
        
        auto on_program_start() -> void override
        {
        }

        auto on_dll_load(std::wstring_view dll_name) -> void override
        {
        }

        auto static FStringDestructor(FString* pThis) -> void
        {
            if (pThis->GetCharTArray().GetData())
            {
                FMemory::Free(pThis->GetCharTArray().GetData());
            }
        }

        auto static NullSub() -> void*
        {
            return nullptr;
        }

        auto static VoidSub() -> void {}

        auto static TrueSub() -> bool
        {
            return true;
        }
        
        auto static CanHoldDoubleValue(void*, double val) -> bool
        {
            return (double)(int)val == val;
        }
        
        static inline FVector ZeroVector = FVector();

        static FName UObject_GetExporterName( void )
        {
            return FName(STR(""), FNAME_Add);
        }

        static FString* UObject_GetDesc(UObject *pThis, FString *result)
        {
            result->Clear();
            return result;
        }

        static const TCHAR* UObject_StaticConfigName()
        {
            return STR("Engine");
        }

        static intptr_t FSub()
        {
            return -1;
        }

        static int64_t Return2()
        {
            return 2;
        }
        
        static void* FSharedReferencer_OperatorEqual(uintptr_t* pThis, uintptr_t* that)
        {
            if (*pThis != *that)
            {
                auto InReferenceController = *that;
                if (InReferenceController)
                {
                    InterlockedIncrement((uint32*)(InReferenceController + 8));
                }
                auto ReferenceController = *pThis;
                if (ReferenceController)
                {
                    if (ReferenceController && InterlockedExchangeAdd((unsigned int*)((char*)ReferenceController + 8), 0xFFFFFFFF) == 1)
                    {
                        if (ReferenceController && InterlockedExchangeAdd((unsigned int*)((char*)ReferenceController + 0xC), 0xFFFFFFFF) == 1)
                        {
                            FMemory::Free((void*)ReferenceController);
                        }
                    }
                }
                *pThis = *that;
            }
            return pThis;
        }
        
        static FText* FText_OperatorEqual(FText* pThis, FText* that)
        {
            pThis->Data = that->Data;
            pThis->SharedRefCollector = that->SharedRefCollector;
            pThis->Flags = that->Flags;
            pThis->Unk = that->Unk;
            return pThis;
        }
        
        static FText* FText_ConstOperatorEqual(FText* pThis, FText* that)
        {
            FSharedReferencer_OperatorEqual((uintptr_t*)&pThis->SharedRefCollector, (uintptr_t*)&that->SharedRefCollector);
            pThis->Data = that->Data;
            pThis->Flags = that->Flags;
            pThis->Unk = that->Unk;
            return pThis;
        }

        static void FText_CopyConstructor(FText* pThis, FText* that)
        {
            pThis->Data = that->Data;
            pThis->SharedRefCollector = that->SharedRefCollector;
            pThis->Flags = that->Flags;
            pThis->Unk = that->Unk;
            if (pThis->SharedRefCollector)
            {
                InterlockedIncrement((unsigned int*)((char*)pThis->SharedRefCollector + 8));
            }
        }

        static void FText_Constructor(FText* pThis)
        {
            FText* Empty = FTextGetEmpty();
            pThis->Data = Empty->Data;
            pThis->SharedRefCollector = Empty->SharedRefCollector;
            pThis->Flags = 0;
            pThis->Unk = Empty->Unk;
            if (pThis->SharedRefCollector)
            {
                InterlockedIncrement((unsigned int*)((char*)pThis->SharedRefCollector + 8));
            }
        }

        static void FText_Destructor(FText* pThis)
        {
            auto SharedRefCollector = pThis->SharedRefCollector;
            if (SharedRefCollector && InterlockedExchangeAdd((unsigned int*)((char*)SharedRefCollector + 8), 0xFFFFFFFF) == 1)
            {
                if (SharedRefCollector && InterlockedExchangeAdd((unsigned int*)((char*)SharedRefCollector + 0xC), 0xFFFFFFFF) == 1)
                {
                    FMemory::Free(SharedRefCollector);
                }
            }
        }
        
        /**
         * Struct containing the function pointer and package name of a UStruct to be registered with UObject system
         */
        struct FPendingStructRegistrant
        {
            UScriptStruct *(*RegisterFn)();
            const TCHAR* PackageName;

            FPendingStructRegistrant() {}
            FPendingStructRegistrant(UScriptStruct *(*Fn)(), const TCHAR* InPackageName)
                : RegisterFn(Fn)
                , PackageName(InPackageName)
            {
            }
            FORCEINLINE bool operator==(const FPendingStructRegistrant& Other) const
            {
                return RegisterFn == Other.RegisterFn;
            }
        };
        
        // used by the EDL at boot time to coordinate loading with what is going on with the deferred registration stuff
        enum class ENotifyRegistrationType
        {
            NRT_Class,
            NRT_ClassCDO,
            NRT_Struct,
            NRT_Enum,
            NRT_Package,
        };

        enum class ENotifyRegistrationPhase
        {
            NRP_Added,
            NRP_Started,
            NRP_Finished,
        };
        
        static inline Function<TMap<FName, UScriptStruct *(*)()>&()> GetDynamicStructMap;
        static inline Function<TArray<FPendingStructRegistrant>&()> GetDeferredCompiledInStructRegistration;
        static inline Function<void(const TCHAR*, const TCHAR*, ENotifyRegistrationType, ENotifyRegistrationPhase, UObject *(*)(), bool)> NotifyRegistrationEvent;
        static inline Function<FText*()> FTextGetEmpty;

        static void UObjectCompiledInDeferStruct(UScriptStruct *(*InRegister)(), const TCHAR* PackageName, const FName ObjectName, bool bDynamic, const TCHAR* DynamicPathName)
        {
            if (!bDynamic)
            {
                FPendingStructRegistrant Registrant(InRegister, PackageName);
                checkSlow(!GetDeferredCompiledInStructRegistration().Contains(Registrant));
                GetDeferredCompiledInStructRegistration().Add(Registrant);
            }
            else
            {
                GetDynamicStructMap().Add(FName(DynamicPathName), InRegister);
            }
            NotifyRegistrationEvent(PackageName, ObjectName.ToString().c_str(), ENotifyRegistrationType::NRT_Struct, ENotifyRegistrationPhase::NRP_Added, (UObject *(*)())(InRegister), bDynamic);
        }
        
        static inline Function<bool(UObject*, TCHAR*, FOutputDevice*, UObject*, bool)> CallFunctionByNameWithArguments;
        static inline Function<void(UObject*, void*)> ObjConstructor;
        static inline Function<void(UObject*, void*)> ObjInitializerConstructor;
        
        static bool ProcessConsoleExec(UObject* pThis, TCHAR* Cmd, FOutputDevice* Ar, UObject* Executor)
        {
            return CallFunctionByNameWithArguments(pThis, Cmd, Ar, Executor, false);
        }
        
        static UEnum* UNumericProperty_GetIntPropertyEnum(char* pThis)
        {
            return (UEnum*)(pThis + 0x90);
        }

        static int UNumericProperty_Int_GetMinAlignment(char* pThis)
        {
            return 4;
        }

        static int UObjectProperty_GetMinAlignment(char* pThis)
        {
            return 8;
        }

        static const TCHAR* UStruct_GetPrefixCPP(char* pThis)
        {
            return STR("F");
        }

        static const UObject* GetTypedOuter(UObject* pThis, UClass* Target)
        {
            return pThis->GetTypedOuter(Target);
        }

        using UnrealScriptFunctionWrapper = void(*)(UObject* Context, FFrame_50_AndBelow& TheStack, void*const RESULT_DECL);
        static inline UnrealScriptFunctionWrapper* GNatives;
        
        static void FFrame_Step(FFrame_50_AndBelow* pThis, UObject* Context, void*const Z_Param__Result)
        {
            int32 B = *pThis->Code++;
            GNatives[B](Context, *pThis, Z_Param__Result);
        }
        
        static void FFrame_StepExplicitProperty(FFrame_50_AndBelow* pThis, void*const Result, FProperty* Property)
        {
            checkSlow(Result != NULL);

            if (Property->GetPropertyFlags() & CPF_OutParm)
            {
                // look through the out parameter infos and find the one that has the address of this property
                FOutParmRec* Out = pThis->OutParms;
                checkSlow(Out);
                while (Out->Property != Property)
                {
                    Out = Out->NextOutParm;
                    checkSlow(Out);
                }
                pThis->MostRecentPropertyAddress = Out->PropAddr;
                // no need to copy property value, since the caller is just looking for MostRecentPropertyAddress
            }
            else
            {
                pThis->MostRecentPropertyAddress = Property->ContainerPtrToValuePtr<uint8>(pThis->Locals);
                Property->CopyCompleteValueToScriptVM(Result, pThis->MostRecentPropertyAddress);
            }
        }
        
        static inline unsigned char* base_module;
        
        static void UObject_Constructor(UObject* pThis, void* a2)
        {
            ObjConstructor(pThis, a2);
        }
        
        static void UObject_InitializerConstructor(UObject* pThis, void* a2)
        {
            ObjInitializerConstructor(pThis, a2);
        }
        
        static void UDataAsset_Constructor(UObject* pThis, void* a2)
        {
            UObject_Constructor(pThis, a2);
            *(uintptr_t*)pThis = (uintptr_t)(base_module + 0x71AB588);
            *(uintptr_t*)(pThis + 0x40) = *(uintptr_t*)(pThis + 0x18);
        }
        
        static void UDataAsset_InitializerConstructor(UObject* pThis, void* a2)
        {
            UObject_InitializerConstructor(pThis, a2);
            *(uintptr_t*)pThis = (uintptr_t)(base_module + 0x71AB588);
            *(uintptr_t*)(pThis + 0x40) = *(uintptr_t*)(pThis + 0x18);
        }
        
        static inline Function<TArray<void*>*()> GetDeferredClassRegistration;

        static void UClassCompiledInDefer(void* ClassInfo, const TCHAR* Name, SIZE_T ClassSize, uint32 Crc)
        {
            // We will either create a new class or update the static class pointer of the existing one
            GetDeferredClassRegistration()->Add(ClassInfo);
        }
        
        static void UObject_PostInitProperties(UObject* pThis)
        {
            ((void(*)(UObject*, bool, bool, UClass*))*(*(uintptr_t**)pThis->GetClassPrivate() + 720))(pThis, true, false, nullptr);
        }

        static FName* FName_CharConstructor(FName* pThis, const char* Name, EFindName FindType)
        {
            *pThis = FName(to_const_wstring(Name), FindType);
            return pThis;
        }

        static void FHeapAllocator_ForAnyElementType_Constructor(void* pThis)
        {
            *(uintptr_t*)pThis = 0;
        }

        static void ICppStructOps_Constructor(char* pThis, int Size, int Alignment)
        {
            *(uintptr_t*)pThis = (uintptr_t)(base_module + 0x7C8B918);
            *(int*)(pThis + 8) = Size;
            *(int*)(pThis + 12) = Alignment;
        }
        
        static auto register_symbols() -> void
        {
            FModuleManager::RegisterImport("?UnknownFunction_1@UObjectBase@@MEAA_NXZ", TrueSub);
            FModuleManager::RegisterImport("?GetNameTableForDebuggerVisualizers_MT@FName@@SAPEAPEAPEAUFNameEntry@@XZ", "48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8B 0D ? ? ? ? 48 89 5C 24 20 48 85 C9 75 ? E8 ? ? ? ? 48 8B 0D ? ? ? ? 48 8B 01 45 33 C0 BA 08 04 00 00");
            FModuleManager::RegisterImport("?Realloc@FMemory@@SAPEAXPEAX_KI@Z", FMemory::Realloc);
            FModuleManager::RegisterImport("?Free@FMemory@@SAXPEAX@Z", FMemory::Free);
            FModuleManager::RegisterImport("??0FName@@QEAA@PEB_WW4EFindName@@@Z", base_module + 0x32654D0);
            FModuleManager::RegisterImport("?QuantizeSize@FMemory@@SA_K_KI@Z", FMemory::QuantizeSize);
            FModuleManager::RegisterImport("?Memcpy@FGenericPlatformString@@CAPEAXPEAXPEBX_K@Z", static_cast<void*(*)(void*,const void*,SIZE_T)>(FMemory::Memcpy));
            FModuleManager::RegisterImport("??1FString@@QEAA@XZ", FStringDestructor);
            FModuleManager::RegisterImport("?GCoreObjectArrayForDebugVisualizers@@3PEAVFFixedUObjectArray@@EA", base_module + 0x8D4BDF0);
            FModuleManager::RegisterImport("??0ForAnyElementType@FHeapAllocator@@QEAA@XZ", FHeapAllocator_ForAnyElementType_Constructor);
            FModuleManager::RegisterImport("?GetBlocks@FNameDebugVisualizer@@SAPEAPEAEXZ", NullSub);
            FModuleManager::RegisterImport("?ZeroVector@FVector@@2U1@B", &ZeroVector);
            FModuleManager::RegisterImport("?Malloc@FMemory@@SAPEAX_KI@Z", FMemory::Malloc);
            FModuleManager::RegisterImport("?PreDestroyFromReplication@UObject@@UEAAXXZ", VoidSub);
            FModuleManager::RegisterImport("?PostRepNotifies@UObject@@UEAAXXZ", VoidSub);
            FModuleManager::RegisterImport("?PostNetReceive@UObject@@UEAAXXZ", VoidSub);
            FModuleManager::RegisterImport("?PreNetReceive@UObject@@UEAAXXZ", VoidSub);
            FModuleManager::RegisterImport("?GetSubobjectsWithStableNamesForNetworking@UObject@@UEAAXAEAV?$TArray@PEAVUObject@@VFDefaultAllocator@@@@@Z", VoidSub);
            FModuleManager::RegisterImport("?IsSupportedForNetworking@UObject@@UEBA_NXZ", "48 8B 49 18 48 8B 01 48 FF A0 B0 01 00 00");
            FModuleManager::RegisterImport("?IsFullNameStableForNetworking@UObject@@UEBA_NXZ", "40 53 48 83 EC 20 48 8B D9 48 8B 49 20 48 85 C9 74 ? 48 8B 01 FF 50 10");
            FModuleManager::RegisterImport("?IsNameStableForNetworking@UObject@@UEBA_NXZ", base_module + 0x33DC950);
            FModuleManager::RegisterImport("?GetLifetimeReplicatedProps@UObject@@UEBAXAEAV?$TArray@VFLifetimeProperty@@VFDefaultAllocator@@@@@Z", VoidSub);
            FModuleManager::RegisterImport("?TagSubobjects@UObject@@UEAAXW4EObjectFlags@@@Z", "40 55 56 57 48 8D 6C 24 B9 48 81 EC D0 00 00 00 48 C7 45 AF FE FF FF FF");
            FModuleManager::RegisterImport("?IsSafeForRootSet@UObject@@UEBA_NXZ", "48 89 5C 24 08 57 48 83 EC 20 48 8B F9 48 8B D9");
            FModuleManager::RegisterImport("?IsLocalizedResource@UObject@@UEBA_NXZ", "40 53 48 83 EC 40 48 C7 44 24 20 FE FF FF FF 48 8B 41 28");
            FModuleManager::RegisterImport("?GetPrimaryAssetId@UObject@@UEBA?AUFPrimaryAssetId@@XZ", "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 48 83 EC 20 33 DB 48 8B FA 39 1D ? ? ? ?");
            FModuleManager::RegisterImport("?IsAsset@UObject@@UEBA_NXZ", "40 53 48 83 EC 20 8B 41 08 48 8B D9 A8 50");
            FModuleManager::RegisterImport("?GetAssetRegistryTags@UObject@@UEBAXAEAV?$TArray@UFAssetRegistryTag@UObject@@VFDefaultAllocator@@@@@Z", "48 8B C4 55 57 41 56 48 8D 68 A1 48 81 EC 90 00 00 00 48 C7 45 E7 FE FF FF FF 48 89 58 10 48 89 70 18 48 8B DA");
            FModuleManager::RegisterImport("?AreNativePropertiesIdenticalTo@UObject@@UEBA_NPEAV1@@Z", TrueSub);
            FModuleManager::RegisterImport("?GetRestoreForUObjectOverwrite@UObject@@UEAAPEAVFRestoreForUObjectOverwrite@@XZ", NullSub);
            FModuleManager::RegisterImport("?GetExporterName@UObject@@UEAA?AVFName@@XZ", UObject_GetExporterName);
            FModuleManager::RegisterImport("?GetResourceSize@UObject@@UEAA_KW4Type@EResourceSizeMode@@@Z", "48 83 EC 58 33 C0 89 54 24 20 48 89 44 24 28 48 8D 54 24 20 48 89 44 24 30 48 89 44 24 38 48 89 44 24 40 48 89 44 24 48 48 8B 01 FF 90 58 01 00 00");
            FModuleManager::RegisterImport("?GetResourceSizeEx@UObject@@UEAAXAEAUFResourceSizeEx@@@Z", VoidSub);
            FModuleManager::RegisterImport("?GetNativePropertyValues@UObject@@UEBA_NAEAV?$TMap@VFString@@V1@VFDefaultSetAllocator@@U?$TDefaultMapHashableKeyFuncs@VFString@@V1@$0A@@@@@I@Z", NullSub);
            FModuleManager::RegisterImport("?GetWorld@UObject@@UEBAPEAVUWorld@@XZ", "40 53 48 83 EC 20 8B 41 08 C1 E8 04 A8 01 0F 85 ? ? ? ?");
            FModuleManager::RegisterImport("?GetDesc@UObject@@UEAA?AVFString@@XZ", UObject_GetDesc);
            FModuleManager::RegisterImport("?Rename@UObject@@UEAA_NPEB_WPEAV1@I@Z", "44 89 4C 24 20 55 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 D9 48 81 EC A0 00 00 00");
            FModuleManager::RegisterImport("?PostReloadConfig@UObject@@UEAAXPEAVUProperty@@@Z", VoidSub);
            FModuleManager::RegisterImport("?PostEditImport@UObject@@UEAAXXZ", VoidSub);
            FModuleManager::RegisterImport("?ImportCustomProperties@UObject@@UEAAXPEB_WPEAVFFeedbackContext@@@Z", VoidSub);
            FModuleManager::RegisterImport("?ExportCustomProperties@UObject@@UEAAXAEAVFOutputDevice@@I@Z", VoidSub);
            FModuleManager::RegisterImport("?GetPrestreamPackages@UObject@@UEAAXAEAV?$TArray@PEAVUObject@@VFDefaultAllocator@@@@@Z", VoidSub);
            FModuleManager::RegisterImport("?GetPreloadDependencies@UObject@@UEAAXAEAV?$TArray@PEAVUObject@@VFDefaultAllocator@@@@@Z", "48 89 5C 24 10 48 89 74 24 18 57 48 83 EC 20 48 8B 79 18");
            FModuleManager::RegisterImport("?GetConfigOverridePlatform@UObject@@UEBAPEB_WXZ", NullSub);
            FModuleManager::RegisterImport("?IsPostLoadThreadSafe@UObject@@UEBA_NXZ", NullSub);
            FModuleManager::RegisterImport("?IsEditorOnly@UObject@@UEBA_NXZ", NullSub);
            FModuleManager::RegisterImport("?NeedsLoadForEditorGame@UObject@@UEBA_NXZ", NullSub);
            FModuleManager::RegisterImport("?NeedsLoadForTargetPlatform@UObject@@UEBA_NPEBVITargetPlatform@@@Z", TrueSub);
            FModuleManager::RegisterImport("?NeedsLoadForServer@UObject@@UEBA_NXZ", "48 83 EC 28 48 8B 51 18 48 8D 0D ? ? ? ? E8 ? ? ? ? 84 C0 0F 94 C0 48 83 C4 28 C3 CC CC 48 83 EC 28");
            FModuleManager::RegisterImport("?NeedsLoadForClient@UObject@@UEBA_NXZ", "48 83 EC 28 48 8B 51 18 48 8D 0D ? ? ? ? E8 ? ? ? ? 84 C0 0F 94 C0 48 83 C4 28 C3 CC CC 40 53");
            FModuleManager::RegisterImport("?PostDuplicate@UObject@@UEAAX_N@Z", VoidSub);
            FModuleManager::RegisterImport("?PostDuplicate@UObject@@UEAAXW4Type@EDuplicateMode@@@Z", "48 8B 01 83 FA 02 0F 94 C2");
            FModuleManager::RegisterImport("?PostRename@UObject@@UEAAXPEAV1@VFName@@@Z", VoidSub);
            FModuleManager::RegisterImport("?PostInterpChange@UObject@@UEAAXPEAVUProperty@@@Z", VoidSub);
            FModuleManager::RegisterImport("?ShutdownAfterError@UObject@@UEAAXXZ", VoidSub);
            FModuleManager::RegisterImport("?Serialize@UObject@@UEAAXAEAVFArchive@@@Z", base_module + 0x33D4700);
            FModuleManager::RegisterImport("?FinishDestroy@UObject@@UEAAXXZ", "40 57 48 83 EC 50 48 C7 44 24 30 FE FF FF FF 48 89 5C 24 68 48 89 74 24 70 48 8B F9 8B 41 08");
            FModuleManager::RegisterImport("?IsReadyForFinishDestroy@UObject@@UEAA_NXZ", "48 83 EC 28 48 81 C1 A8 03 00 00");
            FModuleManager::RegisterImport("?BeginDestroy@UObject@@UEAAXXZ", "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 48 8B 79 28 33 F6");
            FModuleManager::RegisterImport("?PostLoadSubobjects@UObject@@UEAAXPEAUFObjectInstancingGraph@@@Z", base_module + 0x33D42C0);
            FModuleManager::RegisterImport("?PostLoad@UObject@@UEAAXXZ", base_module + 0x62A000);
            FModuleManager::RegisterImport("?Modify@UObject@@UEAA_N_N@Z", "48 8B C4 88 50 10 57 41 54 41 55 41 56 41 57 48 83 EC 50 48 C7 40 A8 FE FF FF FF 48 89 58 08 48 89 68 18 48 89 70 20 44 0F B6 E2");
            FModuleManager::RegisterImport("?PreSave@UObject@@UEAAXPEBVITargetPlatform@@@Z", VoidSub);
            FModuleManager::RegisterImport("?PostSaveRoot@UObject@@UEAAX_N@Z", VoidSub);
            FModuleManager::RegisterImport("?PreSaveRoot@UObject@@UEAA_NPEB_W@Z", NullSub);
            FModuleManager::RegisterImport("?PostCDOContruct@UObject@@UEAAXXZ", VoidSub);
            FModuleManager::RegisterImport("?PostInitProperties@UObject@@UEAAXXZ", UObject_PostInitProperties);
            FModuleManager::RegisterImport("?GetDetailedInfoInternal@UObject@@MEBA?AVFString@@XZ", base_module + 0x05D21B0);
            FModuleManager::RegisterImport("?OnClusterMarkedAsPendingKill@UObjectBaseUtility@@UEAAXXZ", VoidSub);
            FModuleManager::RegisterImport("?CreateCluster@UObjectBaseUtility@@UEAAXXZ", base_module + 0x341EDE0);
            FModuleManager::RegisterImport("?CanBeInCluster@UObjectBaseUtility@@UEBA_NXZ", "0F B6 81 69 01 00 00");
            FModuleManager::RegisterImport("?CanBeClusterRoot@UObjectBaseUtility@@UEBA_NXZ", NullSub);
            FModuleManager::RegisterImport("?DeferredRegister@UObjectBase@@MEAAXPEAVUClass@@PEB_W1@Z", base_module + 0x341A580);
            FModuleManager::RegisterImport("?RegisterDependencies@UObjectBase@@MEAAXXZ", VoidSub);
            FModuleManager::RegisterImport("?StaticFindObjectFast@@YAPEAVUObject@@PEAVUClass@@PEAV1@VFName@@_N3W4EObjectFlags@@W4EInternalObjectFlags@@@Z", "40 57 48 83 EC 50 48 C7 44 24 40 FE FF FF FF 48 89 5C 24 60 48 89 6C 24 68 48 89 74 24 70 41 0F B6 F9");
            FModuleManager::RegisterImport("?GetConvertedDynamicPackageNameToTypeName@@YAAEAV?$TMap@VFName@@V1@VFDefaultSetAllocator@@U?$TDefaultMapHashableKeyFuncs@VFName@@V1@$0A@@@@@XZ", "40 53 48 83 EC 30 48 C7 44 24 20 FE FF FF FF BA 28 00 00 00 65 48 8B 04 25 58 00 00 00 48 8B 08 8B 04 0A 39 05 ? ? ? ? 7F ? 48 8D 05 ? ? ? ? 48 83 C4 30 5B C3 48 8D 0D ? ? ? ? E8 ? ? ? ? 83 3D ? ? ? ? ? 75 ? 48 8D 1D ? ? ? ? 48 89 5C 24 40 48 8D 05 ? ? ? ? 48 89 44 24 40 48 8D 0D ? ? ? ? E8 ? ? ? ? 90 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B C3 48 83 C4 30 5B C3 CC CC CC CC CC CC CC CC CC 48 89 54 24 10");
            FModuleManager::RegisterImport("?UClassCompiledInDefer@@YAXPEAUFFieldCompiledInInfo@@PEB_W_KI@Z", UClassCompiledInDefer);
            FModuleManager::RegisterImport("?UObjectCompiledInDefer@@YAXP6APEAVUClass@@XZ0PEB_W1_N1P6AXAEAV?$TMap@VFName@@V1@VFDefaultSetAllocator@@U?$TDefaultMapHashableKeyFuncs@VFName@@V1@$0A@@@@@@Z@Z", "40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 F9 48 81 EC F8 00 00 00 48 C7 44 24 70 FE FF FF FF");
            FModuleManager::RegisterImport("?StaticClass@UObject@@SAPEAVUClass@@XZ", "48 81 EC 98 00 00 00 48 83 3D ? ? ? ? ? 0F 85 ? ? ? ? E8 ? ? ? ? 48 89 44 24 70 E8 ? ? ? ? 48 89 44 24 78 48 8D 0D ? ? ? ? 48 83 C1 02 48 89 8C 24 80 00 00 00 E8 ? ? ? ? C6 44 24 68 00 48 8D 0D ? ? ? ? 48 89 4C 24 60 48 8D 0D ? ? ? ? 48 89 4C 24 58 48 8D 0D ? ? ? ? 48 89 4C 24 50 48 8D 0D ? ? ? ? 48 89 4C 24 48 48 8D 0D ? ? ? ? 48 89 4C 24 40 48 8B 4C 24 70 48 89 4C 24 38 48 8B 4C 24 78 48 89 4C 24 30 C7 44 24 28 01 01 00 00");
            FModuleManager::RegisterImport("?StaticConfigName@UObject@@SAPEB_WXZ", UObject_StaticConfigName);
            FModuleManager::RegisterImport("??0UObject@@QEAA@AEAVFVTableHelper@@@Z", "48 89 4C 24 08 53 55 56 57 41 56 48 83 EC 60 48 C7 44 24 30 FE FF FF FF");
            FModuleManager::RegisterImport("?AddReferencedObjects@UObject@@SAXPEAV1@AEAVFReferenceCollector@@@Z", VoidSub);
			FModuleManager::RegisterImport("?StaticLink@UStruct@@QEAAX_N@Z", "40 57 48 81 EC C0 00 00 00 48 C7 44 24 20 FE FF FF FF 48 89 9C 24 D0 00 00 00 0F B6 DA");
			FModuleManager::RegisterImport("?LinkChild@UStruct@@QEAAXPEAVUField@@@Z", "48 8B 41 50 48 89 42 40");
			FModuleManager::RegisterImport("?SetCppTypeInfo@UClass@@QEAAXPEAUICppClassTypeInfo@@@Z", "48 89 91 20 01 00 00 C3");
			FModuleManager::RegisterImport("?GetPrivateStaticClassBody@@YAXPEB_W0AEAPEAVUClass@@P6AXXZIW4EClassFlags@@_K0P6AXAEBVFObjectInitializer@@@ZP6APEAVUObject@@AEAVFVTableHelper@@@ZP6AXPEAV4@AEAVFReferenceCollector@@@ZP6APEAV1@XZP6APEAV1@XZ_N@Z", "48 89 5C 24 10 48 89 6C 24 18 57 41 56 41 57 48 83 EC 60");
            FModuleManager::RegisterImport("?Z_Construct_UClass_UObject@@YAPEAVUClass@@XZ", "48 83 EC 48 48 83 3D ? ? ? ? ? 0F 85 ? ? ? ? E8 ? ? ? ? E8 ? ? ? ? 48 89 05 ? ? ? ?");
            FModuleManager::RegisterImport("?StaticClass@UPackage@@SAPEAVUClass@@XZ", "4C 8B DC 48 83 EC 78 48 8B 05 ? ? ? ? 48 85 C0 0F 85 ? ? ? ? 88 44 24 68 4C 8D 0D ? ? ? ? 48 8D 05 ? ? ? ? 49 89 43 E8 4C 8D 05 ? ? ? ? 49 89 43 E0");
            FModuleManager::RegisterImport("?AddToCluster@UObjectBaseUtility@@UEAAXPEAV1@_N@Z", base_module + 0x341E830);
            FModuleManager::RegisterImport("?OverridePerObjectConfigSection@UObject@@UEAAXAEAVFString@@@Z", VoidSub);
            FModuleManager::RegisterImport("?UObjectForceRegistration@@YAXPEAVUObjectBase@@@Z", "48 89 5C 24 18 48 89 74 24 20 41 56 48 83 EC 20 4C 8B F1");

            SignatureContainer process_event{
                {{"C1 E8 1D A8 01 0F 85 ? ? ? ? F7 86 A0 00 00 00 00 04 00 00"}},
                    [&](const SignatureContainer & self) {
                        if (!self.get_match_address())
                            Output::send(std::format(STR("Failed to resolve symbol '?ProcessEvent@UObject@@UEAAXPEAVUFunction@@PEAX@Z': Signature not found\n")));
                        else
                            FModuleManager::RegisterImport("?ProcessEvent@UObject@@UEAAXPEAVUFunction@@PEAX@Z", self.get_match_address() - 0x6C);
                        return true;
                },
                [](SignatureContainer & self) {},
            };

            SinglePassScanner::SignatureContainerMap signature_containers = {
                {ScanTarget::MainExe, {process_event}},
            };

            SinglePassScanner::start_scan(signature_containers);

            FModuleManager::RegisterImport("?GetFunctionCallspace@UObject@@UEAAHPEAVUFunction@@PEAXPEAUFFrame@@@Z", Return2);
            FModuleManager::RegisterImport("?CallRemoteFunction@UObject@@UEAA_NPEAVUFunction@@PEAXPEAUFOutParmRec@@PEAUFFrame@@@Z", NullSub);
            FModuleManager::RegisterImport("?ProcessConsoleExec@UObject@@UEAA_NPEB_WAEAVFOutputDevice@@PEAV1@@Z", ProcessConsoleExec);
            FModuleManager::RegisterImport("?RegenerateClass@UObject@@UEAAPEAVUClass@@PEAV2@PEAV1@AEAV?$TArray@PEAVUObject@@VFDefaultAllocator@@@@@Z", NullSub);
            FModuleManager::RegisterImport("?MarkAsEditorOnlySubobject@UObject@@UEAAXXZ", VoidSub);
            FModuleManager::RegisterImport("?CheckDefaultSubobjectsInternal@UObject@@MEAA_NXZ", "40 53 48 83 EC 20 48 8B D9 E8 ? ? ? ? 84 C0 74 ? 48 8B CB 48 83 C4 20 5B E9 ? ? ? ? 48 83 C4 20 5B C3 CC CC CC CC CC CC CC CC CC CC CC 48 8B C4");
            FModuleManager::RegisterImport("?SetNetPushIdDynamic@UObject@@EEAAXH@Z", VoidSub);
            FModuleManager::RegisterImport("?GetNetPushIdDynamic@UObject@@EEBAHXZ", FSub);
            FModuleManager::RegisterImport("??0UObject@@QEAA@XZ", "48 89 4C 24 08 53 55 56 57 41 56 48 83 EC 60 48 C7 44 24 30 FE FF FF FF");
            FModuleManager::RegisterImport("??1UObject@@UEAA@XZ", VoidSub);
            FModuleManager::RegisterImport("?StaticAllocateObject@@YAPEAVUObject@@PEAVUClass@@PEAV1@VFName@@W4EObjectFlags@@W4EInternalObjectFlags@@_NPEA_N@Z", base_module + 0x34250B0);
            FModuleManager::RegisterImport("?BuildSubobjectMapping@UObject@@UEBAXPEAV1@AEAV?$TMap@PEAVUObject@@PEAV1@VFDefaultSetAllocator@@U?$TDefaultMapHashableKeyFuncs@PEAVUObject@@PEAV1@$0A@@@@@@Z", "48 89 54 24 10 48 83 EC 68");
            FModuleManager::RegisterImport("?IsDestructionThreadSafe@UObject@@UEBA_NXZ", TrueSub);
            FModuleManager::RegisterImport("??4FText@@QEAAAEAV0@AEBV0@@Z", FText_ConstOperatorEqual);
            FModuleManager::RegisterImport("??0FName@@QEAA@PEBDW4EFindName@@@Z", FName_CharConstructor);
            FModuleManager::RegisterImport("??1FOutputDeviceNull@@UEAA@XZ", VoidSub);
            FModuleManager::RegisterImport("??0FOutputDeviceNull@@QEAA@XZ", VoidSub);
            FModuleManager::RegisterImport("??1FText@@QEAA@XZ", FText_Destructor);
            FModuleManager::RegisterImport("??4FText@@QEAAAEAV0@$$QEAV0@@Z", FText_OperatorEqual);
            FModuleManager::RegisterImport("??0FText@@QEAA@$$QEAV0@@Z", FText_CopyConstructor);
            FModuleManager::RegisterImport("??0FText@@QEAA@AEBV0@@Z", FText_CopyConstructor);
            FModuleManager::RegisterImport("??0FText@@QEAA@XZ", FText_Constructor);
            FModuleManager::RegisterImport("??0FString@@QEAA@AEBV0@@Z", "48 89 4C 24 08 57 48 83 EC 30 48 C7 44 24 20 FE FF FF FF 48 89 5C 24 48 48 89 74 24 50 48 8B D9 48 89 4C 24 40 33 C0 48 89 01 48 63 7A 08 48 8B 32 89 79 08 85 FF 75 ? 89 41 0C EB ? 45 33 C0 8B D7 E8 ? ? ? ? 4C 8B C7 4D 03 C0");
            FModuleManager::RegisterImport("??0FString@@QEAA@XZ", VoidSub);
            FModuleManager::RegisterImport("??0FObjectInitializer@@QEAA@XZ", base_module + 0x3425A90);
            FModuleManager::RegisterImport("??1FObjectInitializer@@QEAA@XZ", base_module + 0x3425D20);
            FModuleManager::RegisterImport("?UObjectCompiledInDeferStruct@@YAXP6APEAVUScriptStruct@@XZPEB_WVFName@@_N1@Z", UObjectCompiledInDeferStruct);
            FModuleManager::RegisterImport("?FindExistingStructIfHotReloadOrDynamic@@YAPEAVUScriptStruct@@PEAVUObject@@PEB_W_KI_N@Z", base_module + 0x341C800);
            FModuleManager::RegisterImport("??0ICppStructOps@UScriptStruct@@QEAA@HH@Z", ICppStructOps_Constructor);
            FModuleManager::RegisterImport("??1ICppStructOps@UScriptStruct@@UEAA@XZ", VoidSub);
            FModuleManager::RegisterImport("?GetPrivateStaticClass@UScriptStruct@@CAPEAVUClass@@XZ", base_module + 0x1BDBE80);
            FModuleManager::RegisterImport("??0UScriptStruct@@QEAA@AEBVFObjectInitializer@@PEAV0@PEAUICppStructOps@0@W4EStructFlags@@_K4@Z", base_module + 0x32F1750);
            FModuleManager::RegisterImport("?DeferCppStructOps@UScriptStruct@@SAXVFName@@PEAUICppStructOps@1@@Z", base_module + 0x32F16C0);
            FModuleManager::RegisterImport("??2UFunction@@SAPEAX_KW4EInternal@@PEAVUObject@@VFName@@W4EObjectFlags@@@Z", base_module + 0x1BDA4A0);
            FModuleManager::RegisterImport("??0UFunction@@QEAA@AEBVFObjectInitializer@@PEAV0@W4EFunctionFlags@@G_K@Z", base_module + 0x32F6A00);
            FModuleManager::RegisterImport("??1UFunction@@UEAA@XZ", "40 57 48 83 EC 30 48 C7 44 24 20 FE FF FF FF 48 89 5C 24 40 8B FA 48 8B D9 48 8B 89 90 00 00 00 48 85 C9 74 ? E8 ? ? ? ? 90 48 8B 4B 60");
            FModuleManager::RegisterImport("?AddFunctionToFunctionMapWithOverriddenName@UClass@@QEAAXPEAVUFunction@@VFName@@@Z", "4C 8B DC 4D 89 43 18 49 89 53 10 48 83 EC 38 49 8D 43 18");
            FModuleManager::RegisterImport("??2UIntProperty@@SAPEAX_KW4EInternal@@PEAVUObject@@VFName@@W4EObjectFlags@@@Z", base_module + 0x1BD98D0);
            FModuleManager::RegisterImport("??0UIntProperty@@QEAA@AEBVFObjectInitializer@@W4ECppProperty@@H_K@Z", base_module + 0x1BD9880);
            FModuleManager::RegisterImport("??1UIntProperty@@UEAA@XZ", "40 57 41 56 41 57 48 83 EC 30 48 C7 44 24 20 FE FF FF FF 48 89 5C 24 58 48 89 6C 24 60 48 89 74 24 68 48 8B F9 48 8D 05 ? ? ? ? 48 89 01 80 3D ? ? ? ? ?");
            FModuleManager::RegisterImport("??2UBoolProperty@@SAPEAX_KW4EInternal@@PEAVUObject@@VFName@@W4EObjectFlags@@@Z", base_module + 0x1BDB180);
            FModuleManager::RegisterImport("??0UBoolProperty@@QEAA@AEBVFObjectInitializer@@W4ECppProperty@@H_KII_N@Z", base_module + 0x33E83D0);
            FModuleManager::RegisterImport("??1UBoolProperty@@UEAA@XZ", "40 57 41 56 41 57 48 83 EC 30 48 C7 44 24 20 FE FF FF FF 48 89 5C 24 58 48 89 6C 24 60 48 89 74 24 68 48 8B F9 48 8D 05 ? ? ? ? 48 89 01 80 3D ? ? ? ? ?");
            FModuleManager::RegisterImport("??2UObjectProperty@@SAPEAX_KW4EInternal@@PEAVUObject@@VFName@@W4EObjectFlags@@@Z", base_module + 0x1BDAE10);
            FModuleManager::RegisterImport("??0UObjectProperty@@QEAA@AEBVFObjectInitializer@@W4ECppProperty@@H_KPEAVUClass@@@Z", base_module + 0x1BDADC0);
            FModuleManager::RegisterImport("??1UObjectProperty@@UEAA@XZ", base_module + 0x341A3E0);
            FModuleManager::RegisterImport("??2UStrProperty@@SAPEAX_KW4EInternal@@PEAVUObject@@VFName@@W4EObjectFlags@@@Z", base_module + 0x1BD8FA0);
            FModuleManager::RegisterImport("??0UStrProperty@@QEAA@AEBVFObjectInitializer@@W4ECppProperty@@H_K@Z", base_module + 0x1BD8F40);
            FModuleManager::RegisterImport("??1UStrProperty@@UEAA@XZ", "40 57 41 56 41 57 48 83 EC 30 48 C7 44 24 20 FE FF FF FF 48 89 5C 24 58 48 89 6C 24 60 48 89 74 24 68 48 8B F9 48 8D 05 ? ? ? ? 48 89 01 80 3D ? ? ? ? ?");
            FModuleManager::RegisterImport("??2UArrayProperty@@SAPEAX_KW4EInternal@@PEAVUObject@@VFName@@W4EObjectFlags@@@Z", base_module + 0x1BDABD0);
            FModuleManager::RegisterImport("??0UArrayProperty@@QEAA@AEBVFObjectInitializer@@W4ECppProperty@@H_K@Z", base_module + 0x1BDAAE0);
            FModuleManager::RegisterImport("??1UArrayProperty@@UEAA@XZ", "40 57 41 56 41 57 48 83 EC 30 48 C7 44 24 20 FE FF FF FF 48 89 5C 24 58 48 89 6C 24 60 48 89 74 24 68 48 8B F9 48 8D 05 ? ? ? ? 48 89 01 80 3D ? ? ? ? ?");
            FModuleManager::RegisterImport("??2UStructProperty@@SAPEAX_KW4EInternal@@PEAVUObject@@VFName@@W4EObjectFlags@@@Z", base_module + 0x1BDB070);
            FModuleManager::RegisterImport("??0UStructProperty@@QEAA@AEBVFObjectInitializer@@W4ECppProperty@@H_KPEAVUScriptStruct@@@Z", base_module + 0x3405F90);
            FModuleManager::RegisterImport("??1UStructProperty@@UEAA@XZ", "40 57 41 56 41 57 48 83 EC 30 48 C7 44 24 20 FE FF FF FF 48 89 5C 24 58 48 89 6C 24 60 48 89 74 24 68 48 8B F9 48 8D 05 ? ? ? ? 48 89 01 80 3D ? ? ? ? ?");
            FModuleManager::RegisterImport("??0DetermineBitfieldOffsetAndMask@@QEAA@XZ", "48 8D 05 ? ? ? ? 48 89 01 33 C0 48 89 41 08 48 8B C1");
            FModuleManager::RegisterImport("?DoDetermineBitfieldOffsetAndMask@DetermineBitfieldOffsetAndMask@@QEAAX_K@Z", "48 89 5C 24 08 48 89 74 24 10 48 89 7C 24 18 41 56 48 83 EC 20 48 8B 1D ? ? ? ?");
            FModuleManager::RegisterImport("??2UTextProperty@@SAPEAX_KW4EInternal@@PEAVUObject@@VFName@@W4EObjectFlags@@@Z", base_module + 0x2251490);
            FModuleManager::RegisterImport("??0UTextProperty@@QEAA@AEBVFObjectInitializer@@W4ECppProperty@@H_K@Z", base_module + 0x2251440);
            FModuleManager::RegisterImport("??1UTextProperty@@UEAA@XZ", "40 57 41 56 41 57 48 83 EC 30 48 C7 44 24 20 FE FF FF FF 48 89 5C 24 58 48 89 6C 24 60 48 89 74 24 68 48 8B F9 48 8D 05 ? ? ? ? 48 89 01 80 3D ? ? ? ? ?");
            FModuleManager::RegisterImport("?Z_Construct_UScriptStruct_FVector2D@@YAPEAVUScriptStruct@@XZ", "48 81 EC 08 05 00 00 48 C7 84 24 D0 00 00 00 FE FF FF FF 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 F0 04 00 00 C7 44 24 40 00 00 00 00 E8 ? ? ? ? 48 89 44 24 50 B8 28 00 00 00 8B C0 65 48 8B 0C 25 58 00 00 00 48 8B 09 8B 04 08 39 05 ? ? ? ? 7E ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 83 3D ? ? ? ? ? 75 ? E8 ? ? ? ? C6 44 24 20 00 44 8B C8 41 B8 08 00 00 00 48 8D 15 ? ? ? ? 48 8B 4C 24 50 E8 ? ? ? ? 48 89 05 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 83 3D ? ? ? ? ? 0F 85 ? ? ? ? 48 8D 84 24 D8 00 00 00 48 89 84 24 88 00 00 00 B8 01 00 00 00 83 C8 40 83 C8 04 89 44 24 44 41 B8 01 00 00 00 48 8D 15 ? ? ? ? 48 8B 8C 24 88 00 00 00 E8 ? ? ? ? 8B 4C 24 44 89 4C 24 20 4C 8B 08 4C 8B 44 24 50 33 D2 B9 B0 00 00 00 E8 ? ? ? ? 48 89 44 24 58 48 83 7C 24 58 00 74 ? 48 8D 8C 24 00 01 00 00 E8 ? ? ? ? 48 89 84 24 90 00 00 00 48 8B 84 24 90 00 00 00 48 89 84 24 98 00 00 00 8B 44 24 40 83 C8 01 89 44 24 40 48 C7 44 24 30 04 00 00 00 48 C7 44 24 28 08 00 00 00 C7 44 24 20 38 00 00 00 45 33 C9 45 33 C0 48 8B 94 24 98 00 00 00 48 8B 4C 24 58 E8 ? ? ? ? 48 89 44 24 60 EB ? 48 C7 44 24 60 00 00 00 00 48 8B 44 24 60 48 89 05 ? ? ? ? 8B 44 24 40 83 E0 01 85 C0 74 ? 83 64 24 40 FE 48 8D 8C 24 00 01 00 00 E8 ? ? ? ? 48 8D 84 24 E0 00 00 00 48 89 84 24 A0 00 00 00 B8 01 00 00 00 83 C8 40 83 C8 04 89 44 24 48 41 B8 01 00 00 00 48 8D 15 ? ? ? ? 48 8B 8C 24 A0 00 00 00 E8 ? ? ? ? 8B 4C 24 48 89 4C 24 20 4C 8B 08 4C 8B 05 ? ? ? ? 33 D2 B9 90 00 00 00 E8 ? ? ? ? 48 89 44 24 68 48 83 7C 24 68 00 74 ? 48 8D 8C 24 50 02 00 00 E8 ? ? ? ? 48 89 84 24 A8 00 00 00 48 8B 84 24 A8 00 00 00 48 89 84 24 B0 00 00 00 8B 44 24 40 83 C8 02 89 44 24 40 48 B8 05 00 00 01 00 00 10 00 48 89 44 24 20 41 B9 04 00 00 00 45 33 C0 48 8B 94 24 B0 00 00 00 48 8B 4C 24 68 E8 ? ? ? ? 48 89 44 24 70 EB ? 48 C7 44 24 70 00 00 00 00 48 8B 44 24 70 48 89 84 24 E8 00 00 00 8B 44 24 40 83 E0 02 85 C0 74 ? 83 64 24 40 FD 48 8D 8C 24 50 02 00 00 E8 ? ? ? ? 48 8D 84 24 F0 00 00 00 48 89 84 24 B8 00 00 00 B8 01 00 00 00 83 C8 40 83 C8 04 89 44 24 4C 41 B8 01 00 00 00 48 8D 15 ? ? ? ? 48 8B 8C 24 B8 00 00 00 E8 ? ? ? ? 8B 4C 24 4C 89 4C 24 20 4C 8B 08 4C 8B 05 ? ? ? ? 33 D2 B9 90 00 00 00 E8 ? ? ? ? 48 89 44 24 78 48 83 7C 24 78 00 74 ? 48 8D 8C 24 A0 03 00 00 E8 ? ? ? ? 48 89 84 24 C0 00 00 00 48 8B 84 24 C0 00 00 00 48 89 84 24 C8 00 00 00 8B 44 24 40 83 C8 04 89 44 24 40 48 B8 05 00 00 01 00 00 10 00 48 89 44 24 20 45 33 C9 45 33 C0 48 8B 94 24 C8 00 00 00 48 8B 4C 24 78 E8 ? ? ? ? 48 89 84 24 80 00 00 00 EB ? 48 C7 84 24 80 00 00 00 00 00 00 00 48 8B 84 24 80 00 00 00 48 89 84 24 F8 00 00 00 8B 44 24 40 83 E0 04 85 C0 74 ? 83 64 24 40 FB 48 8D 8C 24 A0 03 00 00 E8 ? ? ? ? 33 D2 48 8B 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 8B 8C 24 F0 04 00 00 48 33 CC E8 ? ? ? ? 48 81 C4 08 05 00 00 C3 CC B8 04 6F 4B 0A");
            FModuleManager::RegisterImport("?Z_Construct_UClass_UObject_NoRegister@@YAPEAVUClass@@XZ", base_module + 0x344EA40);
            FModuleManager::RegisterImport("?AddCppProperty@UArrayProperty@@UEAAXPEAVUProperty@@@Z", base_module + 0x1169DA0);
            FModuleManager::RegisterImport("?AddCppProperty@UField@@UEAAXPEAVUProperty@@@Z", "48 83 EC 28 4C 8D 05 ? ? ? ? BA BE 00 00 00");
            FModuleManager::RegisterImport("?AddCppProperty@UStruct@@UEAAXPEAVUProperty@@@Z", "48 8B 41 50 48 89 42 40");
            FModuleManager::RegisterImport("?AllowCrossLevel@UObjectPropertyBase@@MEBA_NXZ", NullSub);
            FModuleManager::RegisterImport("?ArePropertyGuidsAvailable@UStruct@@MEBA_NXZ", NullSub);
            FModuleManager::RegisterImport("?BeginDestroy@UObjectPropertyBase@@UEAAXXZ", base_module + 0x33E6680);
            FModuleManager::RegisterImport("?Bind@UField@@UEAAXXZ", VoidSub);
            FModuleManager::RegisterImport("?Bind@UFunction@@UEAAXXZ", base_module + 0x32F7270);
            FModuleManager::RegisterImport("?CanHoldDoubleValueInternal@?$TProperty_Numeric@H@@EEBA_NN@Z", base_module + 0x1BD9850);
            FModuleManager::RegisterImport("?CanHoldSignedValueInternal@?$TProperty_Numeric@H@@EEBA_N_J@Z", base_module + 0x1BD9870);
            FModuleManager::RegisterImport("?CanHoldUnsignedValueInternal@?$TProperty_Numeric@H@@EEBA_N_K@Z", base_module + 0x1BD9870);
            FModuleManager::RegisterImport("?CheckValidObject@UObjectPropertyBase@@MEBAXPEAX@Z", base_module + 0x33E7EC0);
            FModuleManager::RegisterImport("?ClearValueInternal@?$TProperty@HVUNumericProperty@@@@UEBAXPEAX@Z", base_module + 0x1BD9780);
            FModuleManager::RegisterImport("?ClearValueInternal@?$TProperty@PEAVUObject@@VUObjectPropertyBase@@@@UEBAXPEAX@Z", base_module + 0x697480);
            FModuleManager::RegisterImport("?ClearValueInternal@?$TProperty@VFString@@VUProperty@@@@UEBAXPEAX@Z", "48 83 EC 48 48 C7 44 24 20 FE FF FF FF 48 8B C2");
            FModuleManager::RegisterImport("?ClearValueInternal@?$TProperty@VFText@@VUProperty@@@@UEBAXPEAX@Z", base_module + 0x2251270);
            FModuleManager::RegisterImport("?ClearValueInternal@UArrayProperty@@UEBAXPEAX@Z", base_module + 0x33E43E0);
            FModuleManager::RegisterImport("?ClearValueInternal@UBoolProperty@@UEBAXPEAX@Z", base_module + 0x33E9A40);
            FModuleManager::RegisterImport("?ClearValueInternal@UStructProperty@@UEBAXPEAX@Z", base_module + 0x3406800);
            FModuleManager::RegisterImport("?ContainsObjectReference@?$TUObjectPropertyBase@PEAVUObject@@@@UEBA_NAEAV?$TArray@PEBVUStructProperty@@VFDefaultAllocator@@@@@Z", TrueSub);
            FModuleManager::RegisterImport("?ContainsObjectReference@UArrayProperty@@UEBA_NAEAV?$TArray@PEBVUStructProperty@@VFDefaultAllocator@@@@@Z", base_module + 0x33B1270);
            FModuleManager::RegisterImport("?ContainsObjectReference@UProperty@@UEBA_NAEAV?$TArray@PEBVUStructProperty@@VFDefaultAllocator@@@@@Z", NullSub);
            FModuleManager::RegisterImport("?ContainsObjectReference@UStructProperty@@UEBA_NAEAV?$TArray@PEBVUStructProperty@@VFDefaultAllocator@@@@@Z", base_module + 0x33B12F0);
            FModuleManager::RegisterImport("?ContainsWeakObjectReference@?$TUObjectPropertyBase@PEAVUObject@@@@UEBA_NXZ", NullSub);
            FModuleManager::RegisterImport("?ContainsWeakObjectReference@UArrayProperty@@UEBA_NXZ", base_module + 0x33B13E0);
            FModuleManager::RegisterImport("?ContainsWeakObjectReference@UProperty@@UEBA_NXZ", NullSub);
            FModuleManager::RegisterImport("?ContainsWeakObjectReference@UStructProperty@@UEBA_NXZ", base_module + 0x33B1440);
            FModuleManager::RegisterImport("?ConvertFromType@?$TProperty_Numeric@H@@UEAA_NAEBUFPropertyTag@@AEAVFArchive@@PEAEPEAVUStruct@@AEA_N@Z", base_module + 0x1BD9270);
            FModuleManager::RegisterImport("?ConvertFromType@UArrayProperty@@UEAA_NAEBUFPropertyTag@@AEAVFArchive@@PEAEPEAVUStruct@@AEA_N@Z", base_module + 0x33E4790);
            FModuleManager::RegisterImport("?ConvertFromType@UBoolProperty@@UEAA_NAEBUFPropertyTag@@AEAVFArchive@@PEAEPEAVUStruct@@AEA_N@Z", base_module + 0x33E8D70);
            FModuleManager::RegisterImport("?ConvertFromType@UObjectProperty@@UEAA_NAEBUFPropertyTag@@AEAVFArchive@@PEAEPEAVUStruct@@AEA_N@Z", base_module + 0x3400AE0);
            FModuleManager::RegisterImport("?ConvertFromType@UStrProperty@@UEAA_NAEBUFPropertyTag@@AEAVFArchive@@PEAEPEAVUStruct@@AEA_N@Z", "48 8B C4 55 41 54 41 55 41 56 41 57 48 8B EC 48 81 EC 80 00 00 00 48 C7 45 A8 FE FF FF FF 48 89 58 08 48 89 70 18");
            FModuleManager::RegisterImport("?ConvertFromType@UStructProperty@@UEAA_NAEBUFPropertyTag@@AEAVFArchive@@PEAEPEAVUStruct@@AEA_N@Z", base_module + 0x3406AA0);
            FModuleManager::RegisterImport("?ConvertFromType@UTextProperty@@UEAA_NAEBUFPropertyTag@@AEAVFArchive@@PEAEPEAVUStruct@@AEA_N@Z", base_module + 0x3416A00);
            FModuleManager::RegisterImport("?CopyCompleteValueFromScriptVM@UObjectPropertyBase@@UEBAXPEAXPEBX@Z", base_module + 0x33E8200);
            FModuleManager::RegisterImport("?CopyCompleteValueFromScriptVM@UProperty@@UEBAXPEAXPEBX@Z", base_module + 0x33E07C0);
            FModuleManager::RegisterImport("?CopyCompleteValueToScriptVM@UObjectPropertyBase@@UEBAXPEAXPEBX@Z", base_module + 0x33E8170);
            FModuleManager::RegisterImport("?CopyCompleteValueToScriptVM@UProperty@@UEBAXPEAXPEBX@Z", base_module + 0x33E07C0);
            FModuleManager::RegisterImport("?CopySingleValueFromScriptVM@UObjectPropertyBase@@UEBAXPEAXPEBX@Z", base_module + 0x33E8190);
            FModuleManager::RegisterImport("?CopySingleValueFromScriptVM@UProperty@@UEBAXPEAXPEBX@Z", base_module + 0x33E0780);
            FModuleManager::RegisterImport("?CopySingleValueToScriptVM@UProperty@@UEBAXPEAXPEBX@Z", base_module + 0x33E0780);
            FModuleManager::RegisterImport("?CopySingleValueToScriptVM@UObjectPropertyBase@@UEBAXPEAXPEBX@Z", base_module + 0x3401220);
            FModuleManager::RegisterImport("?CopyValuesInternal@?$TProperty@HVUNumericProperty@@@@UEBAXPEAXPEBXH@Z", base_module + 0x1BD9740);
            FModuleManager::RegisterImport("?CopyValuesInternal@?$TProperty@PEAVUObject@@VUObjectPropertyBase@@@@UEBAXPEAXPEBXH@Z", base_module + 0x1BDA2E0);
            FModuleManager::RegisterImport("?CopyValuesInternal@?$TProperty@VFString@@VUProperty@@@@UEBAXPEAXPEBXH@Z", "45 85 C9 7E ? 48 89 5C 24 10 56");
            FModuleManager::RegisterImport("?CopyValuesInternal@?$TProperty@VFText@@VUProperty@@@@UEBAXPEAXPEBXH@Z", base_module + 0x2251210);
            FModuleManager::RegisterImport("?CopyValuesInternal@UArrayProperty@@UEBAXPEAXPEBXH@Z", base_module + 0x33E4160);
            FModuleManager::RegisterImport("?CopyValuesInternal@UBoolProperty@@UEBAXPEAXPEBXH@Z", base_module + 0x33E99E0);
            FModuleManager::RegisterImport("?CopyValuesInternal@UStructProperty@@UEBAXPEAXPEBXH@Z", base_module + 0x34067D0);
            FModuleManager::RegisterImport("?DestroyStruct@UStruct@@UEBAXPEAXH@Z", base_module + 0x32EE2E0);
            FModuleManager::RegisterImport("?DestroyValueInternal@?$TProperty@HVUNumericProperty@@@@UEBAXPEAX@Z", VoidSub);
            FModuleManager::RegisterImport("?DestroyValueInternal@?$TProperty@PEAVUObject@@VUObjectPropertyBase@@@@UEBAXPEAX@Z", VoidSub);
            FModuleManager::RegisterImport("?DestroyValueInternal@?$TProperty@VFString@@VUProperty@@@@UEBAXPEAX@Z", "40 57 48 83 EC 30 48 C7 44 24 20 FE FF FF FF 48 89 5C 24 40 48 89 74 24 48 48 8B F2 48 8B F9 33 DB 39 59 48 7E ? 66 66 0F 1F 84 00 00 00 00 00 8B C3 0F AF 47 4C 4C 63 C0 49 8B 0C 30");
            FModuleManager::RegisterImport("?DestroyValueInternal@?$TProperty@VFText@@VUProperty@@@@UEBAXPEAX@Z", base_module + 0x2251320);
            FModuleManager::RegisterImport("?DestroyValueInternal@UArrayProperty@@UEBAXPEAX@Z", base_module + 0x33E44B0);
            FModuleManager::RegisterImport("?DestroyValueInternal@UProperty@@MEBAXPEAX@Z", VoidSub);
            FModuleManager::RegisterImport("?DestroyValueInternal@UStructProperty@@UEBAXPEAX@Z", base_module + 0x3406950);
            FModuleManager::RegisterImport("?EmitReferenceInfo@UArrayProperty@@UEAAXAEAVUClass@@HAEAV?$TArray@PEBVUStructProperty@@VFDefaultAllocator@@@@@Z", base_module + 0x33B18A0);
            FModuleManager::RegisterImport("?EmitReferenceInfo@UObjectProperty@@UEAAXAEAVUClass@@HAEAV?$TArray@PEBVUStructProperty@@VFDefaultAllocator@@@@@Z", base_module + 0x33B1770);
            FModuleManager::RegisterImport("?EmitReferenceInfo@UProperty@@UEAAXAEAVUClass@@HAEAV?$TArray@PEBVUStructProperty@@VFDefaultAllocator@@@@@Z", VoidSub);
            FModuleManager::RegisterImport("?EmitReferenceInfo@UStructProperty@@UEAAXAEAVUClass@@HAEAV?$TArray@PEBVUStructProperty@@VFDefaultAllocator@@@@@Z", base_module + 0x33B1EF0);
            FModuleManager::RegisterImport("?ExportTextItem@UArrayProperty@@UEBAXAEAVFString@@PEBX1PEAVUObject@@H2@Z", base_module + 0x33E3850);
            FModuleManager::RegisterImport("?ExportTextItem@UBoolProperty@@UEBAXAEAVFString@@PEBX1PEAVUObject@@H2@Z", base_module + 0x33E92E0);
            FModuleManager::RegisterImport("?ExportTextItem@UNumericProperty@@UEBAXAEAVFString@@PEBX1PEAVUObject@@H2@Z", base_module + 0x3400720);
            FModuleManager::RegisterImport("?ExportTextItem@UStrProperty@@UEBAXAEAVFString@@PEBX1PEAVUObject@@H2@Z", "48 8B C4 41 56 48 83 EC 60 48 C7 40 B8 FE FF FF FF 48 89 58 08 48 89 68 10 48 89 70 18 48 89 78 20 4D 8B C8");
            FModuleManager::RegisterImport("?ExportTextItem@UStructProperty@@UEBAXAEAVFString@@PEBX1PEAVUObject@@H2@Z", base_module + 0x34066D0);
            FModuleManager::RegisterImport("?ExportTextItem@UTextProperty@@UEBAXAEAVFString@@PEBX1PEAVUObject@@H2@Z", base_module + 0x3416E20);
            FModuleManager::RegisterImport("?FindPropertyGuidFromName@UStruct@@MEBA?AUFGuid@@VFName@@@Z", base_module + 0x10DC450);
            FModuleManager::RegisterImport("?FindPropertyNameFromGuid@UStruct@@MEBA?AVFName@@AEBUFGuid@@@Z", base_module + 0x0597770);
            FModuleManager::RegisterImport("?FinishDestroy@UStruct@@UEAAXXZ", "40 53 48 83 EC 20 48 8B D9 48 81 C1 08 01 00 00 83 79 0C 00");
            FModuleManager::RegisterImport("?GetCPPMacroType@UArrayProperty@@UEBA?AVFString@@AEAV2@@Z", base_module + 0x33E3770);
            FModuleManager::RegisterImport("?GetCPPMacroType@UBoolProperty@@UEBA?AVFString@@AEAV2@@Z", "48 89 54 24 10 57 48 83 EC 50 48 C7 44 24 30 FE FF FF FF 48 89 5C 24 70 48 89 74 24 78 48 8B DA");
            FModuleManager::RegisterImport("?GetCPPMacroType@UObjectProperty@@UEBA?AVFString@@AEAV2@@Z", "48 89 54 24 10 56 57 41 56 48 83 EC 50 48 C7 44 24 20 FE FF FF FF 48 89 9C 24 80 00 00 00 4D 8B F0 48 8B F2 48 8B D9 48 8B 89 90 00 00 00 48 8B 01 FF 90 80 02 00 00 48 8B F8 48 8B 8B 90 00 00 00 4C 8B 49 20 4C 89 4C 24 70 48 8D 54 24 38 48 8D 4C 24 70 E8 ? ? ? ? 90 4C 8D 0D ? ? ? ? 83 7C 24 40 00 4C 0F 45 4C 24 38 4C 8B C7 48 8D 15 ? ? ? ? 48 8D 4C 24 28 E8 ? ? ? ? 90 48 8D 44 24 28 33 DB 4C 3B F0 74 ? 49 8B 0E 48 85 C9 74 ? E8 ? ? ? ? 48 8B 44 24 28 49 89 06 48 8B CB 48 89 5C 24 28 8B 44 24 30 41 89 46 08 8B 44 24 34 41 89 46 0C 48 89 5C 24 30 EB ? 48 8B 4C 24 28 48 85 C9 74 ? E8 ? ? ? ? 90 48 8B 4C 24 38 48 85 C9 74 ? E8 ? ? ? ? 90 48 89 1E 48 C7 46 08 07 00 00 00");
            FModuleManager::RegisterImport("?GetCPPMacroType@UProperty@@UEBA?AVFString@@AEAV2@@Z", base_module + 0x33E09E0);
            FModuleManager::RegisterImport("?GetCPPMacroType@UStructProperty@@UEBA?AVFString@@AEAV2@@Z", base_module + 0x3406600);
            FModuleManager::RegisterImport("?GetCPPType@?$TProperty@HVUNumericProperty@@@@UEBA?AVFString@@PEAV2@I@Z", "48 89 54 24 10 53 48 83 EC 30 48 C7 44 24 20 FE FF FF FF 48 8B DA 33 C0 48 89 02 48 C7 42 08 06 00 00 00 48 8B CA E8 ? ? ? ? 48 8B 0B F2 0F 10 05 ? ? ? ? F2 0F 11 01 8B 05 ? ? ? ? 89 41 08 48 8B C3 48 83 C4 30 5B C3 CC CC CC CC 48 B8 00 02 00 40 10 00 08 00 C7 41 4C 04 00 00 00");
            FModuleManager::RegisterImport("?GetCPPType@?$TProperty@VFString@@VUProperty@@@@UEBA?AVFString@@PEAV2@I@Z", "48 89 54 24 10 53 48 83 EC 30 48 C7 44 24 20 FE FF FF FF 48 8B DA 33 C0 48 89 02 48 C7 42 08 08 00 00 00 48 8B CA E8 ? ? ? ? 48 8B 03 0F 10 05 ? ? ? ? 0F 11 00 48 8B C3 48 83 C4 30 5B C3 CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC 48 B8 00 02 00 00 00 00 08 00");
            FModuleManager::RegisterImport("?GetCPPType@?$TProperty@VFText@@VUProperty@@@@UEBA?AVFString@@PEAV2@I@Z", base_module + 0x22511B0);
            FModuleManager::RegisterImport("?GetCPPType@?$TUObjectPropertyBase@PEAVUObject@@@@UEBA?AVFString@@PEAV2@I@Z", "48 8B C4 48 89 50 10 56 57 41 56 48 83 EC 60 48 C7 40 B8 FE FF FF FF 48 89 58 08 48 89 68 18 41 8B F1 49 8B E8 4C 8B F2 48 8B F9 48 8B 89 98 00 00 00 48 8B 01 FF 90 80 02 00 00 48 8B D8 4C 8B 97 98 00 00 00 4D 8B 5A 20 4C 89 9C 24 88 00 00 00 48 8D 54 24 38 48 8D 8C 24 88 00 00 00 E8 ? ? ? ? 90 4C 8D 0D ? ? ? ? 83 7C 24 40 00 4C 0F 45 4C 24 38 4C 8B C3 48 8D 15 ? ? ? ? 48 8D 4C 24 48 E8 ? ? ? ? 90 48 8B 07 48 8D 4C 24 48 48 89 4C 24 20 44 8B CE 4C 8B C5 49 8B D6 48 8B CF FF 90 20 03 00 00 90 48 8B 4C 24 48 48 85 C9 74 ? E8 ? ? ? ? 90 48 8B 4C 24 38 48 85 C9 74 ? E8 ? ? ? ? 90 49 8B C6 4C 8D 5C 24 60 49 8B 5B 20 49 8B 6B 30 49 8B E3 41 5E 5F 5E C3 CC CC CC CC CC CC CC CC CC CC CC CC CC 48 89 54 24 10");
            FModuleManager::RegisterImport("?GetCPPType@UArrayProperty@@UEBA?AVFString@@PEAV2@I@Z", base_module + 0x33E3650);
            FModuleManager::RegisterImport("?GetCPPType@UBoolProperty@@UEBA?AVFString@@PEAV2@I@Z", "4C 8B DC 49 89 53 10 57 48 83 EC 50 49 C7 43 D8 FE FF FF FF 49 89 5B 18");
            FModuleManager::RegisterImport("?GetCPPType@UStructProperty@@UEBA?AVFString@@PEAV2@I@Z", base_module + 0x3406550);
            FModuleManager::RegisterImport("?GetCPPTypeCustom@UObjectProperty@@UEBA?AVFString@@PEAV2@IAEBV2@@Z", "40 53 48 83 EC 20 4C 8B 44 24 50 48 8B DA 41 83 78 08 00 74 ? 4D 8B 00 EB ? 4C 8D 05 ? ? ? ? 48 8D 15 ? ? ? ? 48 8B CB E8 ? ? ? ? 48 8B C3 48 83 C4 20 5B C3 CC CC CC CC CC CC CC 48 89 54 24 10 57");
            FModuleManager::RegisterImport("?GetCPPTypeForwardDeclaration@?$TProperty_Numeric@H@@UEBA?AVFString@@XZ", "33 C0 48 89 02 48 89 42 08 48 8B C2 C3 CC CC CC 40 53");
            FModuleManager::RegisterImport("?GetCPPTypeForwardDeclaration@UArrayProperty@@UEBA?AVFString@@XZ", "40 53 48 83 EC 20 48 8B 89 90 00 00 00 48 8B DA 48 8B 01 FF 90 60 02 00 00");
            FModuleManager::RegisterImport("?GetCPPTypeForwardDeclaration@UBoolProperty@@UEBA?AVFString@@XZ", "33 C0 48 89 02 48 89 42 08 48 8B C2 C3 CC CC CC 40 53");
            FModuleManager::RegisterImport("?GetCPPTypeForwardDeclaration@UStrProperty@@UEBA?AVFString@@XZ", "33 C0 48 89 02 48 89 42 08 48 8B C2 C3 CC CC CC 40 53");
            FModuleManager::RegisterImport("?GetCPPTypeForwardDeclaration@UStructProperty@@UEBA?AVFString@@XZ", base_module + 0x3406580);
            FModuleManager::RegisterImport("?GetCPPTypeForwardDeclaration@UTextProperty@@UEBA?AVFString@@XZ", "33 C0 48 89 02 48 89 42 08 48 8B C2 C3 CC CC CC 40 53");
            FModuleManager::RegisterImport("?GetFloatingPointPropertyValue@?$TProperty_Numeric@H@@UEBANPEBX@Z", "66 0F 6E 02 F3 0F E6 C0");
            FModuleManager::RegisterImport("?GetID@UObjectPropertyBase@@UEBA?AVFName@@XZ", base_module + 0x33E7EB0);
            FModuleManager::RegisterImport("?GetID@UProperty@@UEBA?AVFName@@XZ", base_module + 0x33E0D70);
            FModuleManager::RegisterImport("?GetInheritanceSuper@UFunction@@UEBAPEAVUStruct@@XZ", NullSub);
            FModuleManager::RegisterImport("?GetIntPropertyEnum@UNumericProperty@@UEBAPEAVUEnum@@XZ", UNumericProperty_GetIntPropertyEnum);
            FModuleManager::RegisterImport("?GetMinAlignment@?$TProperty@HVUNumericProperty@@@@UEBAHXZ", UNumericProperty_Int_GetMinAlignment);
            FModuleManager::RegisterImport("?GetMinAlignment@?$TProperty@PEAVUObject@@VUObjectPropertyBase@@@@UEBAHXZ", UObjectProperty_GetMinAlignment);
            FModuleManager::RegisterImport("?GetMinAlignment@?$TProperty@VFScriptArray@@VUProperty@@@@UEBAHXZ", UObjectProperty_GetMinAlignment);
            FModuleManager::RegisterImport("?GetMinAlignment@?$TProperty@VFString@@VUProperty@@@@UEBAHXZ", UObjectProperty_GetMinAlignment);
            FModuleManager::RegisterImport("?GetMinAlignment@?$TProperty@VFText@@VUProperty@@@@UEBAHXZ", UObjectProperty_GetMinAlignment);
            FModuleManager::RegisterImport("?GetMinAlignment@UBoolProperty@@UEBAHXZ", base_module + 0x33E84F0);
            FModuleManager::RegisterImport("?GetMinAlignment@UStructProperty@@UEBAHXZ", base_module + 0x3406020);
            FModuleManager::RegisterImport("?GetNumericPropertyValueToString@?$TProperty_Numeric@H@@UEBA?AVFString@@PEBX@Z", "40 53 48 83 EC 20 45 8B 00 48 8B DA 48 8B CB 48 8D 15 ? ? ? ? E8 ? ? ? ? 48 8B C3 48 83 C4 20 5B C3 CC CC CC CC CC CC CC CC CC CC CC CC F2 0F 2C C1");
            FModuleManager::RegisterImport("?GetObjectPropertyValue@UObjectProperty@@UEBAPEAVUObject@@PEBX@Z", base_module + 0x2222560);
            FModuleManager::RegisterImport("?GetPrefixCPP@UStruct@@UEBAPEB_WXZ", UStruct_GetPrefixCPP);
            FModuleManager::RegisterImport("?GetPreloadDependencies@UArrayProperty@@UEAAXAEAV?$TArray@PEAVUObject@@VFDefaultAllocator@@@@@Z", base_module + 0x33E2990);
            FModuleManager::RegisterImport("?GetPreloadDependencies@UStruct@@UEAAXAEAV?$TArray@PEAVUObject@@VFDefaultAllocator@@@@@Z", "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 48 83 EC 20 48 8B DA 48 8B F9 E8 ? ? ? ? 48 63 73 08 48 8B 6F 48");
            FModuleManager::RegisterImport("?GetPreloadDependencies@UStructProperty@@UEAAXAEAV?$TArray@PEAVUObject@@VFDefaultAllocator@@@@@Z", base_module + 0x33E2990);
            FModuleManager::RegisterImport("?GetSignedIntPropertyValue@?$TProperty_Numeric@H@@UEBA_JPEBX@Z", "48 63 02 C3");
            FModuleManager::RegisterImport("?GetUnsignedIntPropertyValue@?$TProperty_Numeric@H@@UEBA_KPEBX@Z", "48 63 02 C3");
            FModuleManager::RegisterImport("?GetValueTypeHashInternal@?$TProperty_Numeric@H@@UEBAIPEBX@Z", "8B 02 C3 CC CC CC CC CC CC CC CC CC CC CC CC CC");
            FModuleManager::RegisterImport("?GetValueTypeHashInternal@UBoolProperty@@UEBAIPEBX@Z", base_module + 0x1BD9BF0);
            FModuleManager::RegisterImport("?GetValueTypeHashInternal@UObjectProperty@@EEBAIPEBX@Z", base_module + 0x3401220);
            FModuleManager::RegisterImport("?GetValueTypeHashInternal@UProperty@@MEBAIPEBX@Z", NullSub);
            FModuleManager::RegisterImport("?GetValueTypeHashInternal@UStrProperty@@UEBAIPEBX@Z", "83 7A 08 00 74 ? 48 8B 0A E9 ? ? ? ?");
            FModuleManager::RegisterImport("?GetValueTypeHashInternal@UStructProperty@@EEBAIPEBX@Z", base_module + 0x34062D0);
            FModuleManager::RegisterImport("?Identical@?$TProperty_WithEqualityAndSerializer@HVUNumericProperty@@@@UEBA_NPEBX0I@Z", "4D 85 C0 74 ? 41 8B 00 39 02");
            FModuleManager::RegisterImport("?Identical@?$TProperty_WithEqualityAndSerializer@VFString@@VUProperty@@@@UEBA_NPEBX0I@Z", "48 8B C4 55 57 41 56 48 8B EC 48 83 EC 60 48 C7 45 C0 FE FF FF FF 48 89 58 08 48 89 70 20 4C 8B F2");
            FModuleManager::RegisterImport("?Identical@UArrayProperty@@UEBA_NPEBX0I@Z", base_module + 0x33E2A90);
            FModuleManager::RegisterImport("?Identical@UBoolProperty@@UEBA_NPEBX0I@Z", base_module + 0x33E98A0);
            FModuleManager::RegisterImport("?Identical@UObjectPropertyBase@@UEBA_NPEBX0I@Z", base_module + 0x33E6880);
            FModuleManager::RegisterImport("?Identical@UStructProperty@@UEBA_NPEBX0I@Z", base_module + 0x34061A0);
            FModuleManager::RegisterImport("?Identical@UTextProperty@@UEBA_NPEBX0I@Z", base_module + 0x3416C00);
            FModuleManager::RegisterImport("?ImportText_Internal@UArrayProperty@@UEBAPEB_WPEB_WPEAXHPEAVUObject@@PEAVFOutputDevice@@@Z", base_module + 0x33E3E80);
            FModuleManager::RegisterImport("?ImportText_Internal@UBoolProperty@@UEBAPEB_WPEB_WPEAXHPEAVUObject@@PEAVFOutputDevice@@@Z", base_module + 0x33E93F0);
            FModuleManager::RegisterImport("?ImportText_Internal@UNumericProperty@@UEBAPEB_WPEB_WPEAXHPEAVUObject@@PEAVFOutputDevice@@@Z", "48 8B C4 4C 89 40 18 48 89 48 08 55 56 57 41 54 41 55 41 56 41 57 48 83 EC 50 48 C7 40 A8 FE FF FF FF");
            FModuleManager::RegisterImport("?ImportText_Internal@UObjectProperty@@UEBAPEB_WPEB_WPEAXHPEAVUObject@@PEAVFOutputDevice@@@Z", base_module + 0x3400EB0);
            FModuleManager::RegisterImport("?ImportText_Internal@UStrProperty@@UEBAPEB_WPEB_WPEAXHPEAVUObject@@PEAVFOutputDevice@@@Z", "41 56 48 83 EC 40 48 C7 44 24 20 FE FF FF FF 48 89 5C 24 50 48 89 74 24 58 48 89 7C 24 60 4D 8B F0 48 8B DA 41 F6 C1 02");
            FModuleManager::RegisterImport("?ImportText_Internal@UStructProperty@@UEBAPEB_WPEB_WPEAXHPEAVUObject@@PEAVFOutputDevice@@@Z", base_module + 0x3406710);
            FModuleManager::RegisterImport("?ImportText_Internal@UTextProperty@@UEBAPEB_WPEB_WPEAXHPEAVUObject@@PEAVFOutputDevice@@@Z", base_module + 0x34170B0);
            FModuleManager::RegisterImport("?InitializeStruct@UStruct@@UEBAXPEAXH@Z", base_module + 0x32EE1F0);
            FModuleManager::RegisterImport("?InitializeValueInternal@?$TProperty@HVUNumericProperty@@@@UEBAXPEAX@Z", base_module + 0x1BD9790);
            FModuleManager::RegisterImport("?InitializeValueInternal@?$TProperty@PEAVUObject@@VUObjectPropertyBase@@@@UEBAXPEAX@Z", base_module + 0x1BDAD90);
            FModuleManager::RegisterImport("?InitializeValueInternal@?$TProperty@VFScriptArray@@VUProperty@@@@UEBAXPEAX@Z", base_module + 0x1BDAAB0);
            FModuleManager::RegisterImport("?InitializeValueInternal@?$TProperty@VFString@@VUProperty@@@@UEBAXPEAX@Z", "45 33 D2 4C 8B CA 45 8B C2");
            FModuleManager::RegisterImport("?InitializeValueInternal@?$TProperty@VFText@@VUProperty@@@@UEBAXPEAX@Z", base_module + 0x22513C0);
            FModuleManager::RegisterImport("?InitializeValueInternal@UBoolProperty@@UEBAXPEAX@Z", base_module + 0x33E9A40);
            FModuleManager::RegisterImport("?InitializeValueInternal@UStructProperty@@UEBAXPEAX@Z", base_module + 0x34067E0);
            FModuleManager::RegisterImport("?InstanceSubobjects@UArrayProperty@@UEAAXPEAXPEBXPEAVUObject@@PEAUFObjectInstancingGraph@@@Z", base_module + 0x33E4580);
            FModuleManager::RegisterImport("?InstanceSubobjects@UObjectPropertyBase@@UEAAXPEAXPEBXPEAVUObject@@PEAUFObjectInstancingGraph@@@Z", base_module + 0x33E6790);
            FModuleManager::RegisterImport("?InstanceSubobjects@UProperty@@UEAAXPEAXPEBXPEAVUObject@@PEAUFObjectInstancingGraph@@@Z", VoidSub);
            FModuleManager::RegisterImport("?InstanceSubobjects@UStructProperty@@UEAAXPEAXPEBXPEAVUObject@@PEAUFObjectInstancingGraph@@@Z", base_module + 0x3406970);
            FModuleManager::RegisterImport("?IsFloatingPoint@?$TProperty_Numeric@H@@UEBA_NXZ", NullSub);
            FModuleManager::RegisterImport("?IsInteger@?$TProperty_Numeric@H@@UEBA_NXZ", TrueSub);
            FModuleManager::RegisterImport("?Link@UFunction@@UEAAXAEAVFArchive@@_N@Z", base_module + 0x32F72F0);
            FModuleManager::RegisterImport("?LinkInternal@?$TProperty@HVUNumericProperty@@@@UEAAXAEAVFArchive@@@Z", base_module + 0x1BD91D0);
            FModuleManager::RegisterImport("?LinkInternal@?$TProperty@PEAVUObject@@VUObjectPropertyBase@@@@UEAAXAEAVFArchive@@@Z", base_module + 0x1BDA290);
            FModuleManager::RegisterImport("?LinkInternal@?$TProperty@VFString@@VUProperty@@@@UEAAXAEAVFArchive@@@Z", "48 B8 00 02 00 00 00 00 08 00 C7 41 4C 10 00 00 00");
            FModuleManager::RegisterImport("?LinkInternal@?$TProperty@VFText@@VUProperty@@@@UEAAXAEAVFArchive@@@Z", base_module + 0x2251200);
            FModuleManager::RegisterImport("?LinkInternal@UArrayProperty@@UEAAXAEAVFArchive@@@Z", base_module + 0x33E29E0);
            FModuleManager::RegisterImport("?LinkInternal@UBoolProperty@@UEAAXAEAVFArchive@@@Z", base_module + 0x33E8640);
            FModuleManager::RegisterImport("?LinkInternal@UStructProperty@@UEAAXAEAVFArchive@@@Z", base_module + 0x3406030);
            FModuleManager::RegisterImport("?NetSerializeItem@UArrayProperty@@UEBA_NAEAVFArchive@@PEAVUPackageMap@@PEAXPEAV?$TArray@EVFDefaultAllocator@@@@@Z", "48 83 EC 28 4C 8D 05 ? ? ? ? BA EF 00 00 00");
            FModuleManager::RegisterImport("?NetSerializeItem@UBoolProperty@@UEBA_NAEAVFArchive@@PEAVUPackageMap@@PEAXPEAV?$TArray@EVFDefaultAllocator@@@@@Z", "48 89 5C 24 10 57 48 83 EC 20 0F B6 99 91 00 00 00 4C 8B D2");
            FModuleManager::RegisterImport("?NetSerializeItem@UObjectPropertyBase@@UEBA_NAEAVFArchive@@PEAVUPackageMap@@PEAXPEAV?$TArray@EVFDefaultAllocator@@@@@Z", "48 89 5C 24 10 48 89 6C 24 18 48 89 74 24 20 57 48 83 EC 30 48 8B 01 48 8B DA");
            FModuleManager::RegisterImport("?NetSerializeItem@UProperty@@UEBA_NAEAVFArchive@@PEAVUPackageMap@@PEAXPEAV?$TArray@EVFDefaultAllocator@@@@@Z", base_module + 0x33E0CD0);
            FModuleManager::RegisterImport("?NetSerializeItem@UStructProperty@@UEBA_NAEAVFArchive@@PEAVUPackageMap@@PEAXPEAV?$TArray@EVFDefaultAllocator@@@@@Z", "48 83 EC 48 48 C7 44 24 30 FE FF FF FF 48 8B 89 90 00 00 00");
            FModuleManager::RegisterImport("?PassCPPArgsByRef@?$TProperty@HVUNumericProperty@@@@UEBA_NXZ", NullSub);
            FModuleManager::RegisterImport("?PassCPPArgsByRef@?$TProperty@PEAVUObject@@VUObjectPropertyBase@@@@UEBA_NXZ", NullSub);
            FModuleManager::RegisterImport("?PassCPPArgsByRef@?$TProperty@VFString@@VUProperty@@@@UEBA_NXZ", TrueSub);
            FModuleManager::RegisterImport("?PassCPPArgsByRef@?$TProperty@VFText@@VUProperty@@@@UEBA_NXZ", TrueSub);
            FModuleManager::RegisterImport("?PassCPPArgsByRef@UArrayProperty@@UEBA_NXZ", TrueSub);
            FModuleManager::RegisterImport("?PassCPPArgsByRef@UProperty@@UEBA_NXZ", NullSub);
            FModuleManager::RegisterImport("?PostLoad@UField@@UEAAXXZ", "40 53 48 83 EC 30 48 8B 41 18 48 8B D9 8B 90 CC 00 00 00 C1 EA 0A F6 C2 01 74 ? 45 33 C9 45 33 C0 33 D2 E8 ? ? ? ? 48 8B 03 48 8B CB 48 83 C4 30 5B 48 FF A0 40 02 00 00");
            FModuleManager::RegisterImport("?PropertyNameToDisplayName@UStruct@@UEBA?AVFString@@VFName@@@Z", base_module + 0x33AA9D0);
            FModuleManager::RegisterImport("?RegisterDependencies@UStruct@@UEAAXXZ", base_module + 0x32ED810);
            FModuleManager::RegisterImport("?SameType@UArrayProperty@@UEBA_NPEBVUProperty@@@Z", base_module + 0x33E4740);
            FModuleManager::RegisterImport("?SameType@UObjectPropertyBase@@UEBA_NPEBVUProperty@@@Z", base_module + 0x33E8140);
            FModuleManager::RegisterImport("?SameType@UProperty@@UEBA_NPEBV1@@Z", base_module + 0x33E0D80);
            FModuleManager::RegisterImport("?SameType@UStructProperty@@UEBA_NPEBVUProperty@@@Z", base_module + 0x33E8140);
            FModuleManager::RegisterImport("?Serialize@UArrayProperty@@UEAAXAEAVFArchive@@@Z", base_module + 0x33E31D0);
            FModuleManager::RegisterImport("?Serialize@UBoolProperty@@UEAAXAEAVFArchive@@@Z", base_module + 0x33E8690);
            FModuleManager::RegisterImport("?Serialize@UFunction@@UEAAXAEAVFArchive@@@Z", base_module + 0x32F6EC0);
            FModuleManager::RegisterImport("?Serialize@UObjectPropertyBase@@UEAAXAEAVFArchive@@@Z", base_module + 0x33E6A40);
            FModuleManager::RegisterImport("?Serialize@UProperty@@UEAAXAEAVFArchive@@@Z", base_module + 0x33E0600);
            FModuleManager::RegisterImport("?Serialize@UStructProperty@@UEAAXAEAVFArchive@@@Z", base_module + 0x34063E0);
            FModuleManager::RegisterImport("?SerializeBin@UStruct@@UEBAXAEAVFArchive@@PEAX@Z", base_module + 0x32EE3C0);
            FModuleManager::RegisterImport("?SerializeExpr@UStruct@@UEAA?AW4EExprToken@@AEAHAEAVFArchive@@@Z", base_module + 0x32EFA80);
            FModuleManager::RegisterImport("?SerializeItem@?$TProperty_WithEqualityAndSerializer@HVUNumericProperty@@@@UEBAXAEAVFArchive@@PEAXPEBX@Z", base_module + 0x1BD96B0);
            FModuleManager::RegisterImport("?SerializeItem@?$TProperty_WithEqualityAndSerializer@VFString@@VUProperty@@@@UEBAXAEAVFArchive@@PEAXPEBX@Z", base_module + 0x1BD8E20);
            FModuleManager::RegisterImport("?SerializeItem@UArrayProperty@@UEBAXAEAVFArchive@@PEAXPEBX@Z", base_module + 0x33E2B60);
            FModuleManager::RegisterImport("?SerializeItem@UBoolProperty@@UEBAXAEAVFArchive@@PEAXPEBX@Z", base_module + 0x33E98E0);
            FModuleManager::RegisterImport("?SerializeItem@UObjectProperty@@UEBAXAEAVFArchive@@PEAXPEBX@Z", base_module + 0x3400BD0);
            FModuleManager::RegisterImport("?SerializeItem@UStructProperty@@UEBAXAEAVFArchive@@PEAXPEBX@Z", base_module + 0x34062F0);
            FModuleManager::RegisterImport("?SerializeItem@UTextProperty@@UEBAXAEAVFArchive@@PEAXPEBX@Z", base_module + 0x3416E10);
            FModuleManager::RegisterImport("?SerializeSuperStruct@UStruct@@UEAAXAEAVFArchive@@@Z", base_module + 0x32EF840);
            FModuleManager::RegisterImport("?SerializeTaggedProperties@UStruct@@UEBAXAEAVFArchive@@PEAEPEAV1@1PEBVUObject@@@Z", base_module + 0x32EE750);
            FModuleManager::RegisterImport("?SetFloatingPointPropertyValue@?$TProperty_Numeric@H@@UEBAXPEAXN@Z", "F2 0F 2C C2 89 02");
            FModuleManager::RegisterImport("?SetIntPropertyValue@?$TProperty_Numeric@H@@UEBAXPEAX_J@Z", "44 89 02 C3 CC CC CC CC CC CC CC CC CC CC CC CC");
            FModuleManager::RegisterImport("?SetIntPropertyValue@?$TProperty_Numeric@H@@UEBAXPEAX_K@Z", "44 89 02 C3 CC CC CC CC CC CC CC CC CC CC CC CC");
            FModuleManager::RegisterImport("?SetNumericPropertyValueFromString@?$TProperty_Numeric@H@@UEBAXPEAXPEB_W@Z", "40 53 48 83 EC 20 49 8B C8 48 8B DA FF 15 ? ? ? ? 89 03 48 83 C4 20 5B C3 CC CC CC CC CC CC 48 63 02");
            FModuleManager::RegisterImport("?SetNumericPropertyValueFromString@?$TProperty_Numeric@H@@UEBAXPEAXPEB_W@Z", "4C 89 02 C3 CC CC CC CC CC CC CC CC CC");
            FModuleManager::RegisterImport("?SetObjectPropertyValue@UObjectProperty@@UEBAXPEAXPEAVUObject@@@Z", base_module + 0x2222520);
            FModuleManager::RegisterImport("?SetSuperStruct@UStruct@@UEAAXPEAV1@@Z", base_module + 0x32EF830);
            FModuleManager::RegisterImport("?TagSubobjects@UStruct@@UEAAXW4EObjectFlags@@@Z", "48 89 5C 24 08 57 48 83 EC 40 8B FA");
            FModuleManager::RegisterImport("?Get@FObjectInitializer@@SAAEAV1@XZ", "40 53 48 83 EC 20 33 C0 83 3D ? ? ? ? ?");
            FModuleManager::RegisterImport("?GetTypedOuter@UObjectBaseUtility@@QEBAPEAVUObject@@PEAVUClass@@@Z", GetTypedOuter);
            FModuleManager::RegisterImport("??0UObject@@QEAA@AEBVFObjectInitializer@@@Z", "48 89 4C 24 08 57 48 83 EC 30 48 C7 44 24 20 FE FF FF FF 48 89 5C 24 48 48 8B FA 48 8B D9 48 8D 05 ? ? ? ? 48 89 01 C7 41 30 FF FF FF FF");
            FModuleManager::RegisterImport("?CallFunctionByNameWithArguments@UObject@@QEAA_NPEB_WAEAVFOutputDevice@@PEAV1@_N@Z", base_module + 0x340B150);
            FModuleManager::RegisterImport("?Step@FFrame@@QEAAXPEAVUObject@@QEAX@Z", FFrame_Step);
            FModuleManager::RegisterImport("?StepExplicitProperty@FFrame@@QEAAXQEAXPEAVUProperty@@@Z", FFrame_StepExplicitProperty);
            FModuleManager::RegisterImport("?GetCPPTypeForwardDeclaration@UObjectProperty@@UEBA?AVFString@@XZ", "48 89 54 24 10 57 48 83 EC 40 48 C7 44 24 20 FE FF FF FF 48 89 5C 24 50 48 89 74 24 60 48 8B F2 48 8B D9 48 8B 89 98 00 00 00 48 8B 01 FF 90 80 02 00 00 48 8B F8 48 8B 8B 98 00 00 00 4C 8B 41 20 4C 89 44 24 58 48 8D 54 24 28 48 8D 4C 24 58 E8 ? ? ? ? 90 4C 8D 0D ? ? ? ? 83 7C 24 30 00 4C 0F 45 4C 24 28 4C 8B C7 48 8D 15 ? ? ? ? 48 8B CE E8 ? ? ? ? 90 48 8B 4C 24 28 48 85 C9 74 ? E8 ? ? ? ? 90 48 8B C6 48 8B 5C 24 50 48 8B 74 24 60 48 83 C4 40 5F C3 CC CC 48 89 54 24 10");
            FModuleManager::RegisterImport("?Z_Construct_UClass_UGameInstance_NoRegister@@YAPEAVUClass@@XZ", base_module + 0x4E07310);
            FModuleManager::RegisterImport("?Z_Construct_UClass_UWorld_NoRegister@@YAPEAVUClass@@XZ", base_module + 0x550A1A0);
            FModuleManager::RegisterImport("?Z_Construct_UClass_APlayerController_NoRegister@@YAPEAVUClass@@XZ", base_module + 0x52ED9C0);
            FModuleManager::RegisterImport("?Z_Construct_UClass_AActor_NoRegister@@YAPEAVUClass@@XZ", base_module + 0x4BDD690);
            FModuleManager::RegisterImport("?Z_Construct_UClass_USoundBase_NoRegister@@YAPEAVUClass@@XZ", base_module + 0x541AD00);
            FModuleManager::RegisterImport("?Z_Construct_UClass_UDialogueWave_NoRegister@@YAPEAVUClass@@XZ", base_module + 0x4D69CA0);
            FModuleManager::RegisterImport("??0UDataAsset@@QEAA@AEAVFVTableHelper@@@Z", UDataAsset_Constructor);
            FModuleManager::RegisterImport("?GetPrivateStaticClass@UDataAsset@@CAPEAVUClass@@XZ", "48 81 EC 98 00 00 00 48 83 3D ? ? ? ? ? 0F 85 ? ? ? ? E8 ? ? ? ? 48 89 44 24 70 E8 ? ? ? ? 48 89 44 24 78 48 8D 0D ? ? ? ? 48 83 C1 02 48 89 8C 24 80 00 00 00 E8 ? ? ? ? C6 44 24 68 00 48 8D 0D ? ? ? ? 48 89 4C 24 60 48 8D 0D ? ? ? ? 48 89 4C 24 58 48 8D 0D ? ? ? ? 48 89 4C 24 50 48 8D 0D ? ? ? ? 48 89 4C 24 48 48 8D 0D ? ? ? ? 48 89 4C 24 40 48 8B 4C 24 70 48 89 4C 24 38 48 8B 4C 24 78 48 89 4C 24 30 C7 44 24 28 01 00 00 10 C7 44 24 20 48 00 00 00 4C 8D 0D ? ? ? ? 4C 8D 05 ? ? ? ? 48 8B 8C 24 80 00 00 00 48 8B D1 48 8B C8 E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 81 C4 98 00 00 00 C3 CC CC CC CC CC CC CC CC CC CC 48 83 EC 28 48 83 3D ? ? ? ? ? 0F 85 ? ? ? ? E8 ? ? ? ? E8 ? ? ? ? E8 ? ? ? ? 48 89 05 ? ? ? ? BA 00 00 00 20 48 8B 05 ? ? ? ? 8B 88 CC 00 00 00 E8 ? ? ? ? 8B C8 E8 ? ? ? ? 0F B6 C0 85 C0 0F 84 ? ? ? ? 48 8B 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 05 CC 00 00 00 BA 81 00 08 20");
            FModuleManager::RegisterImport("?Z_Construct_UClass_UDataAsset@@YAPEAVUClass@@XZ", "48 81 EC F8 01 00 00 48 C7 44 24 78 FE FF FF FF 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 E0 01 00 00 C7 44 24 40 00 00 00 00 48 83 3D ? ? ? ? ? 0F 85 ? ? ? ? E8 ? ? ? ? E8 ? ? ? ? E8 ? ? ? ? 48 89 05 ? ? ? ? BA 00 00 00 20 48 8B 05 ? ? ? ? 8B 88 CC 00 00 00 E8 ? ? ? ? 8B C8 E8 ? ? ? ? 0F B6 C0 85 C0 0F 84 ? ? ? ? 48 8B 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 05 CC 00 00 00 BA 81 00 08 20");
            FModuleManager::RegisterImport("??0UDataAsset@@QEAA@AEBVFObjectInitializer@@@Z", UDataAsset_InitializerConstructor);
            FModuleManager::RegisterImport("?NativeOnFocusReceived@UUserWidget@@MEAA?AVFReply@@AEBUFGeometry@@AEBUFFocusEvent@@@Z", "40 53 48 81 EC 30 01 00 00 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 20 01 00 00 41 0F 10 00 4D 8B 09");
            FModuleManager::RegisterImport("?NativeOnKeyChar@UUserWidget@@MEAA?AVFReply@@AEBUFGeometry@@AEBUFCharacterEvent@@@Z", "40 53 48 81 EC 60 01 00 00 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 50 01 00 00 41 0F B7 41 08");
            FModuleManager::RegisterImport("?NativeOnKeyDown@UUserWidget@@MEAA?AVFReply@@AEBUFGeometry@@AEBUFKeyEvent@@@Z", "40 53 48 81 EC 80 01 00 00 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 70 01 00 00 48 8B DA 4C 8B D1 49 8B D1 48 8D 4C 24 70 E8 ? ? ? ? 41 0F 10 00 4C 8B C8 48 8D 94 24 B0 00 00 00 41 0F 10 48 10 49 8B CA 0F 29 44 24 30 41 0F 10 40 20 0F 29 4C 24 40 F2 41 0F 10 48 30 4C 8D 44 24 30 0F 29 44 24 50 F2 0F 11 4C 24 60 E8 ? ? ? ? 48 8B D0 48 8B CB E8 ? ? ? ? 48 8D 8C 24 B0 00 00 00 E8 ? ? ? ? 48 8B C3 48 8B 8C 24 70 01 00 00 48 33 CC E8 ? ? ? ? 48 81 C4 80 01 00 00 5B C3 CC CC CC CC CC CC CC CC CC CC CC CC CC 40 53 48 81 EC 80 01 00 00 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 70 01 00 00 48 8B DA 4C 8B D1 49 8B D1 48 8D 4C 24 70 E8 ? ? ? ? 41 0F 10 00 4C 8B C8 48 8D 94 24 B0 00 00 00 41 0F 10 48 10 49 8B CA 0F 29 44 24 30 41 0F 10 40 20 0F 29 4C 24 40 F2 41 0F 10 48 30 4C 8D 44 24 30 0F 29 44 24 50 F2 0F 11 4C 24 60 E8 ? ? ? ? 48 8B D0 48 8B CB E8 ? ? ? ? 48 8D 8C 24 B0 00 00 00 E8 ? ? ? ? 48 8B C3 48 8B 8C 24 70 01 00 00 48 33 CC E8 ? ? ? ? 48 81 C4 80 01 00 00 5B C3 CC CC CC CC CC CC CC CC CC CC CC CC CC 40 53 48 81 EC 90 01 00 00");
            FModuleManager::RegisterImport("?NativeOnKeyUp@UUserWidget@@MEAA?AVFReply@@AEBUFGeometry@@AEBUFKeyEvent@@@Z", "40 53 48 81 EC 80 01 00 00 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 70 01 00 00 48 8B DA 4C 8B D1 49 8B D1 48 8D 4C 24 70 E8 ? ? ? ? 41 0F 10 00 4C 8B C8 48 8D 94 24 B0 00 00 00 41 0F 10 48 10 49 8B CA 0F 29 44 24 30 41 0F 10 40 20 0F 29 4C 24 40 F2 41 0F 10 48 30 4C 8D 44 24 30 0F 29 44 24 50 F2 0F 11 4C 24 60 E8 ? ? ? ? 48 8B D0 48 8B CB E8 ? ? ? ? 48 8D 8C 24 B0 00 00 00 E8 ? ? ? ? 48 8B C3 48 8B 8C 24 70 01 00 00 48 33 CC E8 ? ? ? ? 48 81 C4 80 01 00 00 5B C3 CC CC CC CC CC CC CC CC CC CC CC CC CC 40 53 48 81 EC 90 01 00 00");
            FModuleManager::RegisterImport("?NativeOnMotionDetected@UUserWidget@@MEAA?AVFReply@@AEBUFGeometry@@AEBUFMotionEvent@@@Z", "40 53 48 81 EC 90 01 00 00 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 80 01 00 00 41 0F B7 41 08");
            FModuleManager::RegisterImport("?NativeOnMouseButtonDoubleClick@UUserWidget@@MEAA?AVFReply@@AEBUFGeometry@@AEBUFPointerEvent@@@Z", "40 53 48 81 EC 30 01 00 00 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 20 01 00 00 41 0F 10 00 48 8B DA 48 8D 54 24 60 41 0F 10 48 10 0F 29 44 24 20 41 0F 10 40 20 0F 29 4C 24 30 F2 41 0F 10 48 30 4C 8D 44 24 20 0F 29 44 24 40 F2 0F 11 4C 24 50 E8 ? ? ? ? 48 8B D0 48 8B CB E8 ? ? ? ? 48 8D 4C 24 60 E8 ? ? ? ? 48 8B C3 48 8B 8C 24 20 01 00 00 48 33 CC E8 ? ? ? ? 48 81 C4 30 01 00 00 5B C3 CC CC CC CC CC CC CC CC CC 48 83 EC 68 0F 10 02 0F 10 4A 10 0F 29 44 24 20 0F 10 42 20 0F 29 4C 24 30 F2 0F 10 4A 30 48 8D 54 24 20 0F 29 44 24 40 F2 0F 11 4C 24 50 E8 ? ? ? ? 48 83 C4 68 C3 CC CC CC CC CC CC CC CC 48 81 EC D8 00 00 00");
            FModuleManager::RegisterImport("?NativeOnMouseButtonDown@UUserWidget@@MEAA?AVFReply@@AEBUFGeometry@@AEBUFPointerEvent@@@Z", "40 53 48 81 EC 30 01 00 00 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 20 01 00 00 41 0F 10 00 48 8B DA 48 8D 54 24 60 41 0F 10 48 10 0F 29 44 24 20 41 0F 10 40 20 0F 29 4C 24 30 F2 41 0F 10 48 30 4C 8D 44 24 20 0F 29 44 24 40 F2 0F 11 4C 24 50 E8 ? ? ? ? 48 8B D0 48 8B CB E8 ? ? ? ? 48 8D 4C 24 60 E8 ? ? ? ? 48 8B C3 48 8B 8C 24 20 01 00 00 48 33 CC E8 ? ? ? ? 48 81 C4 30 01 00 00 5B C3 CC CC CC CC CC CC CC CC CC 40 53 48 81 EC 30 01 00 00 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 20 01 00 00 41 0F 10 00 48 8B DA 48 8D 54 24 60 41 0F 10 48 10 0F 29 44 24 20 41 0F 10 40 20 0F 29 4C 24 30 F2 41 0F 10 48 30 4C 8D 44 24 20 0F 29 44 24 40 F2 0F 11 4C 24 50 E8 ? ? ? ? 48 8B D0 48 8B CB E8 ? ? ? ? 48 8D 4C 24 60 E8 ? ? ? ? 48 8B C3 48 8B 8C 24 20 01 00 00 48 33 CC E8 ? ? ? ? 48 81 C4 30 01 00 00 5B C3 CC CC CC CC CC CC CC CC CC 40 53 48 81 EC 30 01 00 00 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 20 01 00 00 41 0F 10 00 48 8B DA 48 8D 54 24 60 41 0F 10 48 10 0F 29 44 24 20 41 0F 10 40 20 0F 29 4C 24 30 F2 41 0F 10 48 30 4C 8D 44 24 20 0F 29 44 24 40 F2 0F 11 4C 24 50 E8 ? ? ? ? 48 8B D0 48 8B CB E8 ? ? ? ? 48 8D 4C 24 60 E8 ? ? ? ? 48 8B C3 48 8B 8C 24 20 01 00 00 48 33 CC E8 ? ? ? ? 48 81 C4 30 01 00 00 5B C3 CC CC CC CC CC CC CC CC CC 40 53 48 81 EC 30 01 00 00 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 20 01 00 00 41 0F 10 00 48 8B DA 48 8D 54 24 60 41 0F 10 48 10 0F 29 44 24 20 41 0F 10 40 20 0F 29 4C 24 30 F2 41 0F 10 48 30 4C 8D 44 24 20 0F 29 44 24 40 F2 0F 11 4C 24 50 E8 ? ? ? ? 48 8B D0 48 8B CB E8 ? ? ? ? 48 8D 4C 24 60 E8 ? ? ? ? 48 8B C3 48 8B 8C 24 20 01 00 00 48 33 CC E8 ? ? ? ? 48 81 C4 30 01 00 00 5B C3 CC CC CC CC CC CC CC CC CC 48 83 EC 68");
            FModuleManager::RegisterImport("?NativeOnMouseButtonUp@UUserWidget@@MEAA?AVFReply@@AEBUFGeometry@@AEBUFPointerEvent@@@Z", "40 53 48 81 EC 30 01 00 00 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 20 01 00 00 41 0F 10 00 48 8B DA 48 8D 54 24 60 41 0F 10 48 10 0F 29 44 24 20 41 0F 10 40 20 0F 29 4C 24 30 F2 41 0F 10 48 30 4C 8D 44 24 20 0F 29 44 24 40 F2 0F 11 4C 24 50 E8 ? ? ? ? 48 8B D0 48 8B CB E8 ? ? ? ? 48 8D 4C 24 60 E8 ? ? ? ? 48 8B C3 48 8B 8C 24 20 01 00 00 48 33 CC E8 ? ? ? ? 48 81 C4 30 01 00 00 5B C3 CC CC CC CC CC CC CC CC CC 40 53 48 81 EC 30 01 00 00 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 20 01 00 00 41 0F 10 00 48 8B DA 48 8D 54 24 60 41 0F 10 48 10 0F 29 44 24 20 41 0F 10 40 20 0F 29 4C 24 30 F2 41 0F 10 48 30 4C 8D 44 24 20 0F 29 44 24 40 F2 0F 11 4C 24 50 E8 ? ? ? ? 48 8B D0 48 8B CB E8 ? ? ? ? 48 8D 4C 24 60 E8 ? ? ? ? 48 8B C3 48 8B 8C 24 20 01 00 00 48 33 CC E8 ? ? ? ? 48 81 C4 30 01 00 00 5B C3 CC CC CC CC CC CC CC CC CC 48 83 EC 68 0F 10 02 0F 10 4A 10 0F 29 44 24 20 0F 10 42 20 0F 29 4C 24 30 F2 0F 10 4A 30 48 8D 54 24 20 0F 29 44 24 40 F2 0F 11 4C 24 50 E8 ? ? ? ? 48 83 C4 68 C3 CC CC CC CC CC CC CC CC E9 ? ? ? ?");
            FModuleManager::RegisterImport("?NativeOnMouseCaptureLost@UUserWidget@@MEAAXXZ", "48 89 4C 24 08 48 83 EC 38 48 8B 15 ? ? ? ? 48 8B 4C 24 40 E8 ? ? ? ? 48 8B 4C 24 40 48 8B 09 48 89 4C 24 20 45 33 C0 48 8B D0 48 8B 4C 24 40 48 8B 44 24 20 FF 90 00 02 00 00 48 83 C4 38 C3 CC CC CC CC CC CC CC CC CC CC CC CC CC CC 4C 89 44 24 18 48 89 54 24 10 48 89 4C 24 08 48 81 EC F8 00 00 00");
            FModuleManager::RegisterImport("?NativeOnMouseEnter@UUserWidget@@MEAAXAEBUFGeometry@@AEBUFPointerEvent@@@Z", "48 83 EC 68 0F 10 02 0F 10 4A 10 0F 29 44 24 20 0F 10 42 20 0F 29 4C 24 30 F2 0F 10 4A 30 48 8D 54 24 20 0F 29 44 24 40 F2 0F 11 4C 24 50 E8 ? ? ? ? 48 83 C4 68 C3 CC CC CC CC CC CC CC CC E9 ? ? ? ?");
            FModuleManager::RegisterImport("?NativeOnMouseLeave@UUserWidget@@MEAAXAEBUFPointerEvent@@@Z", "48 89 54 24 10 48 89 4C 24 08 48 81 EC A8 00 00 00 48 C7 44 24 28 FE FF FF FF 48 8D 44 24 30 48 8B C8 E8 ? ? ? ? 90 48 8B 94 24 B8 00 00 00 48 8D 4C 24 30 E8 ? ? ? ? 48 8B 15 ? ? ? ? 48 8B 8C 24 B0 00 00 00 E8 ? ? ? ? 48 8B 8C 24 B0 00 00 00 48 8B 09 48 89 4C 24 20 4C 8D 44 24 30 48 8B D0 48 8B 8C 24 B0 00 00 00 48 8B 44 24 20 FF 90 00 02 00 00 90 48 8D 44 24 30 48 8B C8 E8 ? ? ? ? 48 81 C4 A8 00 00 00 C3 CC 4C 89 4C 24 20");
            FModuleManager::RegisterImport("?NativeOnMouseMove@UUserWidget@@MEAA?AVFReply@@AEBUFGeometry@@AEBUFPointerEvent@@@Z", "40 53 48 81 EC 30 01 00 00 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 20 01 00 00 41 0F 10 00 48 8B DA 48 8D 54 24 60 41 0F 10 48 10 0F 29 44 24 20 41 0F 10 40 20 0F 29 4C 24 30 F2 41 0F 10 48 30 4C 8D 44 24 20 0F 29 44 24 40 F2 0F 11 4C 24 50 E8 ? ? ? ? 48 8B D0 48 8B CB E8 ? ? ? ? 48 8D 4C 24 60 E8 ? ? ? ? 48 8B C3 48 8B 8C 24 20 01 00 00 48 33 CC E8 ? ? ? ? 48 81 C4 30 01 00 00 5B C3 CC CC CC CC CC CC CC CC CC 48 83 EC 68 0F 10 02 0F 10 4A 10 0F 29 44 24 20 0F 10 42 20 0F 29 4C 24 30 F2 0F 10 4A 30 48 8D 54 24 20 0F 29 44 24 40 F2 0F 11 4C 24 50 E8 ? ? ? ? 48 83 C4 68 C3 CC CC CC CC CC CC CC CC E9 ? ? ? ?");
            FModuleManager::RegisterImport("?NativeOnMouseWheel@UUserWidget@@MEAA?AVFReply@@AEBUFGeometry@@AEBUFPointerEvent@@@Z", "40 53 48 81 EC 30 01 00 00 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 20 01 00 00 41 0F 10 00 48 8B DA 48 8D 54 24 60 41 0F 10 48 10 0F 29 44 24 20 41 0F 10 40 20 0F 29 4C 24 30 F2 41 0F 10 48 30 4C 8D 44 24 20 0F 29 44 24 40 F2 0F 11 4C 24 50 E8 ? ? ? ? 48 8B D0 48 8B CB E8 ? ? ? ? 48 8D 4C 24 60 E8 ? ? ? ? 48 8B C3 48 8B 8C 24 20 01 00 00 48 33 CC E8 ? ? ? ? 48 81 C4 30 01 00 00 5B C3 CC CC CC CC CC CC CC CC CC 40 53 48 81 EC 30 01 00 00 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 20 01 00 00 41 0F 10 00 48 8B DA 48 8D 54 24 60 41 0F 10 48 10 0F 29 44 24 20 41 0F 10 40 20 0F 29 4C 24 30 F2 41 0F 10 48 30 4C 8D 44 24 20 0F 29 44 24 40 F2 0F 11 4C 24 50 E8 ? ? ? ? 48 8B D0 48 8B CB E8 ? ? ? ? 48 8D 4C 24 60 E8 ? ? ? ? 48 8B C3 48 8B 8C 24 20 01 00 00 48 33 CC E8 ? ? ? ? 48 81 C4 30 01 00 00 5B C3 CC CC CC CC CC CC CC CC CC 48 83 EC 68 0F 10 02 0F 10 4A 10 0F 29 44 24 20 0F 10 42 20 0F 29 4C 24 30 F2 0F 10 4A 30 48 8D 54 24 20 0F 29 44 24 40 F2 0F 11 4C 24 50 E8 ? ? ? ? 48 83 C4 68 C3 CC CC CC CC CC CC CC CC 48 81 EC D8 00 00 00");
            FModuleManager::RegisterImport("?NativeOnNavigation@UUserWidget@@MEAA?AVFNavigationReply@@AEBUFGeometry@@AEBUFNavigationEvent@@AEBV2@@Z", "48 89 54 24 10 57 48 83 EC 30 48 C7 44 24 20 FE FF FF FF 48 89 5C 24 40 48 8B DA 48 8B 7C 24 60");
            FModuleManager::RegisterImport("?NativeOnPreviewKeyDown@UUserWidget@@MEAA?AVFReply@@AEBUFGeometry@@AEBUFKeyEvent@@@Z", "40 53 48 81 EC 80 01 00 00 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 70 01 00 00 48 8B DA 4C 8B D1 49 8B D1 48 8D 4C 24 70 E8 ? ? ? ? 41 0F 10 00 4C 8B C8 48 8D 94 24 B0 00 00 00 41 0F 10 48 10 49 8B CA 0F 29 44 24 30 41 0F 10 40 20 0F 29 4C 24 40 F2 41 0F 10 48 30 4C 8D 44 24 30 0F 29 44 24 50 F2 0F 11 4C 24 60 E8 ? ? ? ? 48 8B D0 48 8B CB E8 ? ? ? ? 48 8D 8C 24 B0 00 00 00 E8 ? ? ? ? 48 8B C3 48 8B 8C 24 70 01 00 00 48 33 CC E8 ? ? ? ? 48 81 C4 80 01 00 00 5B C3 CC CC CC CC CC CC CC CC CC CC CC CC CC 40 53 48 81 EC 80 01 00 00 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 70 01 00 00 48 8B DA 4C 8B D1 49 8B D1 48 8D 4C 24 70 E8 ? ? ? ? 41 0F 10 00 4C 8B C8 48 8D 94 24 B0 00 00 00 41 0F 10 48 10 49 8B CA 0F 29 44 24 30 41 0F 10 40 20 0F 29 4C 24 40 F2 41 0F 10 48 30 4C 8D 44 24 30 0F 29 44 24 50 F2 0F 11 4C 24 60 E8 ? ? ? ? 48 8B D0 48 8B CB E8 ? ? ? ? 48 8D 8C 24 B0 00 00 00 E8 ? ? ? ? 48 8B C3 48 8B 8C 24 70 01 00 00 48 33 CC E8 ? ? ? ? 48 81 C4 80 01 00 00 5B C3 CC CC CC CC CC CC CC CC CC CC CC CC CC 40 53 48 81 EC 80 01 00 00");
            FModuleManager::RegisterImport("?NativeOnPreviewMouseButtonDown@UUserWidget@@MEAA?AVFReply@@AEBUFGeometry@@AEBUFPointerEvent@@@Z", base_module + 0x3EDC7E0);
            FModuleManager::RegisterImport("?NativeOnRemovedFromFocusPath@UUserWidget@@MEAAXAEBUFFocusEvent@@@Z", base_module + 0x3EDC320);
            FModuleManager::RegisterImport("?NativeOnTouchEnded@UUserWidget@@MEAA?AVFReply@@AEBUFGeometry@@AEBUFPointerEvent@@@Z", base_module + 0x3EDCE50);
            FModuleManager::RegisterImport("?NativeOnTouchGesture@UUserWidget@@MEAA?AVFReply@@AEBUFGeometry@@AEBUFPointerEvent@@@Z", base_module + 0x3EDCCA0);
            FModuleManager::RegisterImport("?NativeOnTouchGesture@UUserWidget@@MEAA?AVFReply@@AEBUFGeometry@@AEBUFPointerEvent@@@Z", base_module + 0x3EDCCA0);
            FModuleManager::RegisterImport("?NativeOnTouchMoved@UUserWidget@@MEAA?AVFReply@@AEBUFGeometry@@AEBUFPointerEvent@@@Z", base_module + 0x3EDCDC0);
            FModuleManager::RegisterImport("?NativeOnTouchStarted@UUserWidget@@MEAA?AVFReply@@AEBUFGeometry@@AEBUFPointerEvent@@@Z", base_module + 0x3EDCD30);
            FModuleManager::RegisterImport("?NativePaint@UUserWidget@@MEBAXAEAUFPaintContext@@@Z", base_module + 0x3EDC050);
            FModuleManager::RegisterImport("?NativePreConstruct@UUserWidget@@MEAAXXZ", base_module + 0x3EDB810);
            FModuleManager::RegisterImport("?NativeSupportsKeyboardFocus@UUserWidget@@MEBA_NXZ", base_module + 0x3EDC070);
            FModuleManager::RegisterImport("?NativeOnFocusLost@UUserWidget@@MEAAXAEBUFFocusEvent@@@Z", base_module + 0x3EDC110);
            FModuleManager::RegisterImport("?NeedsLoadForServer@UVisual@@UEBA_NXZ", base_module + 0x3EBEAA0);
            FModuleManager::RegisterImport("?OnAnimationFinished_Implementation@UUserWidget@@UEAAXPEBVUWidgetAnimation@@@Z", VoidSub);
            FModuleManager::RegisterImport("?OnAnimationStarted_Implementation@UUserWidget@@UEAAXPEBVUWidgetAnimation@@@Z", VoidSub);
            FModuleManager::RegisterImport("?OnBindingChanged@UWidget@@MEAAXAEBVFName@@@Z", VoidSub);
            FModuleManager::RegisterImport("?OnLevelRemovedFromWorld@UUserWidget@@MEAAXPEAVULevel@@PEAVUWorld@@@Z", "48 85 D2 75 ? 48 89 5C 24 08");
            FModuleManager::RegisterImport("?OnWidgetRebuilt@UUserWidget@@MEAAXXZ", "4C 8B DC 49 89 5B 18 57 48 83 EC 30 48 8B B9 E0 01 00 00");
            FModuleManager::RegisterImport("?PostDuplicate@UUserWidget@@UEAAX_N@Z", base_module + 0x3ED8D90);
            FModuleManager::RegisterImport("?PostEditImport@UUserWidget@@UEAAXXZ", NullSub);
            FModuleManager::RegisterImport("?PostLoad@UUserWidget@@UEAAXXZ", base_module + 0x3EDD270);
            FModuleManager::RegisterImport("?PreSave@UUserWidget@@UEAAXPEBVITargetPlatform@@@Z", base_module + 0x3EDD0F0);
            FModuleManager::RegisterImport("?RebuildWidget@UUserWidget@@MEAA?AV?$TSharedRef@VSWidget@@$0A@@@XZ", base_module + 0x3ED9AF0);
            FModuleManager::RegisterImport("?ReleaseSlateResources@UUserWidget@@UEAAX_N@Z", NullSub);
            FModuleManager::RegisterImport("?RemoveFromParent@UUserWidget@@UEAAXXZ", base_module + 0x3EDB3C0);
            FModuleManager::RegisterImport("?Serialize@UUserWidget@@UEAAXAEAVFArchive@@@Z", base_module + 0x3EDD330);
            FModuleManager::RegisterImport("?SetContentForSlot@UUserWidget@@UEAAXVFName@@PEAVUWidget@@@Z", base_module + 0x3EDA220);
            FModuleManager::RegisterImport("?SetIsEnabled@UWidget@@UEAAX_N@Z", base_module + 0x3EBF7D0);
            FModuleManager::RegisterImport("?SetVisibility@UWidget@@UEAAXW4ESlateVisibility@@@Z", base_module + 0x3EBF8A0);
            FModuleManager::RegisterImport("?SynchronizeProperties@UUserWidget@@UEAAXXZ", base_module + 0x3EC1380);
            FModuleManager::RegisterImport("?TemplateInitInner@UUserWidget@@MEAAXXZ", base_module + 0x3ED86C0);
            FModuleManager::RegisterImport("?_getUObject@UUserWidget@@UEBAPEAVUObject@@XZ", base_module + 0x3E8F190);
            FModuleManager::RegisterImport("?Z_Construct_UClass_UUserWidget@@YAPEAVUClass@@XZ", base_module + 0x3F9BBA0);
            FModuleManager::RegisterImport("?StaticClass@UUserWidget@@SAPEAVUClass@@XZ", base_module + 0x3F9F590);
            FModuleManager::RegisterImport("??0UUserWidget@@QEAA@AEAVFVTableHelper@@@Z", base_module + 0x3ED8310);
            FModuleManager::RegisterImport("?NativeOnFocusChanging@UUserWidget@@MEAAXAEBVFWeakWidgetPath@@AEBVFWidgetPath@@AEBUFFocusEvent@@@Z", base_module + 0x3EDC120);
            FModuleManager::RegisterImport("?NativeOnDrop@UUserWidget@@MEAA_NAEBUFGeometry@@AEBVFDragDropEvent@@PEAVUDragDropOperation@@@Z", base_module + 0x3EDCC30);
            FModuleManager::RegisterImport("?NativeOnDragOver@UUserWidget@@MEAA_NAEBUFGeometry@@AEBVFDragDropEvent@@PEAVUDragDropOperation@@@Z", base_module + 0x3EDCBD0);
            FModuleManager::RegisterImport("?NativeOnDragLeave@UUserWidget@@MEAAXAEBVFDragDropEvent@@PEAVUDragDropOperation@@@Z", base_module + 0x3EDCBA0);
            FModuleManager::RegisterImport("?NativeOnDragEnter@UUserWidget@@MEAAXAEBUFGeometry@@AEBVFDragDropEvent@@PEAVUDragDropOperation@@@Z", base_module + 0x3EDCB40);
            FModuleManager::RegisterImport("?NativeOnDragDetected@UUserWidget@@MEAAXAEBUFGeometry@@AEBUFPointerEvent@@AEAPEAVUDragDropOperation@@@Z", base_module + 0x3EDCB00);
            FModuleManager::RegisterImport("?NativeOnDragCancelled@UUserWidget@@MEAAXAEBVFDragDropEvent@@PEAVUDragDropOperation@@@Z", base_module + 0x3EDCC90);
            FModuleManager::RegisterImport("?NativeOnCursorQuery@UUserWidget@@MEAA?AVFCursorReply@@AEBUFGeometry@@AEBUFPointerEvent@@@Z", base_module + 0x3EDD010);
            FModuleManager::RegisterImport("?NativeOnAnalogValueChanged@UUserWidget@@MEAA?AVFReply@@AEBUFGeometry@@AEBUFAnalogInputEvent@@@Z", base_module + 0x3EDC690);
            FModuleManager::RegisterImport("?NativeOnAddedToFocusPath@UUserWidget@@MEAAXAEBUFFocusEvent@@@Z", base_module + 0x3EDC310);
            FModuleManager::RegisterImport("?NativeTick@UUserWidget@@MEAAXAEBUFGeometry@@M@Z", base_module + 0x3EDB850);
            FModuleManager::RegisterImport("??0UUserWidget@@QEAA@AEBVFObjectInitializer@@@Z", base_module + 0x3ED8310);
            FModuleManager::RegisterImport("??1UUserWidget@@UEAA@XZ", base_module + 0x3ED8520);
            FModuleManager::RegisterImport("?AddToScreen@UUserWidget@@MEAAXPEAVULocalPlayer@@H@Z", base_module + 0x3EDA620);
            FModuleManager::RegisterImport("?BeginDestroy@UUserWidget@@UEAAXXZ", base_module + 0x3ED8CD0);
            FModuleManager::RegisterImport("?GetContentForSlot@UUserWidget@@UEBAPEAVUWidget@@VFName@@@Z", base_module + 0x3EDA1E0);
            FModuleManager::RegisterImport("?GetOwningPlayer@UUserWidget@@UEBAPEAVAPlayerController@@XZ", base_module + 0x3EDB650);
            FModuleManager::RegisterImport("?GetSlotNames@UUserWidget@@UEBAXAEAV?$TArray@VFName@@VFDefaultAllocator@@@@@Z", base_module + 0x3EDA000);
            FModuleManager::RegisterImport("?GetWorld@UUserWidget@@UEBAPEAVUWorld@@XZ", base_module + 0x3ED9750);
            FModuleManager::RegisterImport("?Initialize@UUserWidget@@UEAA_NXZ", base_module + 0x3ED8A90);
            FModuleManager::RegisterImport("?InitializeInputComponent@UUserWidget@@MEAAXXZ", base_module + 0x3EDBD30);
            FModuleManager::RegisterImport("?NativeDestruct@UUserWidget@@MEAAXXZ", base_module + 0x3EDB830);
            FModuleManager::RegisterImport("?NativeConstruct@UUserWidget@@MEAAXXZ", base_module + 0x3EDB820);
            FModuleManager::RegisterImport("?Modify@UWidget@@UEAA_N_N@Z", base_module + 0x3EC1110);
            FModuleManager::RegisterImport("?IsAsset@UUserWidget@@UEBA_NXZ", NullSub);
            FModuleManager::RegisterImport("?InitializeNativeClassData@UUserWidget@@MEAAXXZ", VoidSub);
            FModuleManager::RegisterImport("?NativeIsInteractable@UUserWidget@@MEBA_NXZ", base_module + 0x3EDC060);
            FModuleManager::RegisterImport("?GUObjectArray@@3VFUObjectArray@@A", base_module + 0x8D4BDE0);
            FModuleManager::RegisterImport("?FindFunctionChecked@UObject@@QEBAPEAVUFunction@@VFName@@@Z", base_module + 0x340C000);
            FModuleManager::RegisterImport("?RegisterFunctions@FNativeFunctionRegistrar@@SAXPEAVUClass@@PEBU?$TNameNativePtrPair@D@@H@Z", base_module + 0x32ED200);
            FModuleManager::RegisterImport("?ExportTextItem@UObjectPropertyBase@@UEBAXAEAVFString@@PEBX1PEAVUObject@@H2@Z", base_module + 0x33E6E80);
            FModuleManager::RegisterImport("?GetStaticStruct@@YAPEAVUScriptStruct@@P6APEAV1@XZPEAVUObject@@PEB_W_KI@Z", base_module + 0x341AB50);
            FModuleManager::RegisterImport("?Z_Construct_UClass_UAnimMontage_NoRegister@@YAPEAVUClass@@XZ", base_module + 0x4C316E0);
        }
        
        auto on_unreal_init() -> void override
        {
            base_module = static_cast<unsigned char*>(SigScannerStaticData::m_modules_info.array[static_cast<size_t>(ScanTarget::MainExe)].lpBaseOfDll);

            Hook::RegisterProcessEventPreCallback(&process_event_hook);
        }

        static inline bool bModuleLoaded;
        
        auto static process_event_hook(UObject* Context, UFunction* Function, void* Parms) -> void
        {
            if (bModuleLoaded) return;
            bModuleLoaded = true;
            if (!FModuleManager::InitializeWrapper()) return;

            GNatives = (UnrealScriptFunctionWrapper*)(base_module + 0x92A1D00);
            GetDynamicStructMap.assign_address(base_module + 0x341AAC0);
            GetDeferredCompiledInStructRegistration.assign_address(base_module + 0x341AA50);
            NotifyRegistrationEvent.assign_address(base_module + 0x32C7970);
            GetDeferredClassRegistration.assign_address(base_module + 0x341AF60);
            ObjInitializerConstructor.assign_address(base_module + 0x34259B0);
            FTextGetEmpty.assign_address(base_module + 0x116AB20);
            
            CallFunctionByNameWithArguments.assign_address(base_module + 0x340B150);
            SignatureContainer uobject_constructor{
                {{"48 89 4C 24 08 53 55 56 57 41 56 48 83 EC 60 48 C7 44 24 30 FE FF FF FF"}},
                    [&](const SignatureContainer & self) {
                        if (!self.get_match_address())
                            Output::send(std::format(STR("Failed to resolve symbol '??0UObject@@QEAA@AEAVFVTableHelper@@@Z': Signature not found\n")));
                        else
                            ObjConstructor.assign_address(self.get_match_address());
                        return true;
                    },
                [](SignatureContainer & self) {},
            };
            
            std::vector<SignatureContainer> signature_containers;
            signature_containers.push_back(uobject_constructor);
      
            SinglePassScanner::SignatureContainerMap signature_containers_map;

            signature_containers_map.emplace(ScanTarget::MainExe, signature_containers);

            SinglePassScanner::start_scan(signature_containers_map);

            register_symbols();
            
            const auto module_directory = std::filesystem::path(UE4SSProgram::get_program().get_module_directory());
            const auto plugin_directory = module_directory.parent_path().parent_path().parent_path() / "Plugins" / "DialoguePlugin" / "Binaries" / "Win64";

            if (!exists(plugin_directory)) return;
            
            SetDllDirectory(plugin_directory.c_str());

            const auto dll_path = plugin_directory.wstring() + STR("\\TresGame-DialoguePlugin-Win64-Shipping.dll");

            FModuleManager::Get().AddModule(FName(STR("DialoguePlugin"), FNAME_Add), FString(dll_path.c_str()));
            FModuleManager::Get().LoadModule(FName(STR("DialoguePlugin")));
        }
    };//class
}

/**
* export the start_mod() and uninstall_mod() functions to
* be used by the core ue4ss system to load in our dll mod
*/
#define DIALOGUEPLUGIN_API __declspec(dllexport) 
extern "C" {
    DIALOGUEPLUGIN_API CppUserModBase* start_mod(){ return new DialoguePlugin::DialoguePluginKH3(); }
    DIALOGUEPLUGIN_API void uninstall_mod(CppUserModBase* mod) { delete mod; }
}