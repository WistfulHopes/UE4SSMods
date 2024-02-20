#include <DynamicOutput/Output.hpp>
#include <Mod/CppUserModBase.hpp>
#include <Modules/ModuleManager.hpp>
#include <polyhook2/Exceptions/AVehHook.hpp>
#include <SigScanner/SinglePassSigScanner.hpp>
#include <Unreal/UObjectGlobals.hpp>
#include <Unreal/UObject.hpp>

#include "Hooks.hpp"
#include "UE4SSProgram.hpp"
#include "UnrealCoreStructs.hpp"

namespace ExamplePlugin
{
    using namespace RC;
    using namespace Unreal;
    
    /**
    * ExamplePluginMod: UE4SS c++ mod class defintion
    */
    class ExamplePluginMod : public CppUserModBase
    {
    public:
        
        // constructor
        ExamplePluginMod() {
            ModVersion = STR("0.1");
            ModName = STR("ExamplePluginMod");
            ModAuthors = STR("UE4SS");
            ModDescription = STR("A basic template C++ mod");
            // Do not change this unless you want to target a UE4SS version
            // other than the one you're currently building with somehow.
            //ModIntendedSDKVersion = STR("2.6");
        }
        
        // destructor
        ~ExamplePluginMod() override {
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

        static inline Function<bool(UObject*, TCHAR*, FOutputDevice*, UObject*, bool)> CallFunctionByNameWithArguments;
        
        static bool ProcessConsoleExec(UObject* pThis, TCHAR* Cmd, FOutputDevice* Ar, UObject* Executor)
        {
            return CallFunctionByNameWithArguments(pThis, Cmd, Ar, Executor, false);
        }

        static auto register_symbols_old() -> void
        {
            FModuleManager::RegisterImport("?UnknownFunction_1@UObjectBase@@MEAA_NXZ", TrueSub);
            FModuleManager::RegisterImport("?GetNameTableForDebuggerVisualizers_MT@FName@@SAPEAPEAPEAUFNameEntry@@XZ", "48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8B 0D ? ? ? ? 48 89 5C 24 20 48 85 C9 75 ? E8 ? ? ? ? 48 8B 0D ? ? ? ? 48 8B 01 45 33 C0 BA 08 04 00 00");
            FModuleManager::RegisterImport("?Realloc@FMemory@@SAPEAXPEAX_KI@Z", FMemory::Realloc);
            FModuleManager::RegisterImport("?Free@FMemory@@SAXPEAX@Z", FMemory::Free);
            FModuleManager::RegisterImport("??0FName@@QEAA@PEB_WW4EFindName@@@Z", "4053 4883EC30 488BD9 4885D2 7421 458BC8 C74424 28 FFFFFFFF 4533C0 C644242001 E8DA040000 488BC3");
            FModuleManager::RegisterImport("?QuantizeSize@FMemory@@SA_K_KI@Z", FMemory::QuantizeSize);
            FModuleManager::RegisterImport("?Memcpy@FGenericPlatformString@@CAPEAXPEAXPEBX_K@Z", static_cast<void*(*)(void*,const void*,SIZE_T)>(FMemory::Memcpy));
            FModuleManager::RegisterImport("??1FString@@QEAA@XZ", FStringDestructor);
            FModuleManager::RegisterImport("?GCoreObjectArrayForDebugVisualizers@@3PEAVFFixedUObjectArray@@EA", (void*)1);
            FModuleManager::RegisterImport("??0ForAnyElementType@FHeapAllocator@@QEAA@XZ", VoidSub);
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
            FModuleManager::RegisterImport("?IsNameStableForNetworking@UObject@@UEBA_NXZ", "48 83 EC 28 F7 41 08 00 00 0C 00");
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
            FModuleManager::RegisterImport("?Rename@UObject@@UEAA_NPEB_WPEAV1@I@Z", "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 48 83 EC 20 41 8B F1 49 8B F8 48 8B EA");
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
            FModuleManager::RegisterImport("?Serialize@UObject@@UEAAXAEAVFArchive@@@Z", "48 89 5C 24 20 57 48 83 EC 20 48 8B 41 28 48 8B DA");
            FModuleManager::RegisterImport("?FinishDestroy@UObject@@UEAAXXZ", "40 57 48 83 EC 50 48 C7 44 24 30 FE FF FF FF 48 89 5C 24 68 48 89 74 24 70 48 8B F9 8B 41 08");
            FModuleManager::RegisterImport("?IsReadyForFinishDestroy@UObject@@UEAA_NXZ", "48 83 EC 28 48 81 C1 A8 03 00 00");
            FModuleManager::RegisterImport("?BeginDestroy@UObject@@UEAAXXZ", "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 48 8B 79 28 33 F6");
            FModuleManager::RegisterImport("?PostLoadSubobjects@UObject@@UEAAXPEAUFObjectInstancingGraph@@@Z", "48 8B C4 57 41 54 41 55 41 56 41 57 48 83 EC 70 48 C7 40 88 FE FF FF FF 48 89 58 10 48 89 68 18 48 89 70 20 0F 29 70 C8 0F 29 78 B8 44 0F 29 40 A8 48 8B F9");
            FModuleManager::RegisterImport("?PostLoad@UObject@@UEAAXXZ", "48 8B C4 55 41 54 41 55 41 56 41 57 48 8D A8 08 F7 FF FF 48 81 EC D0 09 00 00 48 C7 45 B8 FE FF FF FF");
            FModuleManager::RegisterImport("?Modify@UObject@@UEAA_N_N@Z", "48 8B C4 88 50 10 57 41 54 41 55 41 56 41 57 48 83 EC 50 48 C7 40 A8 FE FF FF FF 48 89 58 08 48 89 68 18 48 89 70 20 44 0F B6 E2");
            FModuleManager::RegisterImport("?PreSave@UObject@@UEAAXPEBVITargetPlatform@@@Z", VoidSub);
            FModuleManager::RegisterImport("?PostSaveRoot@UObject@@UEAAX_N@Z", VoidSub);
            FModuleManager::RegisterImport("?PreSaveRoot@UObject@@UEAA_NPEB_W@Z", NullSub);
            FModuleManager::RegisterImport("?PostCDOContruct@UObject@@UEAAXXZ", VoidSub);
            FModuleManager::RegisterImport("?PostInitProperties@UObject@@UEAAXXZ", "40 53 48 83 EC 30 48 8B D9 48 C7 44 24 20 00 00 00 00 48 8B 49 18 45 33 C9 41 B0 01 48 8B D3 48 8B 01 FF 90 D0 02 00 00 0F B6 83 A6 00 00 00");
            FModuleManager::RegisterImport("?GetDetailedInfoInternal@UObject@@MEBA?AVFString@@XZ", "48 89 54 24 10 53 48 83 EC 30 48 C7 44 24 20 FE FF FF FF 48 8B DA 33 C0 48 89 02 48 C7 42 08 1B 00 00 00 48 8B CA E8 ? ? ? ? 48 8B 0B 0F 10 05 ? ? ? ? 0F 11 01 0F 10 0D ? ? ? ? 0F 11 49 10 0F 10 05 ? ? ? ? 0F 11 41 20 8B 05 ? ? ? ? 89 41 30 0F B7 05 ? ? ? ? 66 89 41 34 48 8B C3 48 83 C4 30 5B C3 CC CC CC CC CC 48 8B 01");
            FModuleManager::RegisterImport("?OnClusterMarkedAsPendingKill@UObjectBaseUtility@@UEAAXXZ", VoidSub);
            FModuleManager::RegisterImport("?CreateCluster@UObjectBaseUtility@@UEAAXXZ", "48 8B C4 55 41 54 41 55 41 56 41 57 48 8D A8 F8 FE FF FF 48 81 EC E0 01 00 00 48 C7 44 24 60 FE FF FF FF 48 89 58 10 48 89 70 18 48 89 78 20 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 D8 00 00 00");
            FModuleManager::RegisterImport("?CanBeInCluster@UObjectBaseUtility@@UEBA_NXZ", "0F B6 81 69 01 00 00");
            FModuleManager::RegisterImport("?CanBeClusterRoot@UObjectBaseUtility@@UEBA_NXZ", NullSub);
            FModuleManager::RegisterImport("?DeferredRegister@UObjectBase@@MEAAXPEAVUClass@@PEB_W1@Z", "48 89 5C 24 10 48 89 74 24 18 57 48 83 EC 30 48 8B DA 48 8B F9 49 8B D0");
            FModuleManager::RegisterImport("?RegisterDependencies@UObjectBase@@MEAAXXZ", VoidSub);
            FModuleManager::RegisterImport("?StaticFindObjectFast@@YAPEAVUObject@@PEAVUClass@@PEAV1@VFName@@_N3W4EObjectFlags@@W4EInternalObjectFlags@@@Z", "40 57 48 83 EC 50 48 C7 44 24 40 FE FF FF FF 48 89 5C 24 60 48 89 6C 24 68 48 89 74 24 70 41 0F B6 F9");
            FModuleManager::RegisterImport("?GetConvertedDynamicPackageNameToTypeName@@YAAEAV?$TMap@VFName@@V1@VFDefaultSetAllocator@@U?$TDefaultMapHashableKeyFuncs@VFName@@V1@$0A@@@@@XZ", "40 53 48 83 EC 30 48 C7 44 24 20 FE FF FF FF BA 28 00 00 00 65 48 8B 04 25 58 00 00 00 48 8B 08 8B 04 0A 39 05 ? ? ? ? 7F ? 48 8D 05 ? ? ? ? 48 83 C4 30 5B C3 48 8D 0D ? ? ? ? E8 ? ? ? ? 83 3D ? ? ? ? ? 75 ? 48 8D 1D ? ? ? ? 48 89 5C 24 40 48 8D 05 ? ? ? ? 48 89 44 24 40 48 8D 0D ? ? ? ? E8 ? ? ? ? 90 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B C3 48 83 C4 30 5B C3 CC CC CC CC CC CC CC CC CC 48 89 54 24 10");
            FModuleManager::RegisterImport("?UClassCompiledInDefer@@YAXPEAUFFieldCompiledInInfo@@PEB_W_KI@Z", "40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 F9 48 81 EC F8 00 00 00 48 C7 44 24 70 FE FF FF FF");
            FModuleManager::RegisterImport("?UObjectCompiledInDefer@@YAXP6APEAVUClass@@XZ0PEB_W1_N1P6AXAEAV?$TMap@VFName@@V1@VFDefaultSetAllocator@@U?$TDefaultMapHashableKeyFuncs@VFName@@V1@$0A@@@@@@Z@Z", "40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 F9 48 81 EC F8 00 00 00 48 C7 44 24 70 FE FF FF FF");
            FModuleManager::RegisterImport("?StaticClass@UObject@@SAPEAVUClass@@XZ", "48 81 EC 98 00 00 00 48 83 3D ? ? ? ? ? 0F 85 ? ? ? ? E8 ? ? ? ? 48 89 44 24 70 E8 ? ? ? ? 48 89 44 24 78 48 8D 0D ? ? ? ? 48 83 C1 02 48 89 8C 24 80 00 00 00 E8 ? ? ? ? C6 44 24 68 00 48 8D 0D ? ? ? ? 48 89 4C 24 60 48 8D 0D ? ? ? ? 48 89 4C 24 58 48 8D 0D ? ? ? ? 48 89 4C 24 50 48 8D 0D ? ? ? ? 48 89 4C 24 48 48 8D 0D ? ? ? ? 48 89 4C 24 40 48 8B 4C 24 70 48 89 4C 24 38 48 8B 4C 24 78 48 89 4C 24 30 C7 44 24 28 01 01 00 00");
            FModuleManager::RegisterImport("?StaticConfigName@UObject@@SAPEB_WXZ", UObject_StaticConfigName);
            FModuleManager::RegisterImport("??0UObject@@QEAA@AEAVFVTableHelper@@@Z", "48 83 EC 38 80 3D ? ? ? ? ? 75 ?");
            FModuleManager::RegisterImport("?AddReferencedObjects@UObject@@SAXPEAV1@AEAVFReferenceCollector@@@Z", VoidSub);
			FModuleManager::RegisterImport("?StaticLink@UStruct@@QEAAX_N@Z", "40 57 48 81 EC C0 00 00 00 48 C7 44 24 20 FE FF FF FF 48 89 9C 24 D0 00 00 00 0F B6 DA");
			FModuleManager::RegisterImport("?SetCppTypeInfo@UClass@@QEAAXPEAUICppClassTypeInfo@@@Z", "48 89 91 20 01 00 00 C3");
			FModuleManager::RegisterImport("?GetPrivateStaticClassBody@@YAXPEB_W0AEAPEAVUClass@@P6AXXZIW4EClassFlags@@_K0P6AXAEBVFObjectInitializer@@@ZP6APEAVUObject@@AEAVFVTableHelper@@@ZP6AXPEAV4@AEAVFReferenceCollector@@@ZP6APEAV1@XZP6APEAV1@XZ_N@Z", "48 89 5C 24 10 48 89 6C 24 18 57 41 56 41 57 48 83 EC 60");
            FModuleManager::RegisterImport("?Z_Construct_UClass_UObject@@YAPEAVUClass@@XZ", "48 83 EC 48 48 83 3D ? ? ? ? ? 0F 85 ? ? ? ? E8 ? ? ? ? E8 ? ? ? ? 48 89 05 ? ? ? ?");
            FModuleManager::RegisterImport("?StaticClass@UPackage@@SAPEAVUClass@@XZ", "4C 8B DC 48 83 EC 78 48 8B 05 ? ? ? ? 48 85 C0 0F 85 ? ? ? ? 88 44 24 68 4C 8D 0D ? ? ? ? 48 8D 05 ? ? ? ? 49 89 43 E8 4C 8D 05 ? ? ? ? 49 89 43 E0");
            FModuleManager::RegisterImport("?AddToCluster@UObjectBaseUtility@@UEAAXPEAV1@_N@Z", "40 55 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 10 FF FF FF 48 81 EC F0 01 00 00 48 C7 44 24 70 FE FF FF FF");
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
            FModuleManager::RegisterImport("?StaticAllocateObject@@YAPEAVUObject@@PEAVUClass@@PEAV1@VFName@@W4EObjectFlags@@W4EInternalObjectFlags@@_NPEA_N@Z", "44 89 4C 24 20 4C 89 44 24 18 48 89 54 24 10 55 53 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 F9");
            FModuleManager::RegisterImport("?BuildSubobjectMapping@UObject@@UEBAXPEAV1@AEAV?$TMap@PEAVUObject@@PEAV1@VFDefaultSetAllocator@@U?$TDefaultMapHashableKeyFuncs@PEAVUObject@@PEAV1@$0A@@@@@@Z", "48 89 54 24 10 48 83 EC 68");
            FModuleManager::RegisterImport("?IsDestructionThreadSafe@UObject@@UEBA_NXZ", TrueSub);
        }

        static auto register_symbols_new() -> void
        {
            FModuleManager::RegisterImport("?Realloc@FMemory@@SAPEAXPEAX_KI@Z", FMemory::Realloc);
            FModuleManager::RegisterImport("?Free@FMemory@@SAXPEAX@Z", FMemory::Free);
            FModuleManager::RegisterImport("?QuantizeSize@FMemory@@SA_K_KI@Z", FMemory::QuantizeSize);
            FModuleManager::RegisterImport("?Memcpy@FGenericPlatformString@@CAPEAXPEAXPEBX_K@Z", static_cast<void*(*)(void*,const void*,SIZE_T)>(FMemory::Memcpy));
            FModuleManager::RegisterImport("??1FString@@QEAA@XZ", FStringDestructor);
            FModuleManager::RegisterImport("?GCoreObjectArrayForDebugVisualizers@@3PEAVFChunkedFixedUObjectArray@@EA", (void*)1);
            FModuleManager::RegisterImport("?GetBlocks@FNameDebugVisualizer@@SAPEAPEAEXZ", NullSub);
            FModuleManager::RegisterImport("?ZeroVector@FVector@@2U1@B", &ZeroVector);
            FModuleManager::RegisterImport("?Malloc@FMemory@@SAPEAX_KI@Z", FMemory::Malloc);
            FModuleManager::RegisterImport("?PreDestroyFromReplication@UObject@@UEAAXXZ", VoidSub);
            FModuleManager::RegisterImport("?PostRepNotifies@UObject@@UEAAXXZ", VoidSub);
            FModuleManager::RegisterImport("?PostNetReceive@UObject@@UEAAXXZ", VoidSub);
            FModuleManager::RegisterImport("?PreNetReceive@UObject@@UEAAXXZ", VoidSub);
            FModuleManager::RegisterImport("?GetSubobjectsWithStableNamesForNetworking@UObject@@UEAAXAEAV?$TArray@PEAVUObject@@V?$TSizedDefaultAllocator@$0CA@@@@@@Z", VoidSub);
            FModuleManager::RegisterImport("?IsSupportedForNetworking@UObject@@UEBA_NXZ", "48 8B 01 48 FF A0 C0 01 00 00 CC CC CC CC CC CC 48 89 5C 24 08");
            FModuleManager::RegisterImport("?IsFullNameStableForNetworking@UObject@@UEBA_NXZ", "40 53 48 83 EC 20 48 8B D9 48 8B 49 20 48 85 C9 74 ?? 48 8B 01 FF 90 B8 01 00 00");
            FModuleManager::RegisterImport("?IsNameStableForNetworking@UObject@@UEBA_NXZ", "48 83 EC 28 F7 41 08 00 00 0C 00");
            FModuleManager::RegisterImport("?GetLifetimeReplicatedProps@UObject@@UEBAXAEAV?$TArray@VFLifetimeProperty@@V?$TSizedDefaultAllocator@$0CA@@@@@@Z", VoidSub);
            FModuleManager::RegisterImport("?TagSubobjects@UObject@@UEAAXW4EObjectFlags@@@Z", "48 89 5C 24 18 48 89 74 24 20 57 48 81 EC F0 00 00 00 48 8B 05 ?? ?? ?? ?? 48 33 C4 48 89 84 24 E0 00 00 00 8B FA");
            FModuleManager::RegisterImport("?IsSafeForRootSet@UObject@@UEBA_NXZ", "40 53 48 83 EC 20 48 8B D9 E8 ?? ?? ?? ?? 84 C0 75 ? 8B 43 0C");
            FModuleManager::RegisterImport("?IsLocalizedResource@UObject@@UEBA_NXZ", "40 53 48 83 EC 30 48 89 7C 24 40 33 FF");
            FModuleManager::RegisterImport("?GetPrimaryAssetId@UObject@@UEBA?AUFPrimaryAssetId@@XZ", "48 89 5C 24 08 57 48 83 EC 20 83 3D ?? ?? ?? ?? ?? 48 8B DA");
            FModuleManager::RegisterImport("?IsAsset@UObject@@UEBA_NXZ", "40 53 48 83 EC 20 8B 41 08 48 8B D9 A8 50");
            FModuleManager::RegisterImport("?GetAssetRegistryTags@UObject@@UEBAXAEAV?$TArray@UFAssetRegistryTag@UObject@@V?$TSizedDefaultAllocator@$0CA@@@@@@Z", "40 55 57 41 54 41 56 48 8B EC");
            FModuleManager::RegisterImport("?AreNativePropertiesIdenticalTo@UObject@@UEBA_NPEAV1@@Z", TrueSub);
            FModuleManager::RegisterImport("?GetRestoreForUObjectOverwrite@UObject@@UEAAPEAVFRestoreForUObjectOverwrite@@XZ", NullSub);
            FModuleManager::RegisterImport("?GetExporterName@UObject@@UEAA?AVFName@@XZ", UObject_GetExporterName);
            FModuleManager::RegisterImport("?GetResourceSizeEx@UObject@@UEAAXAEAUFResourceSizeEx@@@Z", "48 8B C4 48 89 58 18 56 48 81 EC E0 00 00 00");
            FModuleManager::RegisterImport("?GetNativePropertyValues@UObject@@UEBA_NAEAV?$TMap@VFString@@V1@VFDefaultSetAllocator@@U?$TDefaultMapHashableKeyFuncs@VFString@@V1@$0A@@@@@I@Z", NullSub);
            FModuleManager::RegisterImport("?GetWorld@UObject@@UEBAPEAVUWorld@@XZ", "48 8B 49 20 48 85 C9 74 ?? 48 8B 01 48 FF A0 50 01 00 00 33 C0");
            FModuleManager::RegisterImport("?GetSparseClassDataStruct@UObject@@UEBAPEAVUScriptStruct@@XZ", "48 8B 49 10 48 85 C9 74 ?? 48 8B 01 48 FF A0 48 01 00 00");
            FModuleManager::RegisterImport("?GetDesc@UObject@@UEAA?AVFString@@XZ", UObject_GetDesc);
            FModuleManager::RegisterImport("?Rename@UObject@@UEAA_NPEB_WPEAV1@I@Z", "48 89 5C 24 08 44 89 4C 24 20 55");
            FModuleManager::RegisterImport("?PostReloadConfig@UObject@@UEAAXPEAVFProperty@@@Z", VoidSub);
            FModuleManager::RegisterImport("?PostEditImport@UObject@@UEAAXXZ", VoidSub);
            FModuleManager::RegisterImport("?ImportCustomProperties@UObject@@UEAAXPEB_WPEAVFFeedbackContext@@@Z", VoidSub);
            FModuleManager::RegisterImport("?ExportCustomProperties@UObject@@UEAAXAEAVFOutputDevice@@I@Z", VoidSub);
            FModuleManager::RegisterImport("?GetPrestreamPackages@UObject@@UEAAXAEAV?$TArray@PEAVUObject@@V?$TSizedDefaultAllocator@$0CA@@@@@@Z", VoidSub);
            FModuleManager::RegisterImport("?GetPreloadDependencies@UObject@@UEAAXAEAV?$TArray@PEAVUObject@@V?$TSizedDefaultAllocator@$0CA@@@@@@Z", "48 89 5C 24 10 48 89 6C 24 18 57 48 83 EC 20 48 8B 79 10");
            FModuleManager::RegisterImport("?GetConfigOverridePlatform@UObject@@UEBAPEB_WXZ", NullSub);
            FModuleManager::RegisterImport("?IsPostLoadThreadSafe@UObject@@UEBA_NXZ", NullSub);
            FModuleManager::RegisterImport("?IsEditorOnly@UObject@@UEBA_NXZ", NullSub);
            FModuleManager::RegisterImport("?NeedsLoadForEditorGame@UObject@@UEBA_NXZ", NullSub);
            FModuleManager::RegisterImport("?NeedsLoadForTargetPlatform@UObject@@UEBA_NPEBVITargetPlatform@@@Z", TrueSub);
            FModuleManager::RegisterImport("?NeedsLoadForServer@UObject@@UEBA_NXZ", "48 83 EC 28 48 8B 51 10 48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 84 C0 0F 94 C0 48 83 C4 28 C3 CC CC 48 89 5C 24 08");
            FModuleManager::RegisterImport("?NeedsLoadForClient@UObject@@UEBA_NXZ", "48 83 EC 28 48 8B 51 10 48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 84 C0 0F 94 C0 48 83 C4 28 C3 CC CC 48 83 EC 28");
            FModuleManager::RegisterImport("?PostDuplicate@UObject@@UEAAX_N@Z", VoidSub);
            FModuleManager::RegisterImport("?PostDuplicate@UObject@@UEAAXW4Type@EDuplicateMode@@@Z", "48 8B 01 83 FA 02 0F 94 C2");
            FModuleManager::RegisterImport("?PostRename@UObject@@UEAAXPEAV1@VFName@@@Z", VoidSub);
            FModuleManager::RegisterImport("?PostInterpChange@UObject@@UEAAXPEAVFProperty@@@Z", VoidSub);
            FModuleManager::RegisterImport("?ShutdownAfterError@UObject@@UEAAXXZ", VoidSub);
            FModuleManager::RegisterImport("?Serialize@UObject@@UEAAXAEAVFArchive@@@Z", "40 53 48 81 EC C0 01 00 00 48 8B 05 ?? ?? ?? ?? 48 33 C4 48 89 84 24 B8 01 00 00 48 8B D9 48 8D 4C 24 28 E8 ?? ?? ?? ?? 48 8B C8 48 8D 54 24 20 E8 ?? ?? ?? ?? 48 8B CB 48 8B 10 E8 ?? ?? ?? ?? 48 8D 4C 24 28 E8 ?? ?? ?? ?? 48 8B 8C 24 B8 01 00 00 48 33 CC E8 ?? ?? ?? ?? 48 81 C4 C0 01 00 00 5B C3 CC CC CC CC CC CC CC CC CC CC CC CC CC 40 53 56");
            FModuleManager::RegisterImport("?Serialize@UObject@@UEAAXVFStructuredArchiveRecord@@@Z", "40 53 56 57 48 83 EC 20 48 8B 02");
            FModuleManager::RegisterImport("?FinishDestroy@UObject@@UEAAXXZ", "40 53 48 83 EC 30 F6 41 0A 01");
            FModuleManager::RegisterImport("?IsReadyForFinishDestroy@UObject@@UEAA_NXZ", TrueSub);
            FModuleManager::RegisterImport("?BeginDestroy@UObject@@UEAAXXZ", "40 53 48 83 EC 30 8B 41 08 48 8B D9 C1 E8 0F");
            FModuleManager::RegisterImport("?PostLoadSubobjects@UObject@@UEAAXPEAUFObjectInstancingGraph@@@Z", "40 53 48 83 EC 20 48 8B 41 10 48 8B D9 8B 90 CC 00 00 00 C1 EA 17");
            FModuleManager::RegisterImport("?PostLoad@UObject@@UEAAXXZ", "48 83 EC 38 48 8B 41 10 8B 90 CC 00 00 00");
            FModuleManager::RegisterImport("?IsReadyForAsyncPostLoad@UObject@@UEBA_NXZ", TrueSub);
            FModuleManager::RegisterImport("?PreSave@UObject@@UEAAXPEBVITargetPlatform@@@Z", VoidSub);
            FModuleManager::RegisterImport("?PostSaveRoot@UObject@@UEAAX_N@Z", VoidSub);
            FModuleManager::RegisterImport("?PreSaveRoot@UObject@@UEAA_NPEB_W@Z", NullSub);
            FModuleManager::RegisterImport("?PostCDOContruct@UObject@@UEAAXXZ", VoidSub);
            FModuleManager::RegisterImport("?PostInitProperties@UObject@@UEAAXXZ", "48 83 EC 38 48 8B D1 48 C7 44 24 20 00 00 00 00");
            FModuleManager::RegisterImport("?GetDetailedInfoInternal@UObject@@MEBA?AVFString@@XZ", "40 53 48 83 EC 20 33 C0 48 8B DA 48 89 02 48 8B CB 48 89 42 08 8D 50 1B E8 ?? ?? ?? ?? 8B 53 08 8D 42 1B 89 43 08 3B 43 0C 7E ?? 48 8B CB E8 ?? ?? ?? ?? 48 8B 0B 48 8D 15 ?? ?? ?? ?? 41 B8 36 00 00 00 E8 ?? ?? ?? ?? 48 8B C3 48 83 C4 20 5B C3 CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC 40 53 48 83 EC 20 48 8B DA");
            FModuleManager::RegisterImport("?OnClusterMarkedAsPendingKill@UObjectBaseUtility@@UEAAXXZ", VoidSub);
            FModuleManager::RegisterImport("?CreateCluster@UObjectBaseUtility@@UEAAXXZ", "40 55 53 41 55 41 56 41 57 48 8D 6C 24 A0");
            FModuleManager::RegisterImport("?CanBeInCluster@UObjectBaseUtility@@UEBA_NXZ", "48 8B 49 20 48 85 C9 74 ?? 48 8B 01 48 FF 60 20");
            FModuleManager::RegisterImport("?CanBeClusterRoot@UObjectBaseUtility@@UEBA_NXZ", NullSub);
            FModuleManager::RegisterImport("?DeferredRegister@UObjectBase@@MEAAXPEAVUClass@@PEB_W1@Z", "48 89 5C 24 10 48 89 74 24 18 57 48 83 EC 20 48 8B DA 48 8B F1 49 8B D0");
            FModuleManager::RegisterImport("?RegisterDependencies@UObjectBase@@MEAAXXZ", VoidSub);
            FModuleManager::RegisterImport("?ConstructUPackage@UE4CodeGen_Private@@YAXAEAPEAVUPackage@@AEBUFPackageParams@1@@Z", "4C 8B DC 56 41 56 48 81 EC 98 01 00 00");
            FModuleManager::RegisterImport("?ConstructUClass@UE4CodeGen_Private@@YAXAEAPEAVUClass@@AEBUFClassParams@1@@Z", "40 56 41 57 48 81 EC 98 01 00 00");
            FModuleManager::RegisterImport("?UClassCompiledInDefer@@YAXPEAUFFieldCompiledInInfo@@PEB_W_KI@Z", "48 89 5C 24 10 48 89 74 24 18 57 48 83 EC 20 48 8B F1 41 B8 01 00 00 00");
            FModuleManager::RegisterImport("?UObjectCompiledInDefer@@YAXP6APEAVUClass@@XZ0PEB_W1_N1P6AXAEAV?$TMap@VFName@@V1@VFDefaultSetAllocator@@U?$TDefaultMapHashableKeyFuncs@VFName@@V1@$0A@@@@@@Z@Z", "40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 F9 48 81 EC E8 00 00 00 48 8B 05 ?? ?? ?? ?? 48 33 C4 48 89 45 EF 80 7D 6F 00");
            FModuleManager::RegisterImport("?StaticClass@UObject@@SAPEAVUClass@@XZ", "4C 8B DC 48 81 EC 88 00 00 00 48 8B 05 ?? ?? ?? ?? 48 85 C0 0F 85 ?? ?? ?? ?? 33 C9 48 8D 05 ?? ?? ?? ?? 49 89 4B F0 4C 8D 0D ?? ?? ?? ?? 88 4C 24 70 4C 8D 05 ?? ?? ?? ?? 49 89 43 E0 48 8D 15 ?? ?? ?? ?? 49 89 43 D8 48 8D 05 ?? ?? ?? ?? 49 89 43 D0 48 8D 05 ?? ?? ?? ?? 49 89 43 C8 48 8D 05 ?? ?? ?? ?? 49 89 43 C0 48 8D 05 ?? ?? ?? ?? 49 89 43 B8 49 89 4B B0 48 8D 0D ?? ?? ?? ?? C7 44 24 30 21 01 00 10");
            FModuleManager::RegisterImport("?StaticConfigName@UObject@@SAPEB_WXZ", UObject_StaticConfigName);
            FModuleManager::RegisterImport("??0UObject@@QEAA@AEAVFVTableHelper@@@Z", "40 53 48 83 EC 20 48 8D 05 ?? ?? ?? ?? 48 8B D9 48 89 01 48 8D 0D ?? ?? ?? ??");
            FModuleManager::RegisterImport("?AddReferencedObjects@UObject@@SAXPEAV1@AEAVFReferenceCollector@@@Z", VoidSub);
			FModuleManager::RegisterImport("?GetPrivateStaticClassBody@@YAXPEB_W0AEAPEAVUClass@@P6AXXZIIW4EClassFlags@@W4EClassCastFlags@@0P6AXAEBVFObjectInitializer@@@ZP6APEAVUObject@@AEAVFVTableHelper@@@ZP6AXPEAV5@AEAVFReferenceCollector@@@ZP6APEAV1@XZP6APEAV1@XZ_NP6AXPEAVUDynamicClass@@@Z@Z", "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 20 57 41 54 41 55 41 56 41 57 48 83 EC 60");
            FModuleManager::RegisterImport("?Z_Construct_UClass_UObject@@YAPEAVUClass@@XZ", "48 83 EC 28 48 8B 05 ?? ?? ?? ?? 48 85 C0 75 ?? 48 8D 15 ?? ?? ?? ?? 48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 48 8B 05 ?? ?? ?? ?? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ?? ?? ?? ?? 48 85 C0 75 ?? 48 8D 15 ?? ?? ?? ?? 48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 48 8B 05 ?? ?? ?? ?? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ?? ?? ?? ?? 48 85 C0 75 ?? 48 8D 15 ?? ?? ?? ?? 48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 48 8B 05 ?? ?? ?? ?? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ?? ?? ?? ?? 48 85 C0 75 ?? 48 8D 15 ?? ?? ?? ?? 48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 48 8B 05 ?? ?? ?? ?? 48 83 C4 28 C3 CC 48 8B 01 48 85 C0 74 ?? 48 8B D1 48 8B C8 E9 ?? ?? ?? ?? C3 CC CC CC CC CC CC CC CC CC CC CC CC 48 89 5C 24 08");
            FModuleManager::RegisterImport("?OverridePerObjectConfigSection@UObject@@UEAAXAEAVFString@@@Z", VoidSub);
            
            SignatureContainer process_event{
                {{"48 8B F2 4C 8B E1 41 B8 FF FF 00 00"}},
                    [&](const SignatureContainer & self) {
                        if (!self.get_match_address())
                            Output::send(std::format(STR("Failed to resolve symbol '?ProcessEvent@UObject@@UEAAXPEAVUFunction@@PEAX@Z': Signature not found\n")));
                        else
                            FModuleManager::RegisterImport("?ProcessEvent@UObject@@UEAAXPEAVUFunction@@PEAX@Z", self.get_match_address() - 0x3F, false);
                        return true;
                },
                [](SignatureContainer & self) {},
            };

            SinglePassScanner::SignatureContainerMap signature_containers = {
                {ScanTarget::MainExe, {process_event}},
            };

            SinglePassScanner::start_scan(signature_containers);

            FModuleManager::RegisterImport("?GetFunctionCallspace@UObject@@UEAAHPEAVUFunction@@PEAUFFrame@@@Z", Return2);
            FModuleManager::RegisterImport("?CallRemoteFunction@UObject@@UEAA_NPEAVUFunction@@PEAXPEAUFOutParmRec@@PEAUFFrame@@@Z", NullSub);
            FModuleManager::RegisterImport("?ProcessConsoleExec@UObject@@UEAA_NPEB_WAEAVFOutputDevice@@PEAV1@@Z", ProcessConsoleExec);
            FModuleManager::RegisterImport("?RegenerateClass@UObject@@UEAAPEAVUClass@@PEAV2@PEAV1@@Z", NullSub);
            FModuleManager::RegisterImport("?MarkAsEditorOnlySubobject@UObject@@UEAAXXZ", VoidSub);
            FModuleManager::RegisterImport("?CheckDefaultSubobjectsInternal@UObject@@MEBA_NXZ", "44 0F B6 CA 4C 8B C1");
            FModuleManager::RegisterImport("?ValidateGeneratedRepEnums@UObject@@UEBAXAEBV?$TArray@UFRepRecord@@V?$TSizedDefaultAllocator@$0CA@@@@@@Z", VoidSub);
            FModuleManager::RegisterImport("?SetNetPushIdDynamic@UObject@@EEAAXH@Z", VoidSub);
            FModuleManager::RegisterImport("?GetNetPushIdDynamic@UObject@@EEBAHXZ", FSub);
            FModuleManager::RegisterImport("??0UObject@@QEAA@XZ", "48 89 5C 24 18 57 48 83 EC 40 48 8D 05 ?? ?? ?? ??");
            FModuleManager::RegisterImport("??1UObjectBase@@UEAA@XZ", "40 57 48 83 EC 30 48 8D 05 ?? ?? ?? ??");
            FModuleManager::RegisterImport("?StaticAllocateObject@@YAPEAVUObject@@PEBVUClass@@PEAV1@VFName@@W4EObjectFlags@@W4EInternalObjectFlags@@_NPEA_N@Z", "48 89 5C 24 20 4C 89 44 24 ?? 55 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 E0 FD FF FF");
            FModuleManager::RegisterImport("?BuildSubobjectMapping@UObject@@UEBAXPEAV1@AEAV?$TMap@PEAVUObject@@PEAV1@VFDefaultSetAllocator@@U?$TDefaultMapHashableKeyFuncs@PEAVUObject@@PEAV1@$0A@@@@@@Z", "48 89 5C 24 08 48 89 54 24 10 57 48 83 EC 60");
            FModuleManager::RegisterImport("?IsDestructionThreadSafe@UObject@@UEBA_NXZ", TrueSub);
        }
        
        auto on_unreal_init() -> void override
        {
            Hook::RegisterProcessEventPreCallback(&process_event_hook);
        }

        static inline bool bModuleLoaded;
        
        auto static process_event_hook([[maybe_unused]] Unreal::UObject* Context, [[maybe_unused]] Unreal::UFunction* Function, [[maybe_unused]] void* Parms) -> void
        {
            if (bModuleLoaded) return;
            bModuleLoaded = true;
            if (!FModuleManager::InitializeWrapper()) return;

            SignatureContainer process_exec{
                        {{"40 55 53 56 57 41 54 41 55 41 56 41 57 48 81 EC B8 02 00 00"}},
                            [&](const SignatureContainer & self) {
                                if (!self.get_match_address())
                                    Output::send(std::format(STR("Failed to resolve symbol '?CallFunctionByNameWithArguments@UObject@@QEAA_NPEB_WAEAVFOutputDevice@@PEAV1@_N@Z': Signature not found\n")));
                                else
                                    CallFunctionByNameWithArguments.assign_address(self.get_match_address());
                                return true;
                        },
                    [](SignatureContainer & self) {},
                };
            SignatureContainer process_exec_old{
                        {{"40 55 53 56 57 41 54 41 55 41 56 41 57 48 81 EC 38 03 00 00"}},
                            [&](const SignatureContainer & self) {
                                if (!self.get_match_address())
                                    Output::send(std::format(STR("Failed to resolve symbol '?CallFunctionByNameWithArguments@UObject@@QEAA_NPEB_WAEAVFOutputDevice@@PEAV1@_N@Z': Signature not found\n")));
                                else
                                    CallFunctionByNameWithArguments.assign_address(self.get_match_address());
                                return true;
                        },
                    [](SignatureContainer & self) {},
                };

            std::vector<SignatureContainer> signature_containers;
            if (Version::IsAtLeast(4, 25))
            {
                signature_containers.push_back(process_exec);
            }
            else
            {
                signature_containers.push_back(process_exec_old);
            }
      
            SinglePassScanner::SignatureContainerMap signature_containers_map;

            signature_containers_map.emplace(ScanTarget::MainExe, signature_containers);

            SinglePassScanner::start_scan(signature_containers_map);

            if (Version::IsAtLeast(4, 25))
            {
                register_symbols_new();
            }
            else
            {
                register_symbols_old();
            }
            
            const auto module_directory = std::filesystem::path(UE4SSProgram::get_program().get_module_directory());
            const auto plugin_directory = module_directory.parent_path().parent_path().parent_path() / "Plugins" / "ExamplePlugin" / "Binaries" / "Win64";

            if (!exists(plugin_directory)) return;
            
            SetDllDirectory(plugin_directory.c_str());

            if (Version::IsAtLeast(4, 25))
            {
                const auto dll_path = plugin_directory.wstring() + STR("\\RED-ExamplePlugin-Win64-Shipping.dll");

                FModuleManager::Get().AddModule(FName(STR("ExamplePlugin"), FNAME_Add), FString(dll_path.c_str()));
            }
            else
            {
                const auto dll_path = plugin_directory.wstring() + STR("\\TresGame-ExamplePlugin-Win64-Shipping.dll");

                FModuleManager::Get().AddModule(FName(STR("ExamplePlugin"), FNAME_Add), FString(dll_path.c_str()));
            }
            FModuleManager::Get().LoadModule(FName(STR("ExamplePlugin"), FNAME_Add));
        }
    };//class
}

/**
* export the start_mod() and uninstall_mod() functions to
* be used by the core ue4ss system to load in our dll mod
*/
#define EXAMPLEPLUGIN_API __declspec(dllexport) 
extern "C" {
    EXAMPLEPLUGIN_API RC::CppUserModBase* start_mod(){ return new ExamplePlugin::ExamplePluginMod(); }
    EXAMPLEPLUGIN_API void uninstall_mod(RC::CppUserModBase* mod) { delete mod; }
}