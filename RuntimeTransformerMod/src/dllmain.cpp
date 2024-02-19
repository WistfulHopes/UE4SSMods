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
#include "Property/FSoftObjectProperty.hpp"

namespace RuntimeTransformer
{
    using namespace RC;
    using namespace Unreal;
    
    /**
    * RuntimeTransformerPlugin: UE4SS c++ mod class defintion
    */
    class RuntimeTransformerPlugin : public CppUserModBase
    {
    public:
        
        // constructor
        RuntimeTransformerPlugin() {
            ModVersion = STR("0.1");
            ModName = STR("RuntimeTransformerPlugin");
            ModAuthors = STR("UE4SS");
            ModDescription = STR("A basic template C++ mod");
            // Do not change this unless you want to target a UE4SS version
            // other than the one you're currently building with somehow.
            //ModIntendedSDKVersion = STR("2.6");
        }
        
        // destructor
        ~RuntimeTransformerPlugin() override {
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
        static inline FVector OneVector = FVector(1, 1, 1);
        static inline FVector ForwardVector = FVector(1, 0, 0);
        static inline FVector RightVector = FVector(0, 1, 0);
        static inline FVector UpVector = FVector(0, 0, 1);

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
        static inline Function<void(AActor*, void*, bool)> AActor_Destroy;

        struct FAttachmentTransformRules
        {
            uint8 LocationRule;
            uint8 RotationRule;
            uint8 ScaleRule;
            bool bWeldSimulatedBodies;
        };
        
        static inline FAttachmentTransformRules SnapToTargetIncludingScale {
            2,
            2,
            2,
            false
        };

        struct FCollisionResponseContainer
        {
            uint8 EnumArray[32];
        };
        
        struct FCollisionResponseParams
        {
            FCollisionResponseContainer CollisionResponse;
        };
        
        static inline FCollisionResponseParams DefaultResponseParam {};
        
        static bool ProcessConsoleExec(UObject* pThis, TCHAR* Cmd, FOutputDevice* Ar, UObject* Executor)
        {
            return CallFunctionByNameWithArguments(pThis, Cmd, Ar, Executor, false);
        }

        static FSoftObjectPath* ClassPathOperatorEqual(FSoftObjectPath* pThis, FSoftObjectPath* that)
        {
            pThis->AssetPathName = that->AssetPathName;
            pThis->SubPathString = that->SubPathString;
            return pThis;
        }
        
        static FSoftObjectPath* ClassPathConstructor(FSoftObjectPath* pThis)
        {
            pThis->AssetPathName = FName();
            pThis->SubPathString = FString();
            return pThis;
        }

        static UObject* APawn_getUObject(AActor* pThis)
        {
            return (UObject*)((char *)pThis - 544);
        }

        static double APawn_TakePointDamage(AActor* pThis, double Damage)
        {
            return Damage;
        }

        static UObject* AActor_GetNetOwner(AActor* pThis)
        {
            return (UObject*)((char *)pThis + 0xE0);
        }

        static bool AActor_IsRelevancyOwnerFor(AActor* pThis, const AActor* ReplicatedActor,
                                                   const AActor* ActorOwner, const AActor* ConnectionActor)
        {
            return pThis == ActorOwner;
        }

        static void AActor_LifeSpanExpired(AActor* pThis)
        {
            AActor_Destroy(pThis, nullptr, true);
        }

        static int64_t AActor_GetLocalRole(AActor* pThis)
        {
            return *(int64_t*)((char *)pThis + 0xF0);
        }
        
        static auto register_symbols() -> void
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
            FModuleManager::RegisterImport("?IsSafeForRootSet@UObject@@UEBA_NXZ", "40 53 48 83 EC 20 48 8B D9 E8 ?? ?? ?? ?? 84 C0 75 ?? 8B 43 0C");
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
            SignatureContainer gengine {
                {{"48 8B 05 ? ? ? ? 48 85 C0 74 ? 8B 80 B0 07 00 00"}},
                    [&](const SignatureContainer & self) {
                        if (!self.get_match_address())
                            Output::send(std::format(STR("Failed to resolve symbol '?GEngine@@3PEAVUEngine@@EA': Signature not found\n")));
                        else
                        {
                            const intptr_t address = reinterpret_cast<intptr_t>(self.get_match_address() + 0x3);
                            FModuleManager::RegisterImport("?GEngine@@3PEAVUEngine@@EA", (void*)(*reinterpret_cast<int*>(address) + address + 0x4));
                        }
                        return true;
                },
                [](SignatureContainer & self) {},
            };
            SignatureContainer world_post_actor_tick {
                {{"48 8D 0D ? ? ? ? 45 8B C7 48 8B D7 E8 ? ? ? ? 48 83 BF F8 01 00 00 00"}},
                    [&](const SignatureContainer & self) {
                        if (!self.get_match_address())
                            Output::send(std::format(STR("Failed to resolve symbol '?OnWorldPostActorTick@FWorldDelegates@@2V?$TMulticastDelegate@XPEAVUWorld@@W4ELevelTick@@M@@A': Signature not found\n")));
                        else
                        {
                            const intptr_t address = reinterpret_cast<intptr_t>(self.get_match_address() + 0x3);
                            FModuleManager::RegisterImport("?OnWorldPostActorTick@FWorldDelegates@@2V?$TMulticastDelegate@XPEAVUWorld@@W4ELevelTick@@M@@A", (void*)(*reinterpret_cast<int*>(address) + address + 0x4));
                        }
                        return true;
                },
                [](SignatureContainer & self) {},
            };
            SignatureContainer world_tick_start {
                {{"48 8D 0D ? ? ? ? 45 8B C6 48 8B D7"}},
                    [&](const SignatureContainer & self) {
                        if (!self.get_match_address())
                            Output::send(std::format(STR("Failed to resolve symbol '?OnWorldTickStart@FWorldDelegates@@2V?$TMulticastDelegate@XPEAVUWorld@@W4ELevelTick@@M@@A': Signature not found\n")));
                        else
                        {
                            const intptr_t address = reinterpret_cast<intptr_t>(self.get_match_address() + 0x3);
                            FModuleManager::RegisterImport("?OnWorldTickStart@FWorldDelegates@@2V?$TMulticastDelegate@XPEAVUWorld@@W4ELevelTick@@M@@A", (void*)(*reinterpret_cast<int*>(address) + address + 0x4));
                        }
                        return true;
                },
                [](SignatureContainer & self) {},
            };
            SignatureContainer begin_play {
                {{"33 F6 84 C0 75 ? F6 47 5B 80"}},
                    [&](const SignatureContainer & self) {
                        FModuleManager::RegisterImport("?BeginPlay@AActor@@MEAAXXZ", self.get_match_address() - 0x48);
                        return true;
                },
                [](SignatureContainer & self)
                {
                    if (!self.get_match_address())
                        Output::send(std::format(STR("Failed to resolve symbol '?BeginPlay@AActor@@MEAAXXZ': Signature not found\n")));
                },
            };
            SignatureContainer destroy_actor {
                {{"75 ? 8B 41 0C 3B 05 ? ? ? ? 7D ? 99 0F B7 D2 03 C2 8B C8 0F B7 C0 2B C2 48 98 C1 F9 10 48 63 C9 48 8D 14 40 48 8B 05 ? ? ? ? 48 8B 0C C8 48 8D 04 D1 EB ? 48 8B C7"}},
                    [&](const SignatureContainer & self) {
                        FModuleManager::RegisterImport("?Destroy@AActor@@QEAA_N_N0@Z", self.get_match_address() - 0x24);
                        return true;
                },
                [](SignatureContainer & self)
                {
                    if (!self.get_match_address())
                        Output::send(std::format(STR("Failed to resolve symbol '?Destroy@AActor@@QEAA_N_N0@Z': Signature not found\n")));
                },
            };

            SinglePassScanner::SignatureContainerMap signature_containers = {
                {ScanTarget::MainExe, {process_event, gengine, world_post_actor_tick, world_tick_start, begin_play, destroy_actor}},
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
            FModuleManager::RegisterImport("?GUObjectArray@@3VFUObjectArray@@A", Container::GetUObjectArray());
            FModuleManager::RegisterImport("?ConstructUEnum@UE4CodeGen_Private@@YAXAEAPEAVUEnum@@AEBUFEnumParams@1@@Z", "40 55 56 41 54 41 55 41 56 41 57 48 8D AC 24 D8 FC FF FF");
            FModuleManager::RegisterImport("?@UE4CodeGen_Private@@YAXAEAPEAVUScriptStruct@@AEBUFStructParams@1@@Z", "40 53 56 57 41 55 41 56 41 57 48 81 EC 08 03 00 00");
            FModuleManager::RegisterImport("?UObjectCompiledInDeferStruct@@YAXP6APEAVUScriptStruct@@XZPEB_W1_N1@Z", "48 89 6C 24 20 48 89 4C 24 08 57 41 56 41 57 48 83 EC 40 41 0F B6 E9 4D 8B F8 4C 8B F2 48 8B F9 45 84 C9 75 ?? 48 89 5C 24 68 48 89 74 24 70 48 89 4C 24 ?? 48 89 54 24 ?? E8 ?? ?? ?? ?? 48 8B D8 48 63 70 08 44 8D 46 01 44 89 40 08 44 3B 40 0C 7E ?? 8B D6 48 8B C8 E8 ?? ?? ?? ?? 48 8B 03 48 8B CE 0F 10 44 24 ?? 48 8B 74 24 70 48 03 C9 48 8B 5C 24 68 0F 11 04 C8 EB ?? 48 8B 94 24 80 00 00 00 48 8D 4C 24 30 41 B8 01 00 00 00 E8 ?? ?? ?? ?? E8 ?? ?? ?? ?? 4C 8D 44 24 60 48 8B C8 48 8D 54 24 30 E8 ?? ?? ?? ?? 48 8B 7C 24 60 45 33 C9 40 88 6C 24 28 49 8B D7 48 89 7C 24 20 49 8B CE 45 8D 41 02");
            FModuleManager::RegisterImport("?GetStaticStruct@@YAPEAVUScriptStruct@@P6APEAV1@XZPEAVUObject@@PEB_W_KI@Z", "48 89 5C 24 10 48 89 6C 24 18 48 89 74 24 20 57 48 83 EC 40 48 8B 42 20 49 8B E8 48 8B DA 48 8B F9 48 85 C0 74 ? 66 66 0F 1F 84 00 00 00 00 00 48 8B D0 48 8B 40 20 48 85 C0 75 ? 48 8B 42 18 48 8D 4C 24 50 48 8D 54 24 30 48 89 44 24 ? E8 ? ? ? ? 83 7C 24 ? ? 48 8D 35 ? ? ? ? 41 B9 01 00 00 00 C6 44 24 28 00 48 8B CE 48 C7 44 24 20 00 00 00 00 48 0F 45 4C 24 ? 48 8B D5 45 8D 41 01");
            FModuleManager::RegisterImport("?UObjectCompiledInDeferEnum@@YAXP6APEAVUEnum@@XZPEB_W1_N1@Z", "48 89 6C 24 20 48 89 4C 24 08 57 41 56 41 57 48 83 EC 40 41 0F B6 E9 4D 8B F8 4C 8B F2 48 8B F9 45 84 C9 75 ? 48 89 5C 24 68 48 89 74 24 70 48 89 4C 24 ? 48 89 54 24 ? E8 ? ? ? ? 48 8B D8 48 63 70 08 44 8D 46 01 44 89 40 08 44 3B 40 0C 7E ? 8B D6 48 8B C8 E8 ? ? ? ? 48 8B 03 48 8B CE 0F 10 44 24 ? 48 8B 74 24 70 48 03 C9 48 8B 5C 24 68 0F 11 04 C8 EB ? 48 8B 94 24 80 00 00 00 48 8D 4C 24 30 41 B8 01 00 00 00 E8 ? ? ? ? E8 ? ? ? ? 4C 8D 44 24 60 48 8B C8 48 8D 54 24 30 E8 ? ? ? ? 48 8B 7C 24 60 45 33 C9 40 88 6C 24 28 49 8B D7 48 89 7C 24 20 49 8B CE 45 8D 41 03");
            FModuleManager::RegisterImport("?GetStaticEnum@@YAPEAVUEnum@@P6APEAV1@XZPEAVUObject@@PEB_W@Z", "48 89 5C 24 10 48 89 6C 24 18 48 89 74 24 20 57 48 83 EC 40 48 8B 42 20 49 8B E8 48 8B DA 48 8B F9 48 85 C0 74 ? 66 66 0F 1F 84 00 00 00 00 00 48 8B D0 48 8B 40 20 48 85 C0 75 ? 48 8B 42 18 48 8D 4C 24 50 48 8D 54 24 30 48 89 44 24 ? E8 ? ? ? ? 83 7C 24 ? ? 48 8D 35 ? ? ? ? 41 B9 01 00 00 00 C6 44 24 28 00 48 8B CE 48 C7 44 24 20 00 00 00 00 48 0F 45 4C 24 ? 48 8B D5 45 8D 41 02");
            FModuleManager::RegisterImport("??0ICppStructOps@UScriptStruct@@QEAA@HH@Z", VoidSub);
            FModuleManager::RegisterImport("??1ICppStructOps@UScriptStruct@@UEAA@XZ", VoidSub);
            FModuleManager::RegisterImport("?DeferCppStructOps@UScriptStruct@@SAXVFName@@PEAUICppStructOps@1@@Z", "48 89 5C 24 20 48 89 54 24 10 48 89 4C 24 ? 57");
            FModuleManager::RegisterImport("?Z_Construct_UScriptStruct_FSoftClassPath@@YAPEAVUScriptStruct@@XZ", "48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 8B 02");
            FModuleManager::RegisterImport("??4FWeakObjectPtr@@QEAAXPEBVUObject@@@Z", "40 53 48 83 EC 20 48 8B D9 48 85 D2 74 ? 8B 52 0C");
            FModuleManager::RegisterImport("??0UObject@@QEAA@AEBVFObjectInitializer@@@Z", "48 89 5C 24 08 57 48 83 EC 20 48 8D 05 ? ? ? ? 48 8B FA 48 89 01 48 8B D9 E8 ? ? ? ?");
            FModuleManager::RegisterImport("?IsTemplate@UObjectBaseUtility@@QEBA_NW4EObjectFlags@@@Z", "48 85 C9 74 ? 85 51 08");
            FModuleManager::RegisterImport("??1UObjectBase@@UEAA@XZ", "40 57 48 83 EC 30 48 8D 05 ? ? ? ?");
            FModuleManager::RegisterImport("?Get@FObjectInitializer@@SAAEAV1@XZ", "40 53 48 83 EC 30 48 8D 44 24 40 C6 44 24 40 00 48 89 44 24 28 48 8D 15 ? ? ? ? 48 8D 05 ? ? ? ? 48 8D 4C 24 20");
            FModuleManager::RegisterImport("?AssertIfInConstructor@FObjectInitializer@@SAXPEAVUObject@@PEB_W@Z", "48 89 5C 24 08 57 48 83 EC 30 48 8D 44 24 50");
            FModuleManager::RegisterImport("?StaticConstructObject_Internal@@YAPEAVUObject@@PEBVUClass@@PEAV1@VFName@@W4EObjectFlags@@W4EInternalObjectFlags@@1_NPEAUFObjectInstancingGraph@@5@Z", static_cast<UObject*(*)(const FStaticConstructObjectParameters& Params)>(UObjectGlobals::StaticConstructObject));
            FModuleManager::RegisterImport("?StaticLoadClass@@YAPEAVUClass@@PEAV1@PEAVUObject@@PEB_W2IPEAVUPackageMap@@@Z", "40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 38 FF FF FF 48 81 EC C8 01 00 00 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 B0 00 00 00 8B BD 30 01 00 00");
            FModuleManager::RegisterImport("?GetObjectsOfClass@@YAXPEBVUClass@@AEAV?$TArray@PEAVUObject@@V?$TSizedDefaultAllocator@$0CA@@@@@_NW4EObjectFlags@@W4EInternalObjectFlags@@@Z", "48 83 EC 48 48 8D 44 24 58 48 89 54 24 58");
            FModuleManager::RegisterImport("??4FSoftClassPath@@QEAAAEAU0@AEBU0@@Z", ClassPathOperatorEqual);
            FModuleManager::RegisterImport("??1FSoftClassPath@@QEAA@XZ", VoidSub);
            FModuleManager::RegisterImport("??0FSoftClassPath@@QEAA@XZ", ClassPathConstructor);
            FModuleManager::RegisterImport("??8FSoftObjectPath@@QEBA_NAEBU0@@Z", "48 83 EC 28 48 8B 02 48 39 01 75 ? 83 7A 10 00");
            FModuleManager::RegisterImport("?ToString@FSoftObjectPath@@QEBA?AVFString@@XZ", "40 53 56 57 48 81 EC 70 08 00 00");
            FModuleManager::RegisterImport("?GetDefaultObject@UClass@@QEBAPEAVUObject@@_N@Z", "40 53 48 83 EC 20 48 8B 81 18 01 00 00 48 8B D9 48 85 C0");
            FModuleManager::RegisterImport("?IsValid@FWeakObjectPtr@@QEBA_NXZ", "44 8B 41 04 45 85 C0 74 ? 8B 01 85 C0 78 ? 3B 05 ? ? ? ? 7D ? 99 0F B7 D2 03 C2 8B C8 0F B7 C0 2B C2 C1 F9 10 48 98 48 63 C9 48 8D 14 40 48 8B 05 ? ? ? ? 48 8B 0C C8 48 8D 04 D1 48 85 C0 74 ? 44 39 40 10 75 ? F7 40 08 00 00 00 30 75 ? B0 01");
            FModuleManager::RegisterImport("?Get@FWeakObjectPtr@@QEBAPEAVUObject@@XZ", "44 8B 41 04 45 85 C0 74 ? 8B 01 85 C0 78 ? 3B 05 ? ? ? ? 7D ? 99 0F B7 D2 03 C2 8B C8 0F B7 C0 2B C2 C1 F9 10 48 98 48 63 C9 48 8D 14 40 48 8B 05 ? ? ? ? 48 8B 0C C8 48 8D 04 D1 48 85 C0 74 ? 44 39 40 10 75 ? F7 40 08 00 00 00 30 75 ? 48 8B 00");
            FModuleManager::RegisterImport("?Get@FWeakObjectPtr@@QEBAPEAVUObject@@_N@Z", "44 8B 49 04 44 0F B6 D2 45 85 C9 74 ? 8B 01 85 C0 78 ? 3B 05 ? ? ? ? 7D ? 99 0F B7 D2 03 C2 8B C8 0F B7 C0 2B C2 C1 F9 10 48 98 48 63 C9 48 8D 14 40 48 8B 05 ? ? ? ? 48 8B 0C C8 4C 8D 04 D1");
            FModuleManager::RegisterImport("?Z_Construct_UScriptStruct_FRuntimeFloatCurve@@YAPEAVUScriptStruct@@XZ", "48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 40 53 48 83 EC 20 48 8B 42 20 45 33 C9 48 85 C0 49 8B D8 41 0F 95 C1 4C 03 C8 4C 89 4A 20 E8 ? ? ? ? 48 89 03 48 83 C4 20 5B C3 CC CC CC CC 48 89 5C 24 08");
            FModuleManager::RegisterImport("?GetDesktopResolution@UGameUserSettings@@QEBA?AUFIntPoint@@XZ", "48 89 5C 24 08 57 48 81 EC 80 00 00 00 48 8B 0D ? ? ? ?");
            FModuleManager::RegisterImport("?GetPrivateStaticClass@UInputSettings@@CAPEAVUClass@@XZ", "4C 8B DC 48 81 EC 88 00 00 00 48 8B 05 ? ? ? ? 48 85 C0 0F 85 ? ? ? ? 33 C9 48 8D 05 ? ? ? ? 49 89 4B F0 4C 8D 0D ? ? ? ? 88 4C 24 70 4C 8D 05 ? ? ? ? 49 89 43 E0 48 8D 15 ? ? ? ? 49 89 43 D8 48 8D 05 ? ? ? ? 49 89 43 D0 48 8D 05 ? ? ? ? 49 89 43 C8 48 8D 05 ? ? ? ? 49 89 43 C0 48 8D 05 ? ? ? ? 49 89 43 B8 49 89 4B B0 48 8D 0D ? ? ? ? C7 44 24 30 06 00 00 10 C7 44 24 28 08 00 00 00 C7 44 24 20 F0 00 00 00 E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 81 C4 88 00 00 00 C3 CC CC CC CC CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 4C 8B DC");
            FModuleManager::RegisterImport("?GetPrivateStaticClass@UPlayerInput@@CAPEAVUClass@@XZ", "4C 8B DC 48 81 EC 88 00 00 00 48 8B 05 ? ? ? ? 48 85 C0 0F 85 ? ? ? ? 33 C9 48 8D 05 ? ? ? ? 49 89 4B F0 4C 8D 0D ? ? ? ? 88 4C 24 70 4C 8D 05 ? ? ? ? 49 89 43 E0 48 8D 15 ? ? ? ? 48 8D 05 ? ? ? ? 49 89 43 D8 48 8D 05 ? ? ? ? 49 89 43 D0 48 8D 05 ? ? ? ? 49 89 43 C8 48 8D 05 ? ? ? ? 49 89 43 C0 48 8D 05 ? ? ? ? 49 89 43 B8 49 89 4B B0 48 8D 0D ? ? ? ? C7 44 24 30 0C 00 00 10 C7 44 24 28 08 00 00 00 C7 44 24 20 A8 03 00 00");
            FModuleManager::RegisterImport("??4FRuntimeFloatCurve@@QEAAAEAU0@AEBU0@@Z", "48 89 5C 24 18 48 89 74 24 20 57 48 83 EC 20 8B 42 68");
            FModuleManager::RegisterImport("??1FRuntimeFloatCurve@@QEAA@XZ", VoidSub);
            FModuleManager::RegisterImport("?GetRichCurveConst@FRuntimeFloatCurve@@QEBAPEBUFRichCurve@@XZ", "48 8B 81 80 00 00 00 48 85 C0 74 ? 48 83 C0 30");
            FModuleManager::RegisterImport("?GetRichCurve@FRuntimeFloatCurve@@QEAAPEAUFRichCurve@@XZ", "48 8B 81 80 00 00 00 48 85 C0 74 ? 48 83 C0 30");
            FModuleManager::RegisterImport("??0FRuntimeFloatCurve@@QEAA@XZ", "45 33 C0 48 8D 51 18 48 8D 05 ? ? ? ? 48 89 01 4C 89 41 08 4C 89 41 10 4C 89 42 10 44 89 42 18 C7 42 1C 80 00 00 00 48 8B 42 10 48 85 C0 48 0F 45 D0 48 8D 05 ? ? ? ? 4C 89 02 4C 89 42 08 C7 41 38 FF FF FF FF 44 89 41 3C 4C 89 41 48 44 89 41 50 4C 89 41 58 4C 89 41 60 C7 41 68 FF FF 7F 7F");
            FModuleManager::RegisterImport("?CreateTransient@UTexture2D@@SAPEAV1@HHW4EPixelFormat@@VFName@@@Z", "4C 8B DC 53 55 56 41 55");
            FModuleManager::RegisterImport("?ConstructUFunction@UE4CodeGen_Private@@YAXAEAPEAVUFunction@@AEBUFFunctionParams@1@@Z", "40 55 53 56 41 54 41 55 41 56 48 8D AC 24 08 F9 FF FF");
            FModuleManager::RegisterImport("?GetNativeInterfaceAddress@UObjectBaseUtility@@QEAAPEAXPEAVUClass@@@Z", "48 89 5C 24 08 48 89 7C 24 10 4C 8B 49 10");
            FModuleManager::RegisterImport("?RegisterFunctions@FNativeFunctionRegistrar@@SAXPEAVUClass@@PEBUFNameNativePtrPair@@H@Z", "45 85 C0 0F 84 ? ? ? ? 4C 8B DC 56");
            FModuleManager::RegisterImport("?RPC_ValidateFailed@@YAXPEB_W@Z", "48 89 0D ? ? ? ? C3 CC CC CC CC CC CC CC CC 48 89 4C 24 08");
            FModuleManager::RegisterImport("?Step@FFrame@@QEAAXPEAVUObject@@QEAX@Z", "48 8B 41 20 4C 8B D2 48 8B D1");
            FModuleManager::RegisterImport("?StepExplicitProperty@FFrame@@QEAAXQEAXPEAVFProperty@@@Z", "41 8B 40 40 4D 8B C8");
            FModuleManager::RegisterImport("?StaticClass@UInterface@@SAPEAVUClass@@XZ", "4C 8B DC 48 81 EC 88 00 00 00 48 8B 05 ? ? ? ? 48 85 C0 0F 85 ? ? ? ? 33 C9 48 8D 05 ? ? ? ? 49 89 4B F0 4C 8D 0D ? ? ? ? 88 4C 24 70 4C 8D 05 ? ? ? ? 49 89 43 E0 48 8D 15 ? ? ? ? 49 89 43 D8 48 8D 05 ? ? ? ? 49 89 43 D0 48 8D 05 ? ? ? ? 49 89 43 C8 48 8D 05 ? ? ? ? 49 89 43 C0 48 8D 05 ? ? ? ? 49 89 43 B8 49 89 4B B0 48 8D 0D ? ? ? ? C7 44 24 30 01 40 00 10");
            FModuleManager::RegisterImport("??0UInterface@@QEAA@AEAVFVTableHelper@@@Z", "40 53 48 83 EC 20 48 8D 05 ?? ?? ?? ?? 48 8B D9 48 89 01 48 8D 0D ?? ?? ?? ??");
            FModuleManager::RegisterImport("?Z_Construct_UClass_UInterface@@YAPEAVUClass@@XZ", "48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 89 5C 24 20 E8 ? ? ? ? E8 ? ? ? ? B2 01 48 8B C8 48 8B D8 E8 ? ? ? ? 48 8D 05 ? ? ? ? 33 D2 48 8B CB 48 89 83 C0 00 00 00 E8 ? ? ? ? 48 8B C3 48 89 1D ? ? ? ? 48 8B 5C 24 20 48 83 C4 28 C3 CC CC CC CC CC CC CC CC F2 0F 10 0D ? ? ? ?");
            FModuleManager::RegisterImport("?Z_Construct_UScriptStruct_FTransform@@YAPEAVUScriptStruct@@XZ", "48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 8B 02");
            FModuleManager::RegisterImport("?Z_Construct_UScriptStruct_FVector@@YAPEAVUScriptStruct@@XZ", "48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 8B 01 48 85 C0 74 ? 48 8B D1 48 8B C8 E9 ? ? ? ? C3 CC CC CC CC CC CC CC CC CC CC CC CC 48 89 5C 24 08");
            FModuleManager::RegisterImport("?Z_Construct_UClass_UClass@@YAPEAVUClass@@XZ", "48 81 EC B8 00 00 00 48 8B 05 ? ? ? ? 48 85 C0 0F 85 ? ? ? ? 48 89 9C 24 B0 00 00 00 E8 ? ? ? ? E8 ? ? ? ? B2 01 48 8B C8 48 8B D8 E8 ? ? ? ? 48 8D 05 ? ? ? ? 41 B8 01 00 00 00 48 8D 15 ? ? ? ? 48 89 83 C0 00 00 00 48 8D 8C 24 C0 00 00 00 E8 ? ? ? ? 41 B9 01 00 00 00 4C 8D 84 24 C0 00 00 00 BA 18 01 00 00");
            FModuleManager::RegisterImport("??0UInterface@@QEAA@AEBVFObjectInitializer@@@Z", "48 89 5C 24 08 57 48 83 EC 20 48 8D 05 ? ? ? ? 48 8B FA 48 89 01 48 8B D9 E8 ? ? ? ?");
            FModuleManager::RegisterImport("?ImplementsInterface@UClass@@QEBA_NPEBV1@@Z", "48 89 5C 24 08 57 48 83 EC 20 48 8B FA 48 8B D9 48 85 D2 74 ? 8B 82 CC 00 00 00");
            FModuleManager::RegisterImport("?IsChildOf@UStruct@@QEBA_NPEBV1@@Z", "48 85 D2 74 ? 48 63 42 38 4C 8D 42 30");
            FModuleManager::RegisterImport("?StaticDuplicateObject@@YAPEAVUObject@@PEBV1@PEAV1@VFName@@W4EObjectFlags@@PEAVUClass@@W4Type@EDuplicateMode@@W4EInternalObjectFlags@@@Z", "40 55 53 56 57 41 54 41 56 41 57 48 8D 6C 24 F1");
            FModuleManager::RegisterImport("?GetPrivateStaticClass@UBoxComponent@@CAPEAVUClass@@XZ", "4C 8B DC 48 81 EC 88 00 00 00 48 8B 05 ? ? ? ? 48 85 C0 0F 85 ? ? ? ? 33 C9 48 8D 05 ? ? ? ? 49 89 4B F0 4C 8D 0D ? ? ? ? 88 4C 24 70 4C 8D 05 ? ? ? ? 49 89 43 E0 48 8D 15 ? ? ? ? 48 8D 05 ? ? ? ? 49 89 43 D8 48 8D 05 ? ? ? ? 49 89 43 D0 48 8D 05 ? ? ? ? 49 89 43 C8 48 8D 05 ? ? ? ? 49 89 43 C0 48 8D 05 ? ? ? ? 49 89 43 B8 49 89 4B B0 48 8D 0D ? ? ? ? C7 44 24 30 04 00 00 10 C7 44 24 28 10 00 00 00 C7 44 24 20 30 04 00 00 E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 81 C4 88 00 00 00 C3 CC CC CC CC CC CC CC CC CC CC CC CC CC CC B8 EA FA 9B 23");
            FModuleManager::RegisterImport("?GetPrivateStaticClass@USphereComponent@@CAPEAVUClass@@XZ", "4C 8B DC 48 81 EC 88 00 00 00 48 8B 05 ? ? ? ? 48 85 C0 0F 85 ? ? ? ? 33 C9 48 8D 05 ? ? ? ? 49 89 4B F0 4C 8D 0D ? ? ? ? 88 4C 24 70 4C 8D 05 ? ? ? ? 49 89 43 E0 48 8D 15 ? ? ? ? 48 8D 05 ? ? ? ? 49 89 43 D8 48 8D 05 ? ? ? ? 49 89 43 D0 48 8D 05 ? ? ? ? 49 89 43 C8 48 8D 05 ? ? ? ? 49 89 43 C0 48 8D 05 ? ? ? ? 49 89 43 B8 49 89 4B B0 48 8D 0D ? ? ? ? C7 44 24 30 04 00 00 10 C7 44 24 28 10 00 00 00 C7 44 24 20 30 04 00 00 E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 81 C4 88 00 00 00 C3 CC CC CC CC CC CC CC CC CC CC CC CC CC CC 4C 8B DC 48 81 EC 88 00 00 00 48 8B 05 ? ? ? ? 48 85 C0 0F 85 ? ? ? ? 33 C9 48 8D 05 ? ? ? ? 49 89 4B F0 4C 8D 0D ? ? ? ? 88 4C 24 70 4C 8D 05 ? ? ? ? 49 89 43 E0 48 8D 15 ? ? ? ? 48 8D 05 ? ? ? ? 49 89 43 D8 48 8D 05 ? ? ? ? 49 89 43 D0 48 8D 05 ? ? ? ? 49 89 43 C8 48 8D 05 ? ? ? ? 49 89 43 C0 48 8D 05 ? ? ? ? 49 89 43 B8 49 89 4B B0 48 8D 0D ? ? ? ? C7 44 24 30 04 00 00 10");
            FModuleManager::RegisterImport("?GetPlayerController@UGameplayStatics@@SAPEAVAPlayerController@@PEBVUObject@@H@Z", "48 89 5C 24 08 57 48 83 EC 30 8B FA 41 B8 01 00 00 00 48 8B D1 48 8B 0D ? ? ? ? E8 ? ? ? ? 48 85 C0 74 ? 33 DB");
            FModuleManager::RegisterImport("?DisplayDebug@AActor@@UEAAXPEAVUCanvas@@AEBVFDebugDisplayInfo@@AEAM2@Z", "48 8B C4 55 53 56 57 41 54 41 55 41 56 41 57 48 8D A8 28 FE FF FF");
            FModuleManager::RegisterImport("?GetActorEyesViewPoint@AActor@@UEBAXAEAUFVector@@AEAUFRotator@@@Z", "48 89 5C 24 08 57 48 83 EC 40 48 8B 81 30 01 00 00");
            FModuleManager::RegisterImport("?Reset@AActor@@UEAAXXZ", "48 89 5C 24 08 57 48 83 EC 20 48 8B 15 ? ? ? ? 48 8B F9 48 8B 19 E8 ? ? ? ? 48 8B D0 45 33 C0 48 8B CF 48 8B 83 10 02 00 00 48 8B 5C 24 30 48 83 C4 20 5F 48 FF E0 CC CC CC CC CC CC CC C6 81 A5 00 00 00 01");
            FModuleManager::RegisterImport("?GetHumanReadableName@AActor@@UEBA?AVFString@@XZ", "40 53 48 83 EC 20 48 8B 41 18 48 8B DA");
            FModuleManager::RegisterImport("?BecomeViewTarget@AActor@@UEAAXPEAVAPlayerController@@@Z", "48 89 5C 24 10 57 48 83 EC 20 48 8B 19 48 8B F9 48 89 54 24 30 48 8B 15 ? ? ? ? E8 ? ? ? ? 48 8B D0 4C 8D 44 24 30 48 8B CF FF 93 10 02 00 00 48 8B 5C 24 38 48 83 C4 20 5F C3 CC CC CC 48 89 5C 24 10 57 48 83 EC 20 48 8B 19 48 8B F9 48 89 54 24 30 48 8B 15 ? ? ? ? E8 ? ? ? ? 48 8B D0 4C 8D 44 24 30 48 8B CF FF 93 10 02 00 00 48 8B 5C 24 38 48 83 C4 20 5F C3 CC CC CC 48 89 5C 24 08 57 48 83 EC 20");
            FModuleManager::RegisterImport("?TakeDamage@AActor@@UEAAMMAEBUFDamageEvent@@PEAVAController@@PEAV1@@Z", "48 8B C4 55 53 56 57 41 55 41 56 41 57 48 8D A8 18 FE FF FF");
            FModuleManager::RegisterImport("?CanBeBaseForCharacter@AActor@@UEBA_NPEAVAPawn@@@Z", "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 48 8B 99 30 01 00 00 48 8B F2");
            FModuleManager::RegisterImport("?OutsideWorldBounds@AActor@@UEAAXXZ", "40 53 48 83 EC 20 8B 81 30 02 00 00 48 8B D9 A8 20");
            FModuleManager::RegisterImport("?Destroyed@AActor@@UEAAXXZ", "40 53 48 83 EC 30 33 D2 48 8B D9 E8 ? ? ? ? 48 8B CB");
            FModuleManager::RegisterImport("?IsBasedOnActor@AActor@@UEBA_NPEBV1@@Z", "48 8B 01 48 FF A0 10 05 00 00");
            FModuleManager::RegisterImport("?TeleportSucceeded@AActor@@UEAAX_N@Z", "48 83 EC 28 84 D2 75 ? 48 8B 01");
            FModuleManager::RegisterImport("?PostRegisterAllComponents@AActor@@UEAAXXZ", VoidSub);
            FModuleManager::RegisterImport("?GetNetConnection@AActor@@UEBAPEAVUNetConnection@@XZ", "48 8B 89 E0 00 00 00 48 85 C9 74 ? 48 8B 01 48 FF A0 A0 04 00 00");
            FModuleManager::RegisterImport("?GetNetOwningPlayer@AActor@@UEAAPEAVUPlayer@@XZ", "80 B9 F0 00 00 00 03 75 ? 48 8B 89 E0 00 00 00");
            FModuleManager::RegisterImport("?GetNetOwner@AActor@@UEBAPEBV1@XZ", AActor_GetNetOwner);
            FModuleManager::RegisterImport("?PostInitializeComponents@AActor@@UEAAXXZ", "8B 41 0C 4C 8B C9 3B 05 ? ? ? ?");
            FModuleManager::RegisterImport("?PreInitializeComponents@AActor@@UEAAXXZ", "40 53 48 83 EC 20 0F B6 81 F3 00 00 00");
            FModuleManager::RegisterImport("?IsNetRelevantFor@AActor@@UEBA_NPEBV1@0AEBUFVector@@@Z", TrueSub);
            FModuleManager::RegisterImport("?ShouldTickIfViewportsOnly@AActor@@UEBA_NXZ", NullSub);
            FModuleManager::RegisterImport("?PostNetReceiveVelocity@AActor@@UEAAXAEBUFVector@@@Z", VoidSub);
            FModuleManager::RegisterImport("?PostNetReceiveLocationAndRotation@AActor@@UEAAXXZ", "48 89 5C 24 08 48 89 7C 24 10 55 48 8B EC 48 83 EC 70 48 8B F9");
            FModuleManager::RegisterImport("?EndPlay@AActor@@MEAAXW4Type@EEndPlayReason@@@Z", "4C 8B DC 55 56 48 81 EC 28 01 00 00");
            FModuleManager::RegisterImport("?GetVelocity@AActor@@UEBA?AUFVector@@XZ", "40 53 48 83 EC 20 48 8B 89 30 01 00 00 48 8B DA 48 85 C9 74 ? 48 8B 01 FF 90 40 04 00 00");
            FModuleManager::RegisterImport("?DisableInput@AActor@@UEAAXPEAVAPlayerController@@@Z", "40 53 48 83 EC 30 4C 8B C2 48 8B D9 48 8B 91 F8 00 00 00");
            FModuleManager::RegisterImport("?EnableInput@AActor@@UEAAXPEAVAPlayerController@@@Z", "48 85 D2 0F 84 ? ? ? ? 48 89 5C 24 18 57 48 83 EC 50 48 8B DA");
            FModuleManager::RegisterImport("?PreReplication@AActor@@UEAAXAEAVIRepChangedPropertyTracker@@@Z", "40 53 56 57 48 83 EC 30 48 8B 01 33 FF");
            FModuleManager::RegisterImport("?ValidateGeneratedRepEnums@AActor@@UEBAXAEBV?$TArray@UFRepRecord@@V?$TSizedDefaultAllocator@$0CA@@@@@@Z", "48 89 5C 24 08 57 48 83 EC 20 65 48 8B 04 25 58 00 00 00 48 8B FA 8B 0D ? ? ? ? BA 14 00 00 00 48 8B 1C C8 48 03 DA 8B 03 39 05 ? ? ? ? 0F 8F ? ? ? ? 8B 03 39 05 ? ? ? ? 0F 8F ? ? ? ? 8B 03 39 05 ? ? ? ? 0F 8F ? ? ? ? 8B 03 39 05 ? ? ? ? 0F 8F ? ? ? ? 8B 03 39 05 ? ? ? ? 0F 8F ? ? ? ? 8B 03 39 05 ? ? ? ? 0F 8F ? ? ? ? 8B 03 39 05 ? ? ? ? 0F 8F ? ? ? ? 8B 03 39 05 ? ? ? ? 0F 8F ? ? ? ? 8B 03 39 05 ? ? ? ? 0F 8F ? ? ? ? 8B 03 39 05 ? ? ? ? 0F 8F ? ? ? ? 48 8B 0F 33 D2 48 8B 01");
            FModuleManager::RegisterImport("?GetLifetimeReplicatedProps@AActor@@UEBAXAEAV?$TArray@VFLifetimeProperty@@V?$TSizedDefaultAllocator@$0CA@@@@@@Z", "48 89 5C 24 08 48 89 7C 24 10 55 48 8B EC 48 83 EC 60 48 8B 79 10");
            FModuleManager::RegisterImport("?PostLoad@AActor@@UEAAXXZ", "40 53 48 83 EC 20 48 89 7C 24 38 48 8B D9 E8 ? ? ? ? 48 8B BB E0 00 00 00");
            FModuleManager::RegisterImport("?GetMoveGoalReachTest@APawn@@UEBAXPEBVAActor@@AEBUFVector@@AEAU3@AEAM3@Z", VoidSub);
            FModuleManager::RegisterImport("?GetNavAgentLocation@APawn@@UEBA?AUFVector@@XZ", "48 83 EC 18 48 8B 81 08 FF FF FF 48 85 C0 74 ? 0F 10 88 E0 01 00 00 0F 28 C1 F3 0F 11 0C 24 0F C6 C1 55 0F C6 C9 AA F3 0F 11 4C 24 08 F3 0F 11 44 24 04 EB ? F2 0F 10 05 ? ? ? ? 8B 05 ? ? ? ? F2 0F 11 04 24 89 44 24 08 48 8D 04 24 F2 0F 10 08");
            FModuleManager::RegisterImport("?GetNavAgentPropertiesRef@APawn@@UEBAAEBUFNavAgentProperties@@XZ", "48 83 EC 28 48 81 C1 D8 FD FF FF");
            FModuleManager::RegisterImport("?_getUObject@APawn@@UEBAPEAVUObject@@XZ", APawn_getUObject);
            FModuleManager::RegisterImport("?IsMoveInputIgnored@APawn@@UEBA_NXZ", "48 83 EC 28 48 8B 89 60 02 00 00 48 85 C9 74 ? 48 8B 01 FF 90 20 07 00 00");
            FModuleManager::RegisterImport("?AddControllerRollInput@APawn@@UEAAXM@Z", "40 53 48 83 EC 30 0F 29 74 24 20 0F 57 C0 0F 28 F1 48 8B D9 0F 2E F0 74 ? 48 8B 89 60 02 00 00 48 85 C9 74 ? F6 81 9C 02 00 00 02 74 ? 48 8B 01 FF 90 A8 06 00 00 84 C0 74 ? 48 8B 8B 60 02 00 00 0F 28 CE 48 8B 01 FF 90 98 0B 00 00");
            FModuleManager::RegisterImport("?AddControllerYawInput@APawn@@UEAAXM@Z", "40 53 48 83 EC 30 0F 29 74 24 20 0F 57 C0 0F 28 F1 48 8B D9 0F 2E F0 74 ? 48 8B 89 60 02 00 00 48 85 C9 74 ? F6 81 9C 02 00 00 02 74 ? 48 8B 01 FF 90 A8 06 00 00 84 C0 74 ? 48 8B 8B 60 02 00 00 0F 28 CE 48 8B 01 FF 90 90 0B 00 00");
            FModuleManager::RegisterImport("?AddControllerPitchInput@APawn@@UEAAXM@Z", "40 53 48 83 EC 30 0F 29 74 24 20 0F 57 C0 0F 28 F1 48 8B D9 0F 2E F0 74 ? 48 8B 89 60 02 00 00 48 85 C9 74 ? F6 81 9C 02 00 00 02 74 ? 48 8B 01 FF 90 A8 06 00 00 84 C0 74 ? 48 8B 8B 60 02 00 00 0F 28 CE 48 8B 01 FF 90 88 0B 00 00");
            FModuleManager::RegisterImport("?ConsumeMovementInputVector@APawn@@UEAA?AUFVector@@XZ", "48 89 5C 24 08 57 48 83 EC 20 48 8B 01 48 8B FA 48 8B D9 FF 90 28 06 00 00");
            FModuleManager::RegisterImport("?AddMovementInput@APawn@@UEAAXUFVector@@M_N@Z", "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 70 48 8B 01 41 0F B6 F1");
            FModuleManager::RegisterImport("?SetupPlayerInputComponent@APawn@@MEAAXPEAVUInputComponent@@@Z", VoidSub);
            FModuleManager::RegisterImport("?DestroyPlayerInputComponent@APawn@@MEAAXXZ", "40 53 48 83 EC 20 48 8B D9 48 8B 89 F8 00 00 00");
            FModuleManager::RegisterImport("?CreatePlayerInputComponent@APawn@@MEAAPEAVUInputComponent@@XZ", "48 89 5C 24 08 57 48 83 EC 50 65 48 8B 04 25 58 00 00 00 48 8B F9 8B 15 ? ? ? ?");
            FModuleManager::RegisterImport("?GetDamageInstigator@APawn@@MEBAPEAVAController@@PEAV2@AEBVUDamageType@@@Z", "48 85 D2 74 ? 48 3B 91 60 02 00 00");
            FModuleManager::RegisterImport("?SpawnDefaultController@APawn@@UEAAXXZ", "48 89 5C 24 10 55 48 8D 6C 24 A9 48 81 EC A0 00 00 00 48 83 B9 60 02 00 00 00");
            FModuleManager::RegisterImport("?DetachFromControllerPendingDestroy@APawn@@UEAAXXZ", "40 53 48 83 EC 20 48 8B D9 48 8B 89 60 02 00 00 48 85 C9 74 ? 48 39 99 58 02 00 00");
            FModuleManager::RegisterImport("?FaceRotation@APawn@@UEAAXUFRotator@@M@Z", "48 89 5C 24 10 56 48 83 EC 40 44 8B 81 30 02 00 00");
            FModuleManager::RegisterImport("?PawnClientRestart@APawn@@UEAAXXZ", "48 89 5C 24 08 57 48 83 EC 20 48 8B 01 48 8B D9 FF 90 48 06 00 00");
            FModuleManager::RegisterImport("?InFreeCam@APawn@@UEBA_NXZ", "48 89 5C 24 08 57 48 83 EC 20 48 8B 99 60 02 00 00 48 85 DB");
            FModuleManager::RegisterImport("?GetBaseAimRotation@APawn@@UEBA?AUFRotator@@XZ", "48 89 5C 24 10 57 48 83 EC 50 48 83 B9 60 02 00 00 00");
            FModuleManager::RegisterImport("?GetPawnViewLocation@APawn@@UEBA?AUFVector@@XZ", "48 83 EC 18 48 8B 81 30 01 00 00 F3 0F 10 91 34 02 00 00");
            FModuleManager::RegisterImport("?GetViewRotation@APawn@@UEBA?AUFRotator@@XZ", "48 89 74 24 10 57 48 83 EC 40 48 8B F9 48 8B F2 48 8B 89 60 02 00 00");
            FModuleManager::RegisterImport("?IsBotControlled@APawn@@UEBA_NXZ", "48 8B 81 48 02 00 00 48 85 C0 74 ? F6 80 32 02 00 00 08 74 ?");
            FModuleManager::RegisterImport("?IsPlayerControlled@APawn@@UEBA_NXZ", "48 8B 81 48 02 00 00 48 85 C0 74 ? F6 80 32 02 00 00 08 75 ?");
            FModuleManager::RegisterImport("?PossessedBy@APawn@@UEAAXPEAVAController@@@Z", "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 48 83 EC 20 48 8B 01 48 8B F2 48 8B D9");
            FModuleManager::RegisterImport("?RecalculateBaseEyeHeight@APawn@@UEAAXXZ", "48 89 5C 24 08 57 48 83 EC 20 48 8B 79 10 48 8B D9 48 8B 87 18 01 00 00");
            FModuleManager::RegisterImport("?SetPlayerDefaults@APawn@@UEAAXXZ", VoidSub);
            FModuleManager::RegisterImport("?ShouldTakeDamage@APawn@@UEBA_NMAEBUFDamageEvent@@PEAVAController@@PEAVAActor@@@Z", "48 83 EC 38 80 B9 F0 00 00 00 03");
            FModuleManager::RegisterImport("?UpdateNavigationRelevance@APawn@@UEAAXXZ", VoidSub);
            FModuleManager::RegisterImport("?OnRep_PlayerState@APawn@@UEAAXXZ", "48 8B 81 48 02 00 00 48 85 C0 74 ? 48 39 88 88 02 00 00");
            FModuleManager::RegisterImport("?OnRep_Controller@APawn@@UEAAXXZ", "40 57 48 83 EC 20 48 8B F9 48 8B 89 60 02 00 00");
            FModuleManager::RegisterImport("?GetDefaultHalfHeight@APawn@@UEBAMXZ", "48 89 5C 24 18 57 48 83 EC 20 48 8B 59 10 48 8B F9");
            FModuleManager::RegisterImport("?ReachedDesiredRotation@APawn@@UEAA_NXZ", "48 89 5C 24 08 48 89 7C 24 10 55 48 8B EC 48 81 EC 80 00 00 00 0F 29 74 24 70");
            FModuleManager::RegisterImport("?GetPawnPhysicsVolume@APawn@@UEBAPEAVAPhysicsVolume@@XZ", "40 53 48 83 EC 20 48 8B 01 48 8B D9 FF 90 28 06 00 00 48 85 C0");
            FModuleManager::RegisterImport("?UnPossessed@APawn@@UEAAXXZ", "48 89 5C 24 08 57 48 83 EC 30 48 8B 01 48 8B D9 48 8B B9 60 02 00 00");
            FModuleManager::RegisterImport("?PawnStartFire@APawn@@UEAAXE@Z", VoidSub);
            FModuleManager::RegisterImport("?Restart@APawn@@UEAAXXZ", "40 53 48 83 EC 30 48 8B 01 48 8B D9 FF 90 28 06 00 00");
            FModuleManager::RegisterImport("?TurnOff@APawn@@UEAAXXZ", "48 89 5C 24 08 57 48 83 EC 20 80 B9 F0 00 00 00 03 48 8B D9");
            FModuleManager::RegisterImport("?GetPawnNoiseEmitterComponent@APawn@@UEBAPEAVUPawnNoiseEmitterComponent@@XZ", "40 53 48 83 EC 20 48 8B D9 E8 ? ? ? ? 4C 8B 03 48 8B D0 48 8B CB 41 FF 90 10 06 00 00 48 85 C0 75 ?");
            FModuleManager::RegisterImport("?GetMovementBase@APawn@@UEBAPEAVUPrimitiveComponent@@XZ", "48 83 EC 28 48 85 C9 74 ? 48 8B 01 FF 90 30 06 00 00");
            FModuleManager::RegisterImport("?DeprojectMousePositionToWorld@APlayerController@@QEBA_NAEAUFVector@@0@Z", "48 89 5C 24 10 48 89 6C 24 18 48 89 74 24 20 57 48 83 EC 20 48 8B 99 A0 02 00 00");
            FModuleManager::RegisterImport("?DisplayDebug@APawn@@UEAAXPEAVUCanvas@@AEBVFDebugDisplayInfo@@AEAM2@Z", "48 89 5C 24 18 55 56 57 41 54 41 55 41 56 41 57 48 8B EC 48 83 EC 40 4C 8B 65 60");
            FModuleManager::RegisterImport("?IsComponentRelevantForNavigation@AActor@@UEBA_NPEAVUActorComponent@@@Z", TrueSub);
            FModuleManager::RegisterImport("?FindComponentByClass@AActor@@UEBAPEAVUActorComponent@@V?$TSubclassOf@VUActorComponent@@@@@Z", "40 55 53 57 48 8D 6C 24 B9 48 81 EC 90 00 00 00 48 8B DA 48 8B F9 48 85 D2");
            FModuleManager::RegisterImport("?PostRenderFor@AActor@@UEAAXPEAVAPlayerController@@PEAVUCanvas@@UFVector@@2@Z", VoidSub);
            FModuleManager::RegisterImport("?GetTargetLocation@AActor@@UEBA?AUFVector@@PEAV1@@Z", "48 83 EC 18 48 8B 81 30 01 00 00 48 85 C0 74 ? 0F 10 88 E0 01 00 00 0F 28 C1 F3 0F 11 0C 24 0F C6 C1 55 0F C6 C9 AA F3 0F 11 4C 24 08 F3 0F 11 44 24 04 EB ? F2 0F 10 05 ? ? ? ? 8B 05 ? ? ? ? F2 0F 11 04 24 89 44 24 08 48 8D 04 24 F2 0F 10 00 8B 40 08 F2 0F 11 02");
            FModuleManager::RegisterImport("?GetActorEyesViewPoint@APawn@@UEBAXAEAUFVector@@AEAUFRotator@@@Z", "48 89 5C 24 08 57 48 83 EC 40 48 8B 81 30 01 00 00");
            FModuleManager::RegisterImport("?PrestreamTextures@AActor@@UEAAXM_NH@Z", "48 8B C4 48 89 58 08 48 89 70 10 48 89 78 18 4C 89 60 20 55 41 56 41 57 48 8D 68 A1 48 81 EC A0 00 00 00 0F 29 70 D8 0F 57 F6");
            FModuleManager::RegisterImport("?ForceNetUpdate@AActor@@UEAAXXZ", "40 57 48 83 EC 20 80 B9 F0 00 00 00 03");
            FModuleManager::RegisterImport("?ForceNetRelevant@AActor@@UEAAXXZ", "40 53 48 83 EC 20 48 8B 01 48 8B D9 FF 90 D0 00 00 00 84 C0 74 ?");
            FModuleManager::RegisterImport("?GetLastRenderTime@AActor@@UEBAMXZ", "F3 0F 10 81 14 01 00 00 C3");
            FModuleManager::RegisterImport("?Reset@APawn@@UEAAXXZ", "40 53 48 83 EC 20 48 8B 81 60 02 00 00");
            FModuleManager::RegisterImport("?GetHumanReadableName@APawn@@UEBA?AVFString@@XZ", "48 89 5C 24 10 48 89 74 24 18 57 48 83 EC 40 48 8B 81 48 02 00 00");
            FModuleManager::RegisterImport("?HasActivePawnControlCameraComponent@AActor@@UEBA_NXZ", "48 8B C4 48 89 58 10 48 89 70 18 55 57 41 56 48 8D 68 A1 48 81 EC 90 00 00 00 F6 41 5A 04 0F 84 ? ? ? ? 4C 8D B1 A0 01 00 00 4C 89 78 08 45 8B 56 28 49 8D 76 10 33 C9 C7 45 DB 01 00 00 00 89 4D D7 44 8B C1 48 89 75 DF C7 45 E7 FF FF FF FF 48 89 4D EB 8D 59 1F 45 85 D2 74 ? 48 8B 46 10 4C 8B CE 48 85 C0 4C 0F 45 C8 41 8D 42 FF 99 23 D3 03 C2 41 8B 11 C1 F8 05 85 D2 75 ? 66 90 48 63 D1 41 83 C0 20 FF C1 44 89 45 EF 89 4D D7 3B D0 7D ? 41 8B 54 91 04 C7 45 E7 FF FF FF FF 85 D2 74 ? 8D 42 FF 23 C2 33 C2 0F BD C8 89 45 DB 74 ? 8B C3 2B C1 EB ? B8 20 00 00 00 44 2B C0 41 8D 40 1F 89 45 EB 41 3B C2 7E ? 44 89 55 EB 41 8B 56 28 41 BF FF FF FF FF 0F 10 55 E7 8B C2 4C 89 75 1F 0F 10 45 D7 23 C3 89 55 EB 0F 11 55 37 0F B6 C8 44 8B C2 0F 11 45 27 41 D3 E7 44 8B CA 0F 10 45 1F 41 C1 F8 05 41 83 E1 E0 66 0F 15 D2 F2 0F 11 55 17 44 89 7D E7 0F 10 4D 2F 0F 11 45 ? 0F 11 4D ? 41 3B D2 74 ? 48 8B 46 10 4C 8B DE 48 85 C0 49 63 C8 4C 0F 45 D8 41 8D 42 FF 99 23 D3 03 C2 41 8B 14 8B C1 F8 05 8B F8 41 23 D7 75 ? 0F 1F 40 00 0F 1F 84 00 00 00 00 00 49 63 C8 41 83 C1 20 41 FF C0 3B CF 7D ? 41 8B 54 8B 04 48 8B C1 C7 45 E7 FF FF FF FF 85 D2 74 ? 8D 42 FF 23 C2 33 C2 0F BD C8 74 ? 2B D9 EB ? BB 20 00 00 00 44 2B CB 41 8D 41 1F 89 45 EB 41 3B C2 7E ? 44 89 55 EB 48 8B 7D E7 4C 8B BC 24 B0 00 00 00 48 C1 EF 20 0F 1F 80 00 00 00 00 48 63 55 ? 3B D7 75 ? 48 39 75 ? 75 ? B0 01 EB ? 32 C0 48 8B 4D ? 84 C0 74 ? 49 3B CE 75 ? 32 C0 EB ? B0 01 84 C0 74 ? 48 8B 09 48 8B C2 48 03 C0 48 8B 1C C1 48 85 DB 74 ? E8 ? ? ? ? 48 8B 53 10 4C 8D 40 30 48 63 40 38 3B 42 38 7F ? 48 8B C8 48 8B 42 30 4C 39 04 C8 75 ? F6 83 8A 00 00 00 01 74 ?");
            FModuleManager::RegisterImport("?HasActiveCameraComponent@AActor@@UEBA_NXZ", "48 8B C4 48 89 58 10 48 89 70 18 55 57 41 56 48 8D 68 A1 48 81 EC 90 00 00 00 F6 41 5A 04 0F 84 ? ? ? ? 4C 8D B1 A0 01 00 00 4C 89 78 08 45 8B 56 28 49 8D 76 10 33 C9 C7 45 DB 01 00 00 00 89 4D D7 44 8B C1 48 89 75 DF C7 45 E7 FF FF FF FF 48 89 4D EB 8D 59 1F 45 85 D2 74 ? 48 8B 46 10 4C 8B CE 48 85 C0 4C 0F 45 C8 41 8D 42 FF 99 23 D3 03 C2 41 8B 11 C1 F8 05 85 D2 75 ? 66 90 48 63 D1 41 83 C0 20 FF C1 44 89 45 EF 89 4D D7 3B D0 7D ? 41 8B 54 91 04 C7 45 E7 FF FF FF FF 85 D2 74 ? 8D 42 FF 23 C2 33 C2 0F BD C8 89 45 DB 74 ? 8B C3 2B C1 EB ? B8 20 00 00 00 44 2B C0 41 8D 40 1F 89 45 EB 41 3B C2 7E ? 44 89 55 EB 41 8B 56 28 41 BF FF FF FF FF 0F 10 55 E7 8B C2 4C 89 75 1F 0F 10 45 D7 23 C3 89 55 EB 0F 11 55 37 0F B6 C8 44 8B C2 0F 11 45 27 41 D3 E7 44 8B CA 0F 10 45 1F 41 C1 F8 05 41 83 E1 E0 66 0F 15 D2 F2 0F 11 55 17 44 89 7D E7 0F 10 4D 2F 0F 11 45 ? 0F 11 4D ? 41 3B D2 74 ? 48 8B 46 10 4C 8B DE 48 85 C0 49 63 C8 4C 0F 45 D8 41 8D 42 FF 99 23 D3 03 C2 41 8B 14 8B C1 F8 05 8B F8 41 23 D7 75 ? 0F 1F 40 00 0F 1F 84 00 00 00 00 00 49 63 C8 41 83 C1 20 41 FF C0 3B CF 7D ? 41 8B 54 8B 04 48 8B C1 C7 45 E7 FF FF FF FF 85 D2 74 ? 8D 42 FF 23 C2 33 C2 0F BD C8 74 ? 2B D9 EB ? BB 20 00 00 00 44 2B CB 41 8D 41 1F 89 45 EB 41 3B C2 7E ? 44 89 55 EB 48 8B 7D E7 4C 8B BC 24 B0 00 00 00 48 C1 EF 20 0F 1F 80 00 00 00 00 48 63 55 ? 3B D7 75 ? 48 39 75 ? 75 ? B0 01 EB ? 32 C0 48 8B 4D ? 84 C0 74 ? 49 3B CE 75 ? 32 C0 EB ? B0 01 84 C0 74 ? 48 8B 09 48 8B C2 48 03 C0 48 8B 1C C1 48 85 DB 74 ? E8 ? ? ? ? 48 8B 53 10 4C 8D 40 30 48 63 40 38 3B 42 38 7F ? 48 8B C8 48 8B 42 30 4C 39 04 C8 75 ? F6 83 8A 00 00 00 01 75 ?");
            FModuleManager::RegisterImport("?CalcCamera@AActor@@UEAAXMAEAUFMinimalViewInfo@@@Z", "4C 8B DC 55 56 48 81 EC 18 01 00 00");
            FModuleManager::RegisterImport("?EndViewTarget@AActor@@UEAAXPEAVAPlayerController@@@Z", "48 89 5C 24 10 57 48 83 EC 20 48 8B 19 48 8B F9 48 89 54 24 30 48 8B 15 ? ? ? ? E8 ? ? ? ? 48 8B D0 4C 8D 44 24 30 48 8B CF FF 93 10 02 00 00 48 8B 5C 24 38 48 83 C4 20 5F C3 CC CC CC 48 89 5C 24 08 57 48 83 EC 20 48 8B 15 ? ? ? ? 48 8B F9 48 8B 19 E8 ? ? ? ? 48 8B D0 45 33 C0 48 8B CF 48 8B 83 10 02 00 00 48 8B 5C 24 30 48 83 C4 20 5F 48 FF E0 CC CC CC CC CC CC CC C6 81 A5 00 00 00 01");
            FModuleManager::RegisterImport("?BecomeViewTarget@APawn@@UEAAXPEAVAPlayerController@@@Z", "48 89 5C 24 08 57 48 83 EC 20 48 8B FA 48 8B D9 E8 ? ? ? ? 48 8B CB E8 ? ? ? ? 83 F8 03");
            FModuleManager::RegisterImport("?InternalTakePointDamage@AActor@@MEAAMMAEBUFPointDamageEvent@@PEAVAController@@PEAV1@@Z", APawn_TakePointDamage);
            FModuleManager::RegisterImport("?InternalTakeRadialDamage@AActor@@MEAAMMAEBUFRadialDamageEvent@@PEAVAController@@PEAV1@@Z", "40 53 48 83 EC 50 4D 63 48 38");
            FModuleManager::RegisterImport("?TakeDamage@APawn@@UEAAMMAEBUFDamageEvent@@PEAVAController@@PEAVAActor@@@Z", "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 48 83 EC 40 48 8B 01 49 8B D9");
            FModuleManager::RegisterImport("?CanBeBaseForCharacter@APawn@@UEBA_NPEAV1@@Z", "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 48 8B 99 30 01 00 00 48 8B F2");
            FModuleManager::RegisterImport("?GetComponentsCollisionResponseToChannel@AActor@@UEBA?AW4ECollisionResponse@@W4ECollisionChannel@@@Z", "48 89 5C 24 08 48 89 74 24 10 48 89 7C 24 18 4C 89 64 24 20 55 41 56 41 57 48 8D 6C 24 B9 48 81 EC 90 00 00 00 33 FF C7 45 DB 01 00 00 00");
            FModuleManager::RegisterImport("?TornOff@AActor@@UEAAXXZ", VoidSub);
            FModuleManager::RegisterImport("?IsRootComponentCollisionRegistered@AActor@@UEBA_NXZ", "48 83 EC 28 48 8B 89 30 01 00 00 48 85 C9 74 ? F6 81 88 00 00 00 01");
            FModuleManager::RegisterImport("?GetSimpleCollisionCylinder@AActor@@UEBAXAEAM0@Z", "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 30 48 8B 01 49 8B F8 48 8B F2 48 8B D9 FF 90 68 05 00 00");
            FModuleManager::RegisterImport("?GetComponentsBoundingCylinder@AActor@@UEBAXAEAM0_N1@Z", "48 89 5C 24 08 44 88 4C 24 20 55 56 57 48 8B EC");
            FModuleManager::RegisterImport("?CalculateComponentsBoundingBoxInLocalSpace@AActor@@UEBA?AUFBox@@_N0@Z", "4C 8B DC 45 88 43 18 55 53");
            FModuleManager::RegisterImport("?GetComponentsBoundingBox@AActor@@UEBA?AUFBox@@_N0@Z", "48 89 5C 24 08 48 89 6C 24 10 44 88 44 24 18");
            FModuleManager::RegisterImport("?OutsideWorldBounds@APawn@@UEAAXXZ", "40 53 48 83 EC 20 8B 81 30 02 00 00 48 8B D9 A8 20");
            FModuleManager::RegisterImport("?FellOutOfWorld@AActor@@UEAAXAEBVUDamageType@@@Z", "40 53 48 83 EC 20 0F B6 81 F0 00 00 00");
            FModuleManager::RegisterImport("?Destroyed@APawn@@UEAAXXZ", "40 53 48 83 EC 20 48 8B 01 48 8B D9 FF 90 F8 06 00 00");
            FModuleManager::RegisterImport("?RegisterActorTickFunctions@AActor@@MEAAX_N@Z", "40 57 48 83 EC 30 48 8B F9 84 D2 74 ?");
            FModuleManager::RegisterImport("?OnConstruction@AActor@@UEAAXAEBUFTransform@@@Z", VoidSub);
            FModuleManager::RegisterImport("?RerunConstructionScripts@AActor@@UEAAXXZ", "40 55 56 48 8D AC 24 48 FF FF FF");
            FModuleManager::RegisterImport("?IsAttachedTo@AActor@@UEBA_NPEBV1@@Z", "48 8B 89 30 01 00 00 48 85 C9 74 ? 48 85 D2 74 ? 48 8B 92 30 01 00 00");
            FModuleManager::RegisterImport("?IsBasedOnActor@APawn@@UEBA_NPEBVAActor@@@Z", "48 89 5C 24 08 57 48 83 EC 20 48 8B 01 48 8B DA 48 8B F9 FF 90 30 06 00 00");
            FModuleManager::RegisterImport("?ClearCrossLevelReferences@AActor@@UEAAXXZ", "40 57 48 83 EC 20 48 8B 81 30 01 00 00");
            FModuleManager::RegisterImport("?TeleportSucceeded@APawn@@UEAAX_N@Z", "48 83 EC 28 84 D2 75 ? 48 8B 01");
            FModuleManager::RegisterImport("?TeleportTo@AActor@@UEAA_NAEBUFVector@@AEBUFRotator@@_N2@Z", "48 89 5C 24 10 48 89 74 24 18 4C 89 6C 24 20");
            FModuleManager::RegisterImport("?InvalidateLightingCacheDetailed@AActor@@UEAAX_N@Z", VoidSub);
            FModuleManager::RegisterImport("?MarkComponentsAsPendingKill@AActor@@UEAAXXZ", "4C 8B DC 48 81 EC 18 01 00 00 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 F8 00 00 00");
            FModuleManager::RegisterImport("?ReregisterAllComponents@AActor@@UEAAXXZ", "40 53 48 83 EC 20 48 8B 01 B2 01 48 8B D9 FF 90 C8 04 00 00");
            FModuleManager::RegisterImport("?PostUnregisterAllComponents@AActor@@UEAAXXZ", "48 8B D1 33 C9 39 0D ? ? ? ? 48 0F 45 0D ? ? ? ? 48 8B 01 48 FF 60 48 CC CC CC CC CC CC 48 8D 05 ? ? ? ? C3 CC CC CC CC CC CC CC CC 48 8B C4 53");
            FModuleManager::RegisterImport("?UnregisterAllComponents@AActor@@UEAAX_N@Z", "4C 8B DC 49 89 5B 10 49 89 6B 18 56 57 41 56 48 81 EC 00 01 00 00 48 8B 05 ? ? ? ?");
            FModuleManager::RegisterImport("?PostRegisterAllComponents@APawn@@UEAAXXZ", "48 89 5C 24 08 57 48 83 EC 20 48 8B F9 E8 ? ? ? ? 48 8B 07 48 8B CF FF 90 28 06 00 00");
            FModuleManager::RegisterImport("?PreRegisterAllComponents@AActor@@UEAAXXZ", "48 8B D1 33 C9 39 0D ? ? ? ? 48 0F 45 0D ? ? ? ? 48 8B 01 48 FF 60 48 CC CC CC CC CC CC 48 8D 05 ? ? ? ? C3 CC CC CC CC CC CC CC CC 48 8B D1 33 C9 39 0D ? ? ? ? 48 0F 45 0D ? ? ? ? 48 8B 01 48 FF 60 48 CC CC CC CC CC CC 48 8D 05 ? ? ? ? C3 CC CC CC CC CC CC CC CC 48 8B C4");
            FModuleManager::RegisterImport("?RegisterAllComponents@AActor@@UEAAXXZ", "40 53 48 83 EC 20 48 8B 01 48 8B D9 FF 90 B8 04 00 00");
            FModuleManager::RegisterImport("?DestroyNetworkActorHandled@AActor@@UEAA_NXZ", "40 53 48 83 EC 20 48 8B 99 A0 02 00 00 48 85 DB 74 ? E8 ? ? ? ? 48 8B 53 10");
            FModuleManager::RegisterImport("?GetNetConnection@APawn@@UEBAPEAVUNetConnection@@XZ", "48 8B C1 48 8B 89 60 02 00 00 48 85 C9 74 ?");
            FModuleManager::RegisterImport("?GetNetOwningPlayer@APawn@@UEAAPEAVUPlayer@@XZ", "40 53 48 83 EC 20 80 B9 F0 00 00 00 03 75 ?");
            FModuleManager::RegisterImport("?GetNetOwner@APawn@@UEBAPEBVAActor@@XZ", AActor_GetNetOwner);
            FModuleManager::RegisterImport("?DispatchPhysicsCollisionHit@AActor@@UEAAXAEBUFRigidBodyCollisionInfo@@0AEBUFCollisionImpactData@@@Z", "48 89 5C 24 08 48 89 74 24 10 48 89 7C 24 18 55 41 54 41 55 41 56 41 57 48 8D AC 24 60 FF FF FF 48 81 EC A0 01 00 00 49 8B 19");
            FModuleManager::RegisterImport("?PostInitializeComponents@APawn@@UEAAXXZ", "40 53 48 83 EC 20 48 8B D9 E8 ? ? ? ? 8B 43 0C 3B 05 ? ? ? ? 7D ? 99 0F B7 D2 03 C2 44 8B C0 0F B7 C0 2B C2 48 98 41 C1 F8 10 49 63 C8 48 8D 14 40 48 8B 05 ? ? ? ? 48 8B 0C C8 48 8D 04 D1 EB ? 33 C0 8B 40 08 C1 E8 1D A8 01 0F 85 ? ? ? ?");
            FModuleManager::RegisterImport("?PreInitializeComponents@APawn@@UEAAXXZ", "40 53 48 83 EC 20 48 8B D9 E8 ? ? ? ? 48 8B CB E8 ? ? ? ? 48 85 C0 75 ?");
            FModuleManager::RegisterImport("?IsRelevancyOwnerFor@AActor@@UEBA_NPEBV1@00@Z", AActor_IsRelevancyOwnerFor);
            FModuleManager::RegisterImport("?IsReplayRelevantFor@AActor@@UEBA_NPEBV1@0AEBUFVector@@M@Z", "48 8B 01 48 FF A0 60 04 00 00 CC CC CC CC CC CC 48 83 EC 28");
            FModuleManager::RegisterImport("?IsNetRelevantFor@APawn@@UEBA_NPEBVAActor@@0AEBUFVector@@@Z", "48 89 5C 24 18 57 41 56 41 57 48 83 EC 20 F6 41 58 08");
            FModuleManager::RegisterImport("?ShouldTickIfViewportsOnly@APawn@@UEBA_NXZ", "40 53 48 83 EC 20 48 8B 01 48 8B D9 FF 90 B0 06 00 00");
            FModuleManager::RegisterImport("?CheckStillInWorld@AActor@@UEAA_NXZ", "40 53 48 83 EC 70 8B 41 0C");
            FModuleManager::RegisterImport("?SetOwner@AActor@@UEAAXPEAV1@@Z", "48 89 5C 24 18 57 48 83 EC 20 4C 8B 81 E0 00 00 00");
            FModuleManager::RegisterImport("?PostNetReceivePhysicState@AActor@@UEAAXXZ", "48 89 5C 24 10 57 48 83 EC 70 48 8B B9 30 01 00 00");
            FModuleManager::RegisterImport("?PostNetReceiveVelocity@APawn@@UEAAXAEBUFVector@@@Z", "40 53 48 83 EC 20 80 B9 F0 00 00 00 01");
            FModuleManager::RegisterImport("?PostNetReceiveLocationAndRotation@APawn@@UEAAXXZ", "48 89 5C 24 18 55 48 8D 6C 24 A9 48 81 EC 90 00 00 00 48 8B 81 30 01 00 00");
            FModuleManager::RegisterImport("?OnRep_ReplicatedMovement@AActor@@UEAAXXZ", "40 53 48 83 EC 20 F6 41 58 10 48 8B D9 0F 84 ? ? ? ? 48 89 74 24 30");
            FModuleManager::RegisterImport("?PostNetInit@AActor@@UEAAXXZ", "40 53 48 83 EC 20 F6 41 58 10 48 8B D9 0F 84 ? ? ? ? 48 89 74 24 30");
            FModuleManager::RegisterImport("?PostNetReceiveRole@AActor@@UEAAXXZ", VoidSub);
            FModuleManager::RegisterImport("?LifeSpanExpired@AActor@@UEAAXXZ", AActor_LifeSpanExpired);
            FModuleManager::RegisterImport("?PostActorCreated@AActor@@UEAAXXZ", VoidSub);
            FModuleManager::RegisterImport("?TickActor@AActor@@UEAAXMW4ELevelTick@@AEAUFActorTickFunction@@@Z", "40 53 48 83 EC 30 8B 41 0C");
            FModuleManager::RegisterImport("?OnNetCleanup@AActor@@UEAAXPEAVUNetConnection@@@Z", VoidSub);
            FModuleManager::RegisterImport("?OnSerializeNewActor@AActor@@UEAAXAEAVFOutBunch@@@Z", VoidSub);
            FModuleManager::RegisterImport("?UseShortConnectTimeout@AActor@@UEBA_NXZ", NullSub);
            FModuleManager::RegisterImport("?OnActorChannelOpen@AActor@@UEAAXAEAVFInBunch@@PEAVUNetConnection@@@Z", VoidSub);
            FModuleManager::RegisterImport("?GetNetDormancy@AActor@@UEAA_NAEBUFVector@@0PEAV1@1PEAVUActorChannel@@M_N@Z", NullSub);
            FModuleManager::RegisterImport("?GetReplayPriority@AActor@@UEAAMAEBUFVector@@0PEAV1@1QEAVUActorChannel@@M@Z", "48 8B 44 24 28 48 85 C0 74 ? 48 3B C8 74 ? 48 39 81 18 01 00 00 75 ? F3 0F 10 44 24 38 F3 0F 59 05 ? ? ? ? F3 0F 59 81 10 01 00 00 C3 F6 41 58 20 0F 85 ? ? ? ? 48 8B 81 30 01 00 00 48 85 C0 0F 84 ? ? ? ? 0F 10 88 E0 01 00 00");
            FModuleManager::RegisterImport("?GetNetPriority@AActor@@UEAAMAEBUFVector@@0PEAV1@1PEAVUActorChannel@@M_N@Z", "F6 41 59 02 74 ? 4C 8B 99 E0 00 00 00");
            FModuleManager::RegisterImport("?IsLevelBoundsRelevant@AActor@@UEBA_NXZ", "0F B6 41 59 C0 E8 03");
            FModuleManager::RegisterImport("?ApplyWorldOffset@AActor@@UEAAXAEBUFVector@@_N@Z", "48 89 5C 24 10 48 89 4C 24 08 55 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 D9 48 81 EC 90 00 00 00 4C 8B E9");
            FModuleManager::RegisterImport("?GetDefaultAttachComponent@AActor@@UEBAPEAVUSceneComponent@@XZ", "48 8B 81 30 01 00 00 C3 CC");
            FModuleManager::RegisterImport("?GatherCurrentMovement@AActor@@UEAAXXZ", "48 89 5C 24 20 55 48 8B EC 48 81 EC 80 00 00 00 F6 41 58 10");
            FModuleManager::RegisterImport("?GetLifeSpan@AActor@@UEBAMXZ", "40 57 48 83 EC 20 48 8B 01 48 8B F9 FF 90 50 01 00 00 48 85 C0");
            FModuleManager::RegisterImport("?SetLifeSpan@AActor@@UEAAXM@Z", "40 57 48 81 EC C0 00 00 00 0F 29 B4 24 A0 00 00 00");
            FModuleManager::RegisterImport("?NotifyHit@AActor@@UEAAXPEAVUPrimitiveComponent@@PEAV1@0_NUFVector@@33AEBUFHitResult@@@Z", "4C 8B DC 48 81 EC 88 00 00 00 48 8B 84 24 C8 00 00 00 F2 0F 10 00 8B 40 08 89 44 24 58 48 8B 84 24 C0 00 00 00 F2 0F 11 44 24 50 F2 0F 10 00 8B 40 08 89 44 24 68 48 8B 84 24 B8 00 00 00 F2 0F 11 44 24 60 F2 0F 10 00 8B 40 08 89 44 24 78 48 8B 84 24 D0 00 00 00 49 89 43 B8 49 8D 43 C8 49 89 43 B0 49 8D 43 D8 49 89 43 A8 49 8D 43 E8 49 89 43 A0 0F B6 84 24 B0 00 00 00 88 44 24 20 F2 0F 11 44 24 70 E8 ? ? ? ? 48 81 C4 88 00 00 00 C3 CC CC CC CC CC CC CC CC CC CC CC CC CC CC 40 53");
            FModuleManager::RegisterImport("?NotifyActorOnInputTouchLeave@AActor@@UEAAXW4Type@ETouchIndex@@@Z", "48 89 5C 24 08 57 48 83 EC 20 48 8B 19 48 8B F9 88 54 24 38 48 8B 15 ? ? ? ? E8 ? ? ? ? 48 8B D0 4C 8D 44 24 38 48 8B CF FF 93 10 02 00 00 48 8B 5C 24 30 48 83 C4 20 5F C3 CC CC CC CC 48 89 5C 24 18");
            FModuleManager::RegisterImport("?NotifyActorOnInputTouchEnter@AActor@@UEAAXW4Type@ETouchIndex@@@Z", "48 89 5C 24 08 57 48 83 EC 20 48 8B 19 48 8B F9 88 54 24 38 48 8B 15 ? ? ? ? E8 ? ? ? ? 48 8B D0 4C 8D 44 24 38 48 8B CF FF 93 10 02 00 00 48 8B 5C 24 30 48 83 C4 20 5F C3 CC CC CC CC 48 89 5C 24 08 57 48 83 EC 20 48 8B 19 48 8B F9 88 54 24 38 48 8B 15 ? ? ? ? E8 ? ? ? ? 48 8B D0 4C 8D 44 24 38 48 8B CF FF 93 10 02 00 00 48 8B 5C 24 30 48 83 C4 20 5F C3 CC CC CC CC 48 89 5C 24 18");
            FModuleManager::RegisterImport("?NotifyActorOnInputTouchEnd@AActor@@UEAAXW4Type@ETouchIndex@@@Z", "48 89 5C 24 08 57 48 83 EC 20 48 8B 19 48 8B F9 88 54 24 38 48 8B 15 ? ? ? ? E8 ? ? ? ? 48 8B D0 4C 8D 44 24 38 48 8B CF FF 93 10 02 00 00 48 8B 5C 24 30 48 83 C4 20 5F C3 CC CC CC CC 48 89 5C 24 08 57 48 83 EC 20 48 8B 19 48 8B F9 88 54 24 38 48 8B 15 ? ? ? ? E8 ? ? ? ? 48 8B D0 4C 8D 44 24 38 48 8B CF FF 93 10 02 00 00 48 8B 5C 24 30 48 83 C4 20 5F C3 CC CC CC CC 48 89 5C 24 08 57 48 83 EC 20 48 8B 19 48 8B F9 88 54 24 38 48 8B 15 ? ? ? ? E8 ? ? ? ? 48 8B D0 4C 8D 44 24 38 48 8B CF FF 93 10 02 00 00 48 8B 5C 24 30 48 83 C4 20 5F C3 CC CC CC CC 48 89 5C 24 18");
            FModuleManager::RegisterImport("?NotifyActorOnInputTouchBegin@AActor@@UEAAXW4Type@ETouchIndex@@@Z", "48 8B 19 48 8B F9 88 54 24 38 48 8B 15 ? ? ? ? E8 ? ? ? ? 48 8B D0 4C 8D 44 24 38 48 8B CF FF 93 10 02 00 00 48 8B 5C 24 30 48 83 C4 20 5F C3 CC CC CC CC 48 89 5C 24 08 57 48 83 EC 20 48 8B 19 48 8B F9 88 54 24 38 48 8B 15 ? ? ? ? E8 ? ? ? ? 48 8B D0 4C 8D 44 24 38 48 8B CF FF 93 10 02 00 00 48 8B 5C 24 30 48 83 C4 20 5F C3 CC CC CC CC 48 89 5C 24 08 57 48 83 EC 20 48 8B 19 48 8B F9 88 54 24 38 48 8B 15 ? ? ? ? E8 ? ? ? ? 48 8B D0 4C 8D 44 24 38 48 8B CF FF 93 10 02 00 00 48 8B 5C 24 30 48 83 C4 20 5F C3 CC CC CC CC 48 89 5C 24 08 57 48 83 EC 20 48 8B 19 48 8B F9 88 54 24 38 48 8B 15 ? ? ? ? E8 ? ? ? ? 48 8B D0 4C 8D 44 24 38 48 8B CF FF 93 10 02 00 00 48 8B 5C 24 30 48 83 C4 20 5F C3 CC CC CC CC 48 89 5C 24 18");
            FModuleManager::RegisterImport("?NotifyActorOnReleased@AActor@@UEAAXUFKey@@@Z", "40 53 48 83 EC 40 48 8B 02 48 8B DA 48 89 44 24 20 48 8B 42 08 48 89 44 24 28 48 8B 42 10 48 89 44 24 30 48 85 C0 74 ? FF 40 08 48 8D 54 24 20 E8 ? ? ? ? 48 8B 5B 10 48 85 DB 74 ? 83 6B 08 01 75 ? 48 8B 03 48 8B CB FF 10 83 6B 0C 01 75 ? 48 8B 03 BA 01 00 00 00 48 8B CB 48 83 C4 40 5B 48 FF 60 08 48 83 C4 40 5B C3 CC CC CC CC 4C 8B DC");
            FModuleManager::RegisterImport("?NotifyActorOnClicked@AActor@@UEAAXUFKey@@@Z", "40 53 48 83 EC 40 48 8B 02 48 8B DA 48 89 44 24 20 48 8B 42 08 48 89 44 24 28 48 8B 42 10 48 89 44 24 30 48 85 C0 74 ? FF 40 08 48 8D 54 24 20 E8 ? ? ? ? 48 8B 5B 10 48 85 DB 74 ? 83 6B 08 01 75 ? 48 8B 03 48 8B CB FF 10 83 6B 0C 01 75 ? 48 8B 03 BA 01 00 00 00 48 8B CB 48 83 C4 40 5B 48 FF 60 08 48 83 C4 40 5B C3 CC CC CC CC E9 ? ? ? ?");
            FModuleManager::RegisterImport("?NotifyActorEndCursorOver@AActor@@UEAAXXZ", "E9 ? ? ? ? CC CC CC CC CC CC CC CC CC CC CC E9 ? ? ? ? CC CC CC CC CC CC CC CC CC CC CC 40 53 48 83 EC 40 48 8B 02 48 8B DA 48 89 44 24 20 48 8B 42 08 48 89 44 24 28 48 8B 42 10 48 89 44 24 30 48 85 C0 74 ? FF 40 08 48 8D 54 24 20 E8 ? ? ? ? 48 8B 5B 10 48 85 DB 74 ? 83 6B 08 01 75 ? 48 8B 03 48 8B CB FF 10 83 6B 0C 01 75 ? 48 8B 03 BA 01 00 00 00 48 8B CB 48 83 C4 40 5B 48 FF 60 08 48 83 C4 40 5B C3 CC CC CC CC E9 ? ? ? ?");
            FModuleManager::RegisterImport("?NotifyActorBeginCursorOver@AActor@@UEAAXXZ", "48 89 5C 24 08 57 48 83 EC 20 48 8B 15 ? ? ? ? 48 8B F9 48 8B 19 E8 ? ? ? ? 48 8B D0 45 33 C0 48 8B CF 48 8B 83 10 02 00 00 48 8B 5C 24 30 48 83 C4 20 5F 48 FF E0 CC CC CC CC CC CC CC 48 89 5C 24 10 57 48 83 EC 20 48 8B 19 48 8B F9 48 89 54 24 30 48 8B 15 ? ? ? ? E8 ? ? ? ? 48 8B D0 4C 8D 44 24 30 48 8B CF FF 93 10 02 00 00 48 8B 5C 24 38 48 83 C4 20 5F C3 CC CC CC 48 89 5C 24 08");
            FModuleManager::RegisterImport("?NotifyActorEndOverlap@AActor@@UEAAXPEAV1@@Z", "48 89 5C 24 08 57 48 83 EC 20 48 8B 15 ? ? ? ? 48 8B F9 48 8B 19 E8 ? ? ? ? 48 8B D0 45 33 C0 48 8B CF 48 8B 83 10 02 00 00 48 8B 5C 24 30 48 83 C4 20 5F 48 FF E0 CC CC CC CC CC CC CC 48 89 5C 24 10 57 48 83 EC 20 48 8B 19 48 8B F9 48 89 54 24 30 48 8B 15 ? ? ? ? E8 ? ? ? ? 48 8B D0 4C 8D 44 24 30 48 8B CF FF 93 10 02 00 00 48 8B 5C 24 38 48 83 C4 20 5F C3 CC CC CC 48 89 5C 24 08");
            FModuleManager::RegisterImport("?NotifyActorBeginOverlap@AActor@@UEAAXPEAV1@@Z", "48 89 5C 24 10 57 48 83 EC 20 48 8B 19 48 8B F9 48 89 54 24 30 48 8B 15 ? ? ? ? E8 ? ? ? ? 48 8B D0 4C 8D 44 24 30 48 8B CF FF 93 10 02 00 00 48 8B 5C 24 38 48 83 C4 20 5F C3 CC CC CC 48 89 5C 24 08 57 48 83 EC 20 48 8B 15 ? ? ? ? 48 8B F9 48 8B 19 E8 ? ? ? ? 48 8B D0 45 33 C0 48 8B CF 48 8B 83 10 02 00 00 48 8B 5C 24 30 48 83 C4 20 5F 48 FF E0 CC CC CC CC CC CC CC 48 89 5C 24 10 57 48 83 EC 20 48 8B 19 48 8B F9 48 89 54 24 30 48 8B 15 ? ? ? ? E8 ? ? ? ? 48 8B D0 4C 8D 44 24 30 48 8B CF FF 93 10 02 00 00 48 8B 5C 24 38 48 83 C4 20 5F C3 CC CC CC 48 89 5C 24 18");
            FModuleManager::RegisterImport("?NotifyActorBeginOverlap@AActor@@UEAAXPEAV1@@Z", "E9 ? ? ? ? CC CC CC CC CC CC CC CC CC CC CC 40 53 48 83 EC 40 48 8B 02 48 8B DA 48 89 44 24 20 48 8B 42 08 48 89 44 24 28 48 8B 42 10 48 89 44 24 30 48 85 C0 74 ? FF 40 08 48 8D 54 24 20 E8 ? ? ? ? 48 8B 5B 10 48 85 DB 74 ? 83 6B 08 01 75 ? 48 8B 03 48 8B CB FF 10 83 6B 0C 01 75 ? 48 8B 03 BA 01 00 00 00 48 8B CB 48 83 C4 40 5B 48 FF 60 08 48 83 C4 40 5B C3 CC CC CC CC E9 ? ? ? ?");
            FModuleManager::RegisterImport("?EndPlay@APawn@@UEAAXW4Type@EEndPlayReason@@@Z", "48 89 5C 24 08 57 48 83 EC 20 8B FA 48 8B D9 85 D2 74 ?");
            FModuleManager::RegisterImport("?RemoveTickPrerequisiteComponent@AActor@@UEAAXPEAVUActorComponent@@@Z", "48 85 D2 74 ? 4C 8D 42 30 48 83 C1 28");
            FModuleManager::RegisterImport("?RemoveTickPrerequisiteActor@AActor@@UEAAXPEAV1@@Z", "48 85 D2 74 ? 4C 8D 42 28 48 83 C1 28");
            FModuleManager::RegisterImport("?AddTickPrerequisiteComponent@AActor@@UEAAXPEAVUActorComponent@@@Z", "F6 41 32 02 74 ? 48 85 D2 74 ? F6 42 3A 02");
            FModuleManager::RegisterImport("?AddTickPrerequisiteActor@AActor@@UEAAXPEAV1@@Z", "F6 41 32 02 74 ? 48 85 D2 74 ? F6 42 32 02");
            FModuleManager::RegisterImport("?K2_DestroyActor@AActor@@UEAAXXZ", AActor_LifeSpanExpired);
            FModuleManager::RegisterImport("?SetActorHiddenInGame@AActor@@UEAAX_N@Z", "44 0F B6 41 58 41 0F B6 C0");
            FModuleManager::RegisterImport("?GetVelocity@APawn@@UEBA?AUFVector@@XZ", "48 89 5C 24 10 57 48 83 EC 20 48 8B F9 48 8B DA 48 8B 89 30 01 00 00 48 85 C9 74 ?");
            FModuleManager::RegisterImport("?DisableInput@APawn@@UEAAXPEAVAPlayerController@@@Z", "48 3B 91 60 02 00 00 74 ? 48 85 D2 75 ? 83 A1 30 02 00 00 EF");
            FModuleManager::RegisterImport("?EnableInput@APawn@@UEAAXPEAVAPlayerController@@@Z", "48 3B 91 60 02 00 00 74 ? 48 85 D2 75 ? 83 89 30 02 00 00 10");
            FModuleManager::RegisterImport("?OnRep_Instigator@AActor@@UEAAXXZ", VoidSub);
            FModuleManager::RegisterImport("?RewindForReplay@AActor@@UEAAXXZ", VoidSub);
            FModuleManager::RegisterImport("?PreReplicationForReplay@AActor@@UEAAXAEAVIRepChangedPropertyTracker@@@Z", "48 8B 01 48 FF A0 A0 03 00 00");
            FModuleManager::RegisterImport("?PreReplication@APawn@@UEAAXAEAVIRepChangedPropertyTracker@@@Z", "40 53 48 83 EC 30 48 8B D9 E8 ? ? ? ? 80 BB F0 00 00 00 03");
            FModuleManager::RegisterImport("?OnSubobjectDestroyFromReplication@AActor@@UEAAXPEAVUObject@@@Z", "48 85 D2 74 ? 53 48 83 EC 20 48 8B DA E8 ? ? ? ? 48 8B 53 10 4C 8D 40 30 48 63 40 38 3B 42 38 7F ? 48 8B C8 48 8B 42 30 4C 39 04 C8 75 ? 48 8B CB");
            FModuleManager::RegisterImport("?ReplicateSubobjects@AActor@@UEAA_NPEAVUActorChannel@@PEAVFOutBunch@@PEAUFReplicationFlags@@@Z", "48 89 6C 24 10 48 89 74 24 18 57 41 54 41 55 41 56 41 57 48 83 EC 20 48 63 81 98 01 00 00");
            FModuleManager::RegisterImport("?OnReplicationPausedChanged@AActor@@UEAAX_N@Z", VoidSub);
            FModuleManager::RegisterImport("?IsReplicationPausedForConnection@AActor@@UEAA_NAEBUFNetViewer@@@Z", NullSub);
            FModuleManager::RegisterImport("?OnRep_AttachmentReplication@AActor@@UEAAXXZ", NullSub);
            FModuleManager::RegisterImport("?SetReplicateMovement@AActor@@UEAAX_N@Z", "80 61 58 EF");
            FModuleManager::RegisterImport("?OnRep_Owner@AActor@@MEAAXXZ", VoidSub);
            FModuleManager::RegisterImport("?HasLocalNetOwner@AActor@@UEBA_NXZ", VoidSub);
            FModuleManager::RegisterImport("?HasNetOwner@AActor@@UEBA_NXZ", "48 8B 89 E0 00 00 00 48 85 C9 75 ? 32 C0");
            FModuleManager::RegisterImport("?TearOff@AActor@@UEAAXXZ", "40 57 48 83 EC 20 48 8B F9 E8 ? ? ? ? FF C8");
            FModuleManager::RegisterImport("?OnRep_ReplicateMovement@AActor@@UEAAXXZ", VoidSub);
            FModuleManager::RegisterImport("?ValidateGeneratedRepEnums@APawn@@UEBAXAEBV?$TArray@UFRepRecord@@V?$TSizedDefaultAllocator@$0CA@@@@@@Z", "40 53 48 83 EC 20 65 48 8B 04 25 58 00 00 00 8B 0D ? ? ? ? BA 14 00 00 00 48 8B 1C C8 48 03 DA 8B 03 39 05 ? ? ? ? 7F ? 8B 03 39 05 ? ? ? ? 7F ? 8B 03");
            FModuleManager::RegisterImport("?CheckDefaultSubobjectsInternal@AActor@@UEBA_NXZ", "40 53 48 83 EC 20 48 8B D9 E8 ? ? ? ? 84 C0 74 ? 48 8B CB 48 83 C4 20 5B E9 ? ? ? ? 48 83 C4 20 5B C3 CC CC CC CC CC CC CC CC CC CC CC 4C 8B DC");
            FModuleManager::RegisterImport("?CallRemoteFunction@AActor@@UEAA_NPEAVUFunction@@PEAXPEAUFOutParmRec@@PEAUFFrame@@@Z", "40 55 56 41 54 41 55 41 56 48 83 EC 40 48 8B 01");
            FModuleManager::RegisterImport("?GetFunctionCallspace@AActor@@UEAAHPEAVUFunction@@PEAUFFrame@@@Z", "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 48 83 EC 20 F7 82 B0 00 00 00 00 20 00 00");
            FModuleManager::RegisterImport("?ProcessEvent@AActor@@UEAAXPEAVUFunction@@PEAX@Z", "48 89 5C 24 10 48 89 6C 24 18 57 48 83 EC 20 F7 82 B0 00 00 00 00 04 00 00");
            FModuleManager::RegisterImport("?PreNetReceive@AActor@@UEAAXXZ", "0F B6 41 58 C0 E8 05");
            FModuleManager::RegisterImport("?GetSubobjectsWithStableNamesForNetworking@AActor@@UEAAXAEAV?$TArray@PEAVUObject@@V?$TSizedDefaultAllocator@$0CA@@@@@@Z", "48 89 5C 24 10 48 89 74 24 18 55 57 41 55 41 56 41 57 48 8D 6C 24 C9 48 81 EC 90 00 00 00 4C 8D A9 A0 01 00 00");
            FModuleManager::RegisterImport("?IsNameStableForNetworking@AActor@@UEBA_NXZ", "F6 41 58 02 75 ?");
            FModuleManager::RegisterImport("?Rename@AActor@@UEAA_NPEB_WPEAVUObject@@I@Z", "48 89 5C 24 10 48 89 6C 24 20 56 57 41 56 48 83 EC 20 41 8B F1");
            FModuleManager::RegisterImport("?IsEditorOnly@AActor@@UEBA_NXZ", "0F B6 41 5A C0 E8 06");
            FModuleManager::RegisterImport("?PostRename@AActor@@UEAAXPEAVUObject@@VFName@@@Z", "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 48 8B 01 49 8B D8 48 8B F1");
            FModuleManager::RegisterImport("?IsReadyForFinishDestroy@AActor@@UEAA_NXZ", "48 83 EC 28 48 81 C1 10 02 00 00");
            FModuleManager::RegisterImport("?BeginDestroy@AActor@@UEAAXXZ", "48 89 5C 24 08 57 48 83 EC 20 48 8B D9 E8 ? ? ? ? 48 8B D0 48 8B CB E8 ? ? ? ? 4C 8B 03");
            FModuleManager::RegisterImport("?PostLoadSubobjects@AActor@@UEAAXPEAUFObjectInstancingGraph@@@Z", "40 55 53 56 57 41 55 48 8B EC 48 83 EC 70 48 8B 99 30 01 00 00");
            FModuleManager::RegisterImport("?PostLoad@APawn@@UEAAXXZ", "40 53 48 83 EC 20 48 8B D9 E8 ? ? ? ? C6 83 F3 00 00 00 00");
            FModuleManager::RegisterImport("?PostInitProperties@AActor@@UEAAXXZ", "40 53 48 83 EC 20 48 8B D9 E8 ? ? ? ? 0F B6 43 5B");
            FModuleManager::RegisterImport("?CanBeInCluster@AActor@@UEBA_NXZ", "0F B6 41 5B D0 E8");
            FModuleManager::RegisterImport("?IsLocallyControlled@APawn@@UEBA_NXZ", "48 83 EC 28 48 8B 89 60 02 00 00 48 85 C9 74 ? 48 8B 01 FF 90 A8 06 00 00");
            FModuleManager::RegisterImport("?GetWorld@AActor@@UEBAPEAVUWorld@@XZ", "40 53 48 83 EC 20 8B 41 08 48 8B D9 C1 E8 04 A8 01 75 ? 48 8B 51 20");
            FModuleManager::RegisterImport("?IsSupportedForNetworking@AActor@@UEBA_NXZ", TrueSub);
            FModuleManager::RegisterImport("?SnapToTargetIncludingScale@FAttachmentTransformRules@@2U1@A", &SnapToTargetIncludingScale);
            FModuleManager::RegisterImport("?DefaultResponseParam@FCollisionResponseParams@@2U1@A", &DefaultResponseParam);
            FModuleManager::RegisterImport("?StaticClass@AActor@@SAPEAVUClass@@XZ", "4C 8B DC 48 81 EC 88 00 00 00 48 8B 05 ? ? ? ? 48 85 C0 0F 85 ? ? ? ? 49 89 43 F0 4C 8D 0D ? ? ? ? 88 44 24 70 4C 8D 05 ? ? ? ? 48 8D 05 ? ? ? ? 49 89 43 E0 48 8D 15 ? ? ? ? 49 89 43 D8 48 8D 0D ? ? ? ? 48 8D 05 ? ? ? ? 49 89 43 D0 48 8D 05 ? ? ? ? 49 89 43 C8 48 8D 05 ? ? ? ? 49 89 43 C0 48 8D 05 ? ? ? ? 49 89 43 B8 48 B8 00 00 00 00 10 00 00 00");
            FModuleManager::RegisterImport("?StaticConfigName@AActor@@SAPEB_WXZ", UObject_StaticConfigName);
            FModuleManager::RegisterImport("??0AActor@@QEAA@AEAVFVTableHelper@@@Z", "40 53 48 83 EC 20 48 8D 05 ?? ?? ?? ?? 48 8B D9 48 89 01 48 8D 0D ?? ?? ?? ??");
            FModuleManager::RegisterImport("?AddReferencedObjects@AActor@@SAXPEAVUObject@@AEAVFReferenceCollector@@@Z", "48 89 5C 24 08 57 48 83 EC 20 48 8B DA 48 8B F9 48 8D 91 A0 01 00 00");
            FModuleManager::RegisterImport("?StaticClass@APawn@@SAPEAVUClass@@XZ", "4C 8B DC 48 81 EC 88 00 00 00 48 8B 05 ? ? ? ? 48 85 C0 0F 85 ? ? ? ? 49 89 43 F0 4C 8D 0D ? ? ? ? 88 44 24 70 4C 8D 05 ? ? ? ? 48 8D 05 ? ? ? ? 49 89 43 E0 48 8D 15 ? ? ? ? 48 8D 05 ? ? ? ? 49 89 43 D8 48 8D 0D ? ? ? ? 48 8D 05 ? ? ? ? 49 89 43 D0 48 8D 05 ? ? ? ? 49 89 43 C8 48 8D 05 ? ? ? ? 49 89 43 C0 48 8D 05 ? ? ? ? 49 89 43 B8 48 B8 00 00 00 00 40 00 00 00");
            FModuleManager::RegisterImport("?StaticConfigName@APawn@@SAPEB_WXZ", UObject_StaticConfigName);
            FModuleManager::RegisterImport("??0APawn@@QEAA@AEAVFVTableHelper@@@Z", "40 53 48 83 EC 20 48 8D 05 ?? ?? ?? ?? 48 8B D9 48 89 01 48 8D 0D ?? ?? ?? ??");
            FModuleManager::RegisterImport("?StaticConfigName@APawn@@SAPEB_WXZ", UObject_StaticConfigName);
            FModuleManager::RegisterImport("?Z_Construct_UClass_AActor@@YAPEAVUClass@@XZ", "48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 8B 01 48 85 C0 74 ? 48 8B D1 48 8B C8 E9 ? ? ? ? C3 CC CC CC CC CC CC CC CC CC CC CC CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? E8 ? ? ? ? 48 8B D0 4C 8D 05 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 89 05 ? ? ? ? 48 83 C4 28 C3 CC CC CC CC CC CC CC CC CC 4C 8B DC");
            FModuleManager::RegisterImport("?Z_Construct_UClass_USceneComponent_NoRegister@@YAPEAVUClass@@XZ", "4C 8B DC 48 81 EC 88 00 00 00 48 8B 05 ? ? ? ? 48 85 C0 0F 85 ? ? ? ? 49 89 43 F0 4C 8D 0D ? ? ? ? 88 44 24 70 4C 8D 05 ? ? ? ? 48 8D 05 ? ? ? ? 49 89 43 E0 48 8D 15 ? ? ? ? 48 8D 05 ? ? ? ? 49 89 43 D8 48 8D 0D ? ? ? ? 48 8D 05 ? ? ? ? 49 89 43 D0 48 8D 05 ? ? ? ? 49 89 43 C8 48 8D 05 ? ? ? ? 49 89 43 C0 48 8D 05 ? ? ? ? 49 89 43 B8 48 B8 00 00 00 00 80 00 00 00");
            FModuleManager::RegisterImport("?Z_Construct_UClass_UBoxComponent_NoRegister@@YAPEAVUClass@@XZ", "4C 8B DC 48 81 EC 88 00 00 00 48 8B 05 ? ? ? ? 48 85 C0 0F 85 ? ? ? ? 33 C9 48 8D 05 ? ? ? ? 49 89 4B F0 4C 8D 0D ? ? ? ? 88 4C 24 70 4C 8D 05 ? ? ? ? 49 89 43 E0 48 8D 15 ? ? ? ? 48 8D 05 ? ? ? ? 49 89 43 D8 48 8D 05 ? ? ? ? 49 89 43 D0 48 8D 05 ? ? ? ? 49 89 43 C8 48 8D 05 ? ? ? ? 49 89 43 C0 48 8D 05 ? ? ? ? 49 89 43 B8 49 89 4B B0 48 8D 0D ? ? ? ? C7 44 24 30 04 00 00 10 C7 44 24 28 10 00 00 00 C7 44 24 20 30 04 00 00 E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 81 C4 88 00 00 00 C3 CC CC CC CC CC CC CC CC CC CC CC CC CC CC B8 EA FA 9B 23");
            FModuleManager::RegisterImport("?Z_Construct_UClass_USphereComponent_NoRegister@@YAPEAVUClass@@XZ", "4C 8B DC 48 81 EC 88 00 00 00 48 8B 05 ? ? ? ? 48 85 C0 0F 85 ? ? ? ? 33 C9 48 8D 05 ? ? ? ? 49 89 4B F0 4C 8D 0D ? ? ? ? 88 4C 24 70 4C 8D 05 ? ? ? ? 49 89 43 E0 48 8D 15 ? ? ? ? 48 8D 05 ? ? ? ? 49 89 43 D8 48 8D 05 ? ? ? ? 49 89 43 D0 48 8D 05 ? ? ? ? 49 89 43 C8 48 8D 05 ? ? ? ? 49 89 43 C0 48 8D 05 ? ? ? ? 49 89 43 B8 49 89 4B B0 48 8D 0D ? ? ? ? C7 44 24 30 04 00 00 10 C7 44 24 28 10 00 00 00 C7 44 24 20 30 04 00 00 E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 81 C4 88 00 00 00 C3 CC CC CC CC CC CC CC CC CC CC CC CC CC CC 4C 8B DC 48 81 EC 88 00 00 00 48 8B 05 ? ? ? ? 48 85 C0 0F 85 ? ? ? ? 33 C9 48 8D 05 ? ? ? ? 49 89 4B F0 4C 8D 0D ? ? ? ? 88 4C 24 70 4C 8D 05 ? ? ? ? 49 89 43 E0 48 8D 15 ? ? ? ? 48 8D 05 ? ? ? ? 49 89 43 D8 48 8D 05 ? ? ? ? 49 89 43 D0 48 8D 05 ? ? ? ? 49 89 43 C8 48 8D 05 ? ? ? ? 49 89 43 C0 48 8D 05 ? ? ? ? 49 89 43 B8 49 89 4B B0 48 8D 0D ? ? ? ? C7 44 24 30 04 00 00 10");
            FModuleManager::RegisterImport("?Z_Construct_UClass_APawn@@YAPEAVUClass@@XZ", "48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 4C 8B DC 48 81 EC 88 00 00 00 48 8B 05 ? ? ? ? 48 85 C0 0F 85 ? ? ? ? 33 C9 48 8D 05 ? ? ? ? 49 89 4B F0 4C 8D 0D ? ? ? ? 88 4C 24 70 4C 8D 05 ? ? ? ? 49 89 43 E0 48 8D 15 ? ? ? ? 48 8D 05 ? ? ? ? 49 89 43 D8 48 8D 05 ? ? ? ? 49 89 43 D0 48 8D 05 ? ? ? ? 49 89 43 C8 48 8D 05 ? ? ? ? 49 89 43 C0 48 8D 05 ? ? ? ? 49 89 43 B8 49 89 4B B0 48 8D 0D ? ? ? ? C7 44 24 30 04 00 00 10 C7 44 24 28 10 00 00 00");
            FModuleManager::RegisterImport("?Z_Construct_UClass_AActor_NoRegister@@YAPEAVUClass@@XZ", "4C 8B DC 48 81 EC 88 00 00 00 48 8B 05 ? ? ? ? 48 85 C0 0F 85 ? ? ? ? 49 89 43 F0 4C 8D 0D ? ? ? ? 88 44 24 70 4C 8D 05 ? ? ? ? 48 8D 05 ? ? ? ? 49 89 43 E0 48 8D 15 ? ? ? ? 49 89 43 D8 48 8D 0D ? ? ? ? 48 8D 05 ? ? ? ? 49 89 43 D0 48 8D 05 ? ? ? ? 49 89 43 C8 48 8D 05 ? ? ? ? 49 89 43 C0 48 8D 05 ? ? ? ? 49 89 43 B8 48 B8 00 00 00 00 10 00 00 00");
            FModuleManager::RegisterImport("?Z_Construct_UClass_AActor_NoRegister@@YAPEAVUClass@@XZ", "4C 8B DC 48 81 EC 88 00 00 00 48 8B 05 ? ? ? ? 48 85 C0 0F 85 ? ? ? ? 49 89 43 F0 4C 8D 0D ? ? ? ? 88 44 24 70 4C 8D 05 ? ? ? ? 48 8D 05 ? ? ? ? 49 89 43 E0 48 8D 15 ? ? ? ? 48 8D 05 ? ? ? ? 49 89 43 D8 48 8D 0D ? ? ? ? 48 8D 05 ? ? ? ? 49 89 43 D0 48 8D 05 ? ? ? ? 49 89 43 C8 48 8D 05 ? ? ? ? 49 89 43 C0 48 8D 05 ? ? ? ? 49 89 43 B8 48 B8 00 00 00 00 40 00 00 00");
            FModuleManager::RegisterImport("?Z_Construct_UScriptStruct_FHitResult@@YAPEAVUScriptStruct@@XZ", "48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC E9 ? ? ? ? CC CC CC CC CC CC CC CC CC CC CC 4C 8B DC 48 81 EC 88 00 00 00 48 8B 05 ? ? ? ? 48 85 C0 0F 85 ? ? ? ? 33 C9 48 8D 05 ? ? ? ? 49 89 4B F0 4C 8D 0D ? ? ? ? 88 4C 24 70 4C 8D 05 ? ? ? ? 49 89 43 E0 48 8D 15 ? ? ? ? 49 89 43 D8");
            FModuleManager::RegisterImport("?Z_Construct_UEnum_Engine_ECollisionChannel@@YAPEAVUEnum@@XZ", "48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC 48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 83 C4 28 C3 CC E9 ? ? ? ?");
            FModuleManager::RegisterImport("?GetPrivateStaticClass@UShapeComponent@@CAPEAVUClass@@XZ", "4C 8B DC 48 81 EC 88 00 00 00 48 8B 05 ? ? ? ? 48 85 C0 0F 85 ? ? ? ? 33 C9 48 8D 05 ? ? ? ? 49 89 4B F0 4C 8D 0D ? ? ? ? 88 4C 24 70 4C 8D 05 ? ? ? ? 49 89 43 E0 48 8D 15 ? ? ? ? 48 8D 05 ? ? ? ? 49 89 43 D8 48 8D 05 ? ? ? ? 49 89 43 D0 48 8D 05 ? ? ? ? 49 89 43 C8 48 8D 05 ? ? ? ? 49 89 43 C0 48 8D 05 ? ? ? ? 49 89 43 B8 49 89 4B B0 48 8D 0D ? ? ? ? C7 44 24 30 05 00 00 10 C7 44 24 28 10 00 00 00 C7 44 24 20 20 04 00 00");
            FModuleManager::RegisterImport("?GetPrivateStaticClass@APlayerController@@CAPEAVUClass@@XZ", "4C 8B DC 48 81 EC 88 00 00 00 48 8B 05 ? ? ? ? 48 85 C0 0F 85 ? ? ? ? 49 89 43 F0 4C 8D 0D ? ? ? ? 88 44 24 70 4C 8D 05 ? ? ? ? 48 8D 05 ? ? ? ? 49 89 43 E0 48 8D 15 ? ? ? ? 48 8D 05 ? ? ? ? 49 89 43 D8 48 8D 0D ? ? ? ? 48 8D 05 ? ? ? ? 49 89 43 D0 48 8D 05 ? ? ? ? 49 89 43 C8 48 8D 05 ? ? ? ? 49 89 43 C0 48 8D 05 ? ? ? ? 49 89 43 B8 48 B8 00 00 00 00 20 00 00 00");
            FModuleManager::RegisterImport("?InternalGetTimerElapsed@FTimerManager@@AEBAMQEBUFTimerData@@@Z", "48 85 D2 74 ? 44 0F B6 42 01 41 83 E8 01 74 ? 41 83 F8 02 74 ? F3 0F 10 42 04");
            FModuleManager::RegisterImport("?InternalClearTimer@FTimerManager@@AEAAXUFTimerHandle@@@Z", "48 89 5C 24 08 48 89 54 24 ? 57 48 83 EC 20 48 8B C2");
            FModuleManager::RegisterImport("?InternalSetTimer@FTimerManager@@AEAAXAEAUFTimerHandle@@$$QEAUFTimerUnifiedDelegate@@M_NM@Z", "48 8B C4 55 56 57 41 56 48 8D 68 D8");
            FModuleManager::RegisterImport("?FindTimer@FTimerManager@@IEAAPEAUFTimerData@@AEBUFTimerHandle@@@Z", "4C 8B 12 4C 8B D9 4D 85 D2");
            FModuleManager::RegisterImport("?SpawnActor@UWorld@@QEAAPEAVAActor@@PEAVUClass@@PEBUFTransform@@AEBUFActorSpawnParameters@@@Z", "40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 F8 FE FF FF 48 81 EC 08 02 00 00 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 50");
            FModuleManager::RegisterImport("?SpawnActor@UWorld@@QEAAPEAVAActor@@PEAVUClass@@PEBUFVector@@PEBUFRotator@@AEBUFActorSpawnParameters@@@Z", "40 53 56 57 48 83 EC 70 48 8B 05 ? ? ? ? 48 33 C4 48 89 44 24 60 0F 28 1D ? ? ? ?");
            FModuleManager::RegisterImport("?LineTraceMultiByProfile@UWorld@@QEBA_NAEAV?$TArray@UFHitResult@@V?$TSizedDefaultAllocator@$0CA@@@@@AEBUFVector@@1VFName@@AEBUFCollisionQueryParams@@@Z", "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 20 57 41 56 41 57 48 81 EC 90 00 00 00");
            FModuleManager::RegisterImport("?LineTraceMultiByObjectType@UWorld@@QEBA_NAEAV?$TArray@UFHitResult@@V?$TSizedDefaultAllocator@$0CA@@@@@AEBUFVector@@1AEBUFCollisionObjectQueryParams@@AEBUFCollisionQueryParams@@@Z", "40 53 48 83 EC 40 48 8B 44 24 70 48 8B DA 48 89 44 24 38");
            FModuleManager::RegisterImport("?LineTraceMultiByChannel@UWorld@@QEBA_NAEAV?$TArray@UFHitResult@@V?$TSizedDefaultAllocator@$0CA@@@@@AEBUFVector@@1W4ECollisionChannel@@AEBUFCollisionQueryParams@@AEBUFCollisionResponseParams@@@Z", "48 83 EC 48 48 8D 05 ? ? ? ? 48 89 44 24 38 48 8B 84 24 80 00 00 00 48 89 44 24 30 48 8B 44 24 78 48 89 44 24 28 8B 44 24 70");
            FModuleManager::RegisterImport("??0FActorSpawnParameters@@QEAA@XZ", "33 C0 48 89 01 80 61 29 F0");
            FModuleManager::RegisterImport("??1APawn@@UEAA@XZ", "48 89 5C 24 10 57 48 83 EC 20 48 8D 05 ? ? ? ? 48 8B D9 48 89 81 28 02 00 00");
            FModuleManager::RegisterImport("?GetLifetimeReplicatedProps@APawn@@UEBAXAEAV?$TArray@VFLifetimeProperty@@V?$TSizedDefaultAllocator@$0CA@@@@@@Z", "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 40 48 8B DA E8 ? ? ? ? E8 ? ? ? ? 48 8B F0 E8 ? ? ? ? 41 B8 01 00 00 00 48 8D 15 ? ? ? ? 48 8D 4C 24 60 E8 ? ? ? ? 48 8D 54 24 30 48 8B C8 48 8B F8 E8 ? ? ? ? 48 8B 4C 24 ? 48 85 C9 74 ? E8 ? ? ? ? 48 8B 17 48 8B CE E8 ? ? ? ? 33 C9 4C 8D 44 24 20 48 89 4C 24 ? 48 8B D3 89 4C 24 ? 48 8B C8 E8 ? ? ? ? E8 ? ? ? ? 48 8B F0 E8 ? ? ? ? 41 B8 01 00 00 00 48 8D 15 ? ? ? ? 48 8D 4C 24 60 E8 ? ? ? ? 48 8D 54 24 30 48 8B C8 48 8B F8 E8 ? ? ? ? 48 8B 4C 24 ? 48 85 C9 74 ? E8 ? ? ? ? 48 8B 17 48 8B CE E8 ? ? ? ? 33 C9 4C 8D 44 24 20 48 89 4C 24 ? 48 8B D3 89 4C 24 ? 48 8B C8 E8 ? ? ? ? 48 C7 44 24 ? ? ? ? ?");
            FModuleManager::RegisterImport("??0APawn@@QEAA@AEBVFObjectInitializer@@@Z", "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 40 48 8B F9 E8 ? ? ? ? 33 F6 48 8D 05 ? ? ? ?");
            FModuleManager::RegisterImport("??1FCollisionQueryParams@@QEAA@XZ", NullSub);
            FModuleManager::RegisterImport("?AddIgnoredActors@FCollisionQueryParams@@QEAAXAEBV?$TArray@PEAVAActor@@V?$TSizedDefaultAllocator@$0CA@@@@@@Z", "40 57 41 55 41 56 48 83 EC 20");
            FModuleManager::RegisterImport("??0FCollisionQueryParams@@QEAA@XZ", VoidSub);
            FModuleManager::RegisterImport("??1AActor@@UEAA@XZ", "40 57 48 83 EC 30 48 8D 05 ?? ?? ?? ??");
            FModuleManager::RegisterImport("?GetWorldTimerManager@AActor@@QEBAAEAVFTimerManager@@XZ", "48 83 EC 28 48 8B 01 FF 90 50 01 00 00 48 8B 88 88 01 00 00");
            FModuleManager::RegisterImport("?PostCreateBlueprintComponent@AActor@@IEAAXPEAVUActorComponent@@@Z", "48 81 C1 60 01 00 00 E9 ? ? ? ? CC CC CC CC 48 83 EC 58");
            FModuleManager::RegisterImport("?GetParentComponent@AActor@@QEBAPEAVUChildActorComponent@@XZ", "48 85 D2 0F 84 ? ? ? ? 48 89 54 24 10 56 57");
            FModuleManager::RegisterImport("?Tick@AActor@@UEAAXM@Z", "40 53 48 83 EC 30 48 8B 41 10 48 8B D9");
            FModuleManager::RegisterImport("?DetachFromActor@AActor@@QEAAXAEBUFDetachmentTransformRules@@@Z", "48 8B 89 30 01 00 00 48 85 C9 74 ? 48 8B 01 48 FF A0 00 04 00 00");
            FModuleManager::RegisterImport("?AttachToComponent@AActor@@QEAAXPEAVUSceneComponent@@AEBUFAttachmentTransformRules@@VFName@@@Z", "48 8B 89 30 01 00 00 48 85 C9 74 ? 48 85 D2 0F 85 ? ? ? ?");
            FModuleManager::RegisterImport("?RegisterComponent@UActorComponent@@QEAAXXZ", "40 53 48 83 EC 20 48 8B D9 48 8B 89 A8 00 00 00 48 85 C9 74 ? 48 8B 01");
            FModuleManager::RegisterImport("?GetPrivateStaticClass@USceneComponent@@CAPEAVUClass@@XZ", "4C 8B DC 48 81 EC 88 00 00 00 48 8B 05 ? ? ? ? 48 85 C0 0F 85 ? ? ? ? 49 89 43 F0 4C 8D 0D ? ? ? ? 88 44 24 70 4C 8D 05 ? ? ? ? 48 8D 05 ? ? ? ? 49 89 43 E0 48 8D 15 ? ? ? ? 48 8D 05 ? ? ? ? 49 89 43 D8 48 8D 0D ? ? ? ? 48 8D 05 ? ? ? ? 49 89 43 D0 48 8D 05 ? ? ? ? 49 89 43 C8 48 8D 05 ? ? ? ? 49 89 43 C0 48 8D 05 ? ? ? ? 49 89 43 B8 48 B8 00 00 00 00 80 00 00 00");
            FModuleManager::RegisterImport("?SetRelativeTransform@USceneComponent@@QEAAXAEBUFTransform@@_NPEAUFHitResult@@W4ETeleportType@@@Z", "48 89 5C 24 08 57 48 83 EC 50 0F 10 52 10");
            FModuleManager::RegisterImport("?GetRelativeTransform@USceneComponent@@QEBA?AUFTransform@@XZ", "48 89 5C 24 08 57 48 81 EC 80 00 00 00 F2 0F 10 81 44 01 00 00");
            FModuleManager::RegisterImport("?SetWorldScale3D@USceneComponent@@QEAAXUFVector@@@Z", "48 89 5C 24 18 57 48 83 EC 70 48 8B 05 ? ? ? ? 48 33 C4 48 89 44 24 60 F2 0F 10 02");
            FModuleManager::RegisterImport("?SetWorldTransform@USceneComponent@@QEAAXAEBUFTransform@@_NPEAUFHitResult@@W4ETeleportType@@@Z", "40 53 55 56 57 48 81 EC A8 00 00 00 48 8B 05 ? ? ? ?");
            FModuleManager::RegisterImport("?GetForwardVector@USceneComponent@@QEBA?AUFVector@@XZ", "48 83 EC 18 0F 28 2D ? ? ? ? 48 8B C2 0F 29 34 24 0F 28 C5 0F 10 B1 D0 01 00 00 0F 28 DD 0F C6 C5 C9 0F 28 E6 0F C6 E6 C9 0F 28 CE 0F C6 CE D2 0F 59 C1 0F C6 DD D2 0F C6 F6 FF 0F 59 DC 0F 5C D8 0F 58 DB 0F 59 F3 0F 28 C3 0F C6 C3 C9 0F 28 D3 0F C6 D3 D2 0F 59 C1 0F 59 D4 0F 58 F5 0F 5C D0 0F 58 D6 0F 28 34 24 0F 28 C2 F3 0F 11 12 0F C6 C2 55 0F C6 D2 AA F3 0F 11 52 08 F3 0F 11 42 04 48 83 C4 18 C3 CC CC CC CC CC CC CC CC CC 48 89 5C 24 08");
            FModuleManager::RegisterImport("?GetUpVector@USceneComponent@@QEBA?AUFVector@@XZ", "48 83 EC 18 0F 28 2D ? ? ? ? 48 8B C2 0F 29 34 24 0F 28 C5 0F 10 B1 D0 01 00 00 0F 28 DD 0F C6 C5 C9 0F 28 E6 0F C6 E6 C9 0F 28 CE 0F C6 CE D2 0F 59 C1 0F C6 DD D2 0F C6 F6 FF 0F 59 DC 0F 5C D8 0F 58 DB 0F 59 F3 0F 28 C3 0F C6 C3 C9 0F 28 D3 0F C6 D3 D2 0F 59 C1 0F 59 D4 0F 58 F5 0F 5C D0 0F 58 D6 0F 28 34 24 0F 28 C2 F3 0F 11 12 0F C6 C2 55 0F C6 D2 AA F3 0F 11 52 08 F3 0F 11 42 04 48 83 C4 18 C3 CC CC CC CC CC CC CC CC CC 33 C0");
            FModuleManager::RegisterImport("?GetRightVector@USceneComponent@@QEBA?AUFVector@@XZ", "48 83 EC 18 0F 28 2D ? ? ? ? 48 8B C2 0F 29 34 24 0F 28 C5 0F 10 B1 D0 01 00 00 0F 28 DD 0F C6 C5 C9 0F 28 E6 0F C6 E6 C9 0F 28 CE 0F C6 CE D2 0F 59 C1 0F C6 DD D2 0F C6 F6 FF 0F 59 DC 0F 5C D8 0F 58 DB 0F 59 F3 0F 28 C3 0F C6 C3 C9 0F 28 D3 0F C6 D3 D2 0F 59 C1 0F 59 D4 0F 58 F5 0F 5C D0 0F 58 D6 0F 28 34 24 0F 28 C2 F3 0F 11 12 0F C6 C2 55 0F C6 D2 AA F3 0F 11 52 08 F3 0F 11 42 04 48 83 C4 18 C3 CC CC CC CC CC CC CC CC CC 48 89 5C 24 10");
            FModuleManager::RegisterImport("?SetupAttachment@USceneComponent@@QEAAXPEAV1@VFName@@@Z", "F6 81 88 00 00 00 01 4C 8B CA");
            FModuleManager::RegisterImport("?AttachToComponent@USceneComponent@@QEAA_NPEAV1@AEBUFAttachmentTransformRules@@VFName@@@Z", "40 55 53 56 41 54 41 56 48 8D AC 24 C0 F9 FF FF");
            FModuleManager::RegisterImport("??0AActor@@QEAA@XZ", "48 89 5C 24 18 57 48 83 EC 40 48 8D 05 ?? ?? ?? ??");
            FModuleManager::RegisterImport("?GetLocalRole@AActor@@QEBA?AW4ENetRole@@XZ", AActor_GetLocalRole);
            FModuleManager::RegisterImport("?SetActorRotation@AActor@@QEAA_NAEBUFQuat@@W4ETeleportType@@@Z", "48 83 EC 48 48 8B 89 30 01 00 00 48 85 C9 74 ? 48 8B 01");
            FModuleManager::RegisterImport("?SetActorTransform@AActor@@QEAA_NAEBUFTransform@@_NPEAUFHitResult@@W4ETeleportType@@@Z", "40 53 48 81 EC C0 00 00 00 48 8B 89 30 01 00 00 49 8B D9 48 85 C9 0F 84 ? ? ? ?");
            FModuleManager::RegisterImport("?SetActorRelativeRotation@AActor@@QEAAXAEBUFQuat@@_NPEAUFHitResult@@W4ETeleportType@@@Z", "40 53 48 81 EC D0 00 00 00 48 8B 89 30 01 00 00 33 DB 45 84 C0 49 0F 45 D9 48 85 C9 74 ? 8B 42 08 4C 8B CB F2 0F 10 02 48 8D 54 24 30 89 44 24 38 0F B6 84 24 00 01 00 00 88 44 24 20 F2 0F 11 44 24 30 E8 ? ? ? ? 48 81 C4 D0 00 00 00 5B C3 48 85 DB 74 ? 33 D2 48 8D 4C 24 40 41 B8 88 00 00 00 E8 ? ? ? ? F3 0F 10 05 ? ? ? ? 48 8D 44 24 40 F3 0F 11 44 24 48 0F 10 00 0F 10 48 10 0F 11 03 0F 10 40 20 0F 11 4B 10 0F 10 48 30 0F 11 43 20 0F 10 40 40 0F 11 4B 30 0F 10 48 50 0F 11 43 40 0F 10 40 60 0F 11 4B 50 0F 11 43 60 0F 10 40 70 48 8B 80 80 00 00 00 0F 11 43 70 48 89 83 80 00 00 00 48 81 C4 D0 00 00 00 5B C3 40 53 48 81 EC C0 00 00 00 48 8B 89 30 01 00 00 33 DB 45 84 C0 49 0F 45 D9 48 85 C9 74 ? 0F B6 84 24 F0 00 00 00 4C 8B CB 88 84 24 F0 00 00 00 48 81 C4 C0 00 00 00 5B E9 ? ? ? ? 48 85 DB 74 ? 33 D2 48 8D 4C 24 30 41 B8 88 00 00 00 E8 ? ? ? ? F3 0F 10 05 ? ? ? ? 48 8D 44 24 30 F3 0F 11 44 24 38 0F 10 00 0F 10 48 10 0F 11 03 0F 10 40 20 0F 11 4B 10 0F 10 48 30 0F 11 43 20 0F 10 40 40 0F 11 4B 30 0F 10 48 50 0F 11 43 40 0F 10 40 60 0F 11 4B 50 0F 11 43 60 0F 10 40 70 48 8B 80 80 00 00 00 0F 11 43 70 48 89 83 80 00 00 00 48 81 C4 C0 00 00 00 5B C3 CC CC CC CC 48 83 EC 58");
            FModuleManager::RegisterImport("?GetMovementComponent@APawn@@UEBAPEAVUPawnMovementComponent@@XZ", "40 53 48 83 EC 20 48 8B D9 E8 ? ? ? ? 4C 8B 03 48 8B D0 48 8B CB 48 83 C4 20");
            FModuleManager::RegisterImport("??$LogBogusChars@_WD@FGenericPlatformString@@CAXPEBDH@Z", "48 89 5C 24 18 55 41 56 41 57 48 83 EC 40 45 33 FF");
            FModuleManager::RegisterImport("?OneVector@FVector@@2U1@B", &OneVector);
            FModuleManager::RegisterImport("?Rotator@FQuat@@QEBA?AUFRotator@@XZ", "48 89 5C 24 08 57 48 83 EC 60 F3 0F 10 59 04");
            FModuleManager::RegisterImport("?ForwardVector@FVector@@2U1@B", &ForwardVector);
            FModuleManager::RegisterImport("?RightVector@FVector@@2U1@B", &RightVector);
            FModuleManager::RegisterImport("?Quaternion@FRotator@@QEBA?AUFQuat@@XZ", "48 83 EC 38 0F 28 2D ? ? ? ?");
            FModuleManager::RegisterImport("?GenerateNewID@FDelegateHandle@@CA_KXZ", "B9 01 00 00 00 8B C1 F0 48 0F C1 05 ? ? ? ?");
            FModuleManager::RegisterImport("??0FName@@QEAA@PEB_WW4EFindName@@@Z", "48 89 5C 24 08 57 48 83 EC 30 48 8B D9 48 89 54 24 20");
            FModuleManager::RegisterImport("?ToString@FName@@QEBA?AVFString@@XZ", "48 89 5C 24 10 48 89 6C 24 18 48 89 74 24 20 57 48 83 EC 20 8B 01");
            FModuleManager::RegisterImport("??8@YA_NUFNameEntryId@@W4EName@@@Z", "48 63 C2 48 8D 15 ? ? ? ? 3B 0C 82");
            FModuleManager::RegisterImport("?AppendChars@FString@@QEAAXPEB_WH@Z", "45 85 C0 0F 84 ? ? ? ? 48 89 5C 24 18 48 89 6C 24 20 56");
            FModuleManager::RegisterImport("??0FString@@QEAA@$$QEAV0@@Z", "45 33 C0 4C 89 01 48 8B 02 48 89 01 4C 89 02 8B 42 08 89 41 08 8B 42 0C 89 41 0C 48 8B C1");
            FModuleManager::RegisterImport("?UpVector@FVector@@2U1@B", &UpVector);
            FModuleManager::RegisterImport("?FindFunctionChecked@UObject@@QEBAPEAVUFunction@@VFName@@@Z", "48 89 5C 24 18 48 89 54 24 ? 57 48 83 EC 50");
            FModuleManager::RegisterImport("?FindFunction@UObject@@QEBAPEAVUFunction@@VFName@@@Z", "48 8B 49 10 41 B8 01 00 00 00");
            FModuleManager::RegisterImport("?CreateDefaultSubobject@UObject@@QEAAPEAV1@VFName@@PEAVUClass@@1_N2@Z", "48 89 5C 24 10 48 89 6C 24 18 56 57 41 56 48 83 EC 50 48 8D 44 24 70");
            FModuleManager::RegisterImport("?OnSubobjectCreatedFromReplication@AActor@@UEAAXPEAVUObject@@@Z", "48 85 D2 74 ? 53 48 83 EC 20 48 8B DA E8 ? ? ? ? 48 8B 53 10 4C 8D 40 30 48 63 40 38 3B 42 38 7F ? 48 8B C8 48 8B 42 30 4C 39 04 C8 75 ? 48 8B CB");
            FModuleManager::RegisterImport("?OnSubobjectDestroyFromReplication@AActor@@UEAAXPEAVUObject@@@Z", "48 85 D2 74 ? 53 48 83 EC 20 48 8B DA E8 ? ? ? ? 48 8B 53 10 4C 8D 40 30 48 63 40 38 3B 42 38 7F ? 48 8B C8 48 8B 42 30 4C 39 04 C8 75 ? 48 8B 03 33 D2");
            FModuleManager::RegisterImport("?PostNetReceive@AActor@@UEAAXXZ", "40 53 48 83 EC 20 F6 41 5B 08");
            FModuleManager::RegisterImport("?Serialize@AActor@@UEAAXAEAVFArchive@@@Z", "40 53 48 81 EC C0 01 00 00 48 8B 05 ?? ?? ?? ?? 48 33 C4 48 89 84 24 B8 01 00 00 48 8B D9 48 8D 4C 24 28 E8 ?? ?? ?? ?? 48 8B C8 48 8D 54 24 20 E8 ?? ?? ?? ?? 48 8B CB 48 8B 10 E8 ?? ?? ?? ?? 48 8D 4C 24 28 E8 ?? ?? ?? ?? 48 8B 8C 24 B8 01 00 00 48 33 CC E8 ?? ?? ?? ?? 48 81 C4 C0 01 00 00 5B C3 CC CC CC CC CC CC CC CC CC CC CC CC CC 40 53 56");
            FModuleManager::RegisterImport("?Serialize@AActor@@UEAAXVFStructuredArchiveRecord@@@Z", "40 53 56 57 48 83 EC 20 48 8B 02");
        }
        
        auto on_unreal_init() -> void override
        {
            Hook::RegisterProcessEventPreCallback(&process_event_hook);
        }

        static inline bool bModuleLoaded;
        
        auto static process_event_hook([[maybe_unused]] UObject* Context, [[maybe_unused]] UFunction* Function, [[maybe_unused]] void* Parms) -> void
        {
            if (!FModuleManager::InitializeWrapper() || bModuleLoaded) return;

            bModuleLoaded = true;
            SignatureContainer process_exec{
                        {{"40 55 53 56 57 41 54 41 55 41 56 41 57 48 81 EC B8 02 00 00"}},
                            [&](const SignatureContainer & self) {
                                if (!self.get_match_address())
                                    Output::send(std::format(STR("Failed to resolve symbol '?ProcessConsoleExec@UObject@@UEAA_NPEB_WAEAVFOutputDevice@@PEAV1@@Z': Signature not found\n")));
                                else
                                    CallFunctionByNameWithArguments.assign_address(self.get_match_address());
                                return true;
                        },
                    [](SignatureContainer & self) {},
                };
            
            SignatureContainer destroy_actor {
                {{"75 ? 8B 41 0C 3B 05 ? ? ? ? 7D ? 99 0F B7 D2 03 C2 8B C8 0F B7 C0 2B C2 48 98 C1 F9 10 48 63 C9 48 8D 14 40 48 8B 05 ? ? ? ? 48 8B 0C C8 48 8D 04 D1 EB ? 48 8B C7"}},
                    [&](const SignatureContainer & self) {
                        if (!self.get_match_address())
                            Output::send(std::format(STR("Failed to resolve symbol '?Destroy@AActor@@QEAA_N_N0@Z': Signature not found\n")));
                        else
                        {
                            AActor_Destroy.assign_address(self.get_match_address() - 0x24);
                        }
                        return true;
                },
                [](SignatureContainer & self) {},
            };
            
            SinglePassScanner::SignatureContainerMap signature_containers = {
                {ScanTarget::MainExe, {process_exec}},
                {ScanTarget::MainExe, {destroy_actor}},
            };

            SinglePassScanner::start_scan(signature_containers);

            register_symbols();

            const auto module_directory = std::filesystem::path(UE4SSProgram::get_program().get_module_directory());
            const auto plugin_directory = module_directory.parent_path().parent_path().parent_path() / "Plugins" / "RuntimeTransformer" / "Binaries" / "Win64";

            if (!exists(plugin_directory)) return;
            
            SetDllDirectory(plugin_directory.c_str());

            const auto dll_path = plugin_directory.wstring() + STR("\\RED-RuntimeTransformer-Win64-Shipping.dll");

            FModuleManager::Get().AddModule(FName(STR("RuntimeTransformer"), FNAME_Add), FString(dll_path.c_str()));
            FModuleManager::Get().LoadModule(FName(STR("RuntimeTransformer"), FNAME_Add));
        }
    };//class
}

/**
* export the start_mod() and uninstall_mod() functions to
* be used by the core ue4ss system to load in our dll mod
*/
#define RUNTIMETRANSFORMER_API __declspec(dllexport) 
extern "C" {
    RUNTIMETRANSFORMER_API CppUserModBase* start_mod(){ return new RuntimeTransformer::RuntimeTransformerPlugin(); }
    RUNTIMETRANSFORMER_API void uninstall_mod(CppUserModBase* mod) { delete mod; }
}