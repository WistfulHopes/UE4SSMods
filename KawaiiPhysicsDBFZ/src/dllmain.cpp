#include <DynamicOutput/Output.hpp>
#include <Mod/CppUserModBase.hpp>
#include <Modules/ModuleManager.hpp>
#include <polyhook2/Exceptions/AVehHook.hpp>
#include <SigScanner/SinglePassSigScanner.hpp>
#include <Unreal/UObjectGlobals.hpp>
#include <Unreal/UObject.hpp>

#include "FText.hpp"
#include "Hooks.hpp"
#include "UClass.hpp"
#include "UE4SSProgram.hpp"
#include "UEnum.hpp"
#include "FWeakObjectPtr.hpp"
#include "Quat.hpp"
#include "Rotator.hpp"
#include "Transform.hpp"
#include "Objbase.h"

namespace KawaiiPhysics
{
    using namespace RC;
    using namespace Unreal;
    
    /**
    * KawaiiPhysicsDBFZ: UE4SS c++ mod class defintion
    */
    class KawaiiPhysicsDBFZ : public CppUserModBase
    {
    public:
        
        // constructor
        KawaiiPhysicsDBFZ() {
            ModVersion = STR("0.1");
            ModName = STR("KawaiiPhysicsDBFZ");
            ModAuthors = STR("UE4SS");
            ModDescription = STR("A basic template C++ mod");
            // Do not change this unless you want to target a UE4SS version
            // other than the one you're currently building with somehow.
            //ModIntendedSDKVersion = STR("2.6");
        }
        
        // destructor
        ~KawaiiPhysicsDBFZ() override {
            // fill when required
        }
                
        static inline Function<void(UObject*)> UObjectBase_Destuctor;
        static inline Function<void*(void*, void*, const void*)> GetParentBoneIndex;
        static inline Function<void*(void*, void*)> FRichCurve_CopyConstructor;
        static inline Function<void(void*, void*)> FAnimNode_SkeletalControlBase_Construct;
        static inline Function<bool(UObject*, TCHAR*, FOutputDevice*, UObject*, bool)> CallFunctionByNameWithArguments;
        static inline FVector ZeroVector = FVector();
        static inline FVector OneVector = FVector(1,1,1);
        static inline FQuat FQuatIdentity = FQuat(0,0,0,1);
        static inline FTransform FTransformIdentity = FTransform();
        static inline FRotator ZeroRotator = FRotator();

        auto on_program_start() -> void override
        {
        }

        auto on_dll_load(std::wstring_view dll_name) -> void override
        {
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

        static intptr_t FSub()
        {
            return -1;
        }

        static int64_t Return2()
        {
            return 2;
        }

        static const TCHAR* UObject_StaticConfigName()
        {
            return STR("Engine");
        }

        static void ICppStructOps_Constructor(char* pThis, int Size, int Alignment)
        {
            *(int*)(pThis + 8) = Size;
            *(int*)(pThis + 12) = Alignment;
        }

        static void UEnum_Destructor(UEnum* pThis)
        {
            if (pThis->GetNames().GetData())
            {
                FMemory::Free(pThis->GetNames().GetData());
            }
            FMemory::Free(pThis->GetCppType().GetCharTArray().GetData());
            UObjectBase_Destuctor(pThis);
        }

        static int UNumericProperty_Char_GetMinAlignment(char* pThis)
        {
            return 4;
        }

        static int UNumericProperty_Int_GetMinAlignment(char* pThis)
        {
            return 4;
        }

        static int UObjectProperty_GetMinAlignment(char* pThis)
        {
            return 8;
        }
        
        static FString* UObject_GetDesc(UObject *pThis, FString *result)
        {
            result->Clear();
            return result;
        }
        
        static FName UObject_GetExporterName( void )
        {
            return FName(STR(""), FNAME_Add);
        }
        
        static void* FBaseCompactPose_GetParentBoneIndex( uint64* pThis, void* result, const void* BoneIndex )
        {
            result = GetParentBoneIndex((void*)pThis[2], result, BoneIndex);
            return result;
        }
        
        static void* FBoneContainer_GetCompactPoseIndexFromSkeletonIndex( char* pThis, void* result, const int SkeletonIndex )
        {
            result = ((char**)pThis + 0x80)[4 * SkeletonIndex];
            return result;
        }
        
        static void* FBoneContainer_GetRefPoseCompactArray( char* pThis )
        {
            return pThis + 0xB0;
        }
        
        static void* FBoneContainer_GetSkeletonAsset( char* pThis )
        {
            return ((FWeakObjectPtr*)(pThis + 0x40))->Get();
        }
        
        enum class EQueryMobilityType
        {
            Any,
            Static,	//Any shape that is considered static by physx (static mobility)
            Dynamic	//Any shape that is considered dynamic by physx (movable/stationary mobility)
        };

        struct TStatId {};
        
        /** Structure that defines parameters passed into collision function */
        struct FCollisionQueryParams
        {
            /** Tag used to provide extra information or filtering for debugging of the trace (e.g. Collision Analyzer) */
            FName TraceTag;

            /** Tag used to indicate an owner for this trace */
            FName OwnerTag;

            /** Whether we should perform the trace in the asynchronous scene.  Default is false. */
            bool bTraceAsyncScene;

            /** Whether we should trace against complex collision */
            bool bTraceComplex;

            /** Whether we want to find out initial overlap or not. If true, it will return if this was initial overlap. */
            bool bFindInitialOverlaps;

            /** Whether we want to return the triangle face index for complex static mesh traces */
            bool bReturnFaceIndex;

            /** Only fill in the PhysMaterial field of  */
            bool bReturnPhysicalMaterial;

            /** Whether to ignore blocking results. */
            bool bIgnoreBlocks;

            /** Whether to ignore touch/overlap results. */
            bool bIgnoreTouches;

            /** Filters query by mobility types (static vs stationary/movable)*/
            EQueryMobilityType MobilityType;

            /** TArray typedef of components to ignore. */
            typedef TArray<uint32, TInlineAllocator<8>> IgnoreComponentsArrayType;

            /** TArray typedef of actors to ignore. */
            typedef TArray<uint32, TInlineAllocator<4>> IgnoreActorsArrayType;

            /** Extra filtering done on the query. See declaration for filtering logic */
            uint8 IgnoreMask;

            /** StatId used for profiling individual expensive scene queries */
            TStatId StatId;
            
            /** Tracks whether the IgnoreComponents list is verified unique. */
            mutable bool bComponentListUnique;

            /** Set of components to ignore during the trace */
            mutable IgnoreComponentsArrayType IgnoreComponents;

            /** Set of actors to ignore during the trace */
            IgnoreActorsArrayType IgnoreActors;
        };
        
        static void FCollisionQueryParams_Destructor( FCollisionQueryParams* pThis )
        {
            pThis->IgnoreActors.Empty();
            pThis->IgnoreComponents.Empty();
        }

        static bool FRichCurve_HasAnyData(char* pThis)
        {
            return 3.4028235e38 != *(float*)(pThis + 0x5C) || *(int*)(pThis + 0x68);
        }

        static void FRuntimeFloatCurve_Destructor(char* pThis)
        {
            if (*(pThis + 0x60))
            {
                FMemory::Free(*(char**)(pThis + 0x60));
            }
            *(int*)(pThis + 0x50) = 0;
            if (*(pThis + 0x48))
            {
                FMemory::Free(*(char**)(pThis + 0x48));
            }
            if (*(pThis + 0x48))
            {
                FMemory::Free(*(char**)(pThis + 0x48));
            }
            auto Data = *(TArray<void*>*)(pThis + 0x8);
            Data.Empty();
            *(int*)(pThis + 0x38) = -1;
            *(int*)(pThis + 0x3C) = 0;
        }

        static void FRuntimeCurveFloat_CopyConstructor(char* pThis, char* that)
        {
            FRichCurve_CopyConstructor(pThis, that);
            *(uintptr_t*)(pThis + 0x70) = *(uintptr_t*)(that + 0x70);
        }

        struct FRichCurveKey
        {
            char padding[0x1C];
        };
        
        static void FRuntimeCurveFloat_OperatorEquals(char* pThis, char* that)
        {
            *(uint8*)(pThis + 0x58) = *(uint8*)(that + 0x58);
            *(uint8*)(pThis + 0x59) = *(uint8*)(that + 0x59);
            *(float*)(pThis + 0x5C) = *(float*)(that + 0x5C);
            auto ThisKeys = (TArray<FRichCurveKey>*)(that + 0x60);
            auto ThatKeys = (TArray<FRichCurveKey>*)(that + 0x60);
            *ThisKeys = *ThatKeys;
            *(uintptr_t*)(pThis + 0x70) = *(uintptr_t*)(that + 0x70);
        }
        
        auto static FStringDestructor(FString* pThis) -> void
        {
            if (pThis->GetCharTArray().GetData())
            {
                FMemory::Free(pThis->GetCharTArray().GetData());
            }
        }
        
        auto static FAnimNode_SkeletalControlBase_Constructor(void* pThis) -> void
        {
            FAnimNode_SkeletalControlBase_Construct(nullptr, pThis);
        }
        
        static bool ProcessConsoleExec(UObject* pThis, TCHAR* Cmd, FOutputDevice* Ar, UObject* Executor)
        {
            return CallFunctionByNameWithArguments(pThis, Cmd, Ar, Executor, false);
        }
        
        MS_ALIGN(16) struct FPlane
            : public FVector
        {
            /** The w-component. */
            float W;
        };

        static float GetTForSegmentPlaneIntersect(FVector& StartPoint, FVector& EndPoint, FPlane& Plane)
        {
            auto Temp = FVector(EndPoint.X() - StartPoint.X(), EndPoint.Y() - StartPoint.Y(), EndPoint.Z() - StartPoint.Z());
            return ( Plane.W - (StartPoint.X() * Plane.X() + StartPoint.Y() * Plane.Y() + StartPoint.Z() * Plane.Z()) ) / (Temp.X() * Plane.X() + Temp.Y() * Plane.Y() + Temp.Z() * Plane.Z());	
        }

        static bool SegmentPlaneIntersection(FVector& StartPoint, FVector& EndPoint, FPlane& Plane, FVector& out_IntersectionPoint)
        {
            float T = GetTForSegmentPlaneIntersect(StartPoint, EndPoint, Plane);
            // If the parameter value is not between 0 and 1, there is no intersection
            if (T > -(1.e-4f) && T < 1.f + (1.e-4f))
            {
                auto Temp = FVector(T * (EndPoint.X() - StartPoint.X()), T * (EndPoint.Y() - StartPoint.Y()), T * (EndPoint.Z() - StartPoint.Z()));
                out_IntersectionPoint = FVector(Temp.X() + StartPoint.X(), Temp.Y() + StartPoint.Y(), Temp.Z() + StartPoint.Z());
                return true;
            }
            return false;
        }

        static void FName_Constructor(FName* pThis, const TCHAR* Name, EFindName FindType)
        {
            *pThis = FName(Name, FindType);
        }

        static GUID* NewGuid(GUID* result)
        {
            CoCreateGuid(result);
            return result;
        }
        
        /** Enum indicating how each type should respond */
        enum ECollisionResponse
        {
            ECR_Ignore,
            ECR_Overlap,
            ECR_Block,
            ECR_MAX,
        };

        struct FCollisionResponseContainer
        {
            uint8 EnumArray[32];

            void SetAllChannels(ECollisionResponse NewResponse)
            {
                for(int32 i=0; i<32; i++)
                {
                    EnumArray[i] = NewResponse;
                }
            }
        };

        /** Structure that defines response container for the query. Advanced option. */
        struct FCollisionResponseParams
        {
            /** 
             *	Collision Response container for trace filtering. If you'd like to ignore certain channel for this trace, use this struct.
             *	By default, every channel will be blocked
             */
            struct FCollisionResponseContainer CollisionResponse;

            FCollisionResponseParams(ECollisionResponse DefaultResponse = ECR_Block)
            {
                CollisionResponse.SetAllChannels(DefaultResponse);
            }

            FCollisionResponseParams(const FCollisionResponseContainer& ResponseContainer)
            {
                CollisionResponse = ResponseContainer;
            }
            /** static variable for default data to be used without reconstructing everytime **/
            static FCollisionResponseParams DefaultResponseParam;
        };

        static void FCollisionResponseParams_Constructor(FCollisionResponseParams* pThis, const FCollisionResponseContainer& ResponseContainer)
        {
            pThis->CollisionResponse = ResponseContainer;
        }
        
        static auto register_symbols() -> void
        {
            FModuleManager::RegisterImport("?Z_Construct_UScriptStruct_FTransform@@YAPEAVUScriptStruct@@XZ", "48 81 EC 68 06 00 00 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 50 06 00 00 C7 44 24 40 00 00 00 00 E8 ? ? ? ? 48 89 44 24 58 B8 18 00 00 00 8B C0 8B 0D ? ? ? ? 65 48 8B 14 25 58 00 00 00 48 8B 0C CA 8B 04 08 39 05 ? ? ? ? 7E ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 83 3D ? ? ? ? ? 75 ? E8 ? ? ? ? C6 44 24 20 00 44 8B C8 41 B8 30 00 00 00 48 8D 15 ? ? ? ? 48 8B 4C 24 58 E8 ? ? ? ? 48 89 05 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 83 3D ? ? ? ? ? 0F 85 ? ? ? ? 48 8D 84 24 D8 00 00 00 48 89 84 24 A0 00 00 00");
            FModuleManager::RegisterImport("?StaticFindObjectFast@@YAPEAVUObject@@PEAVUClass@@PEAV1@VFName@@_N3W4EObjectFlags@@W4EInternalObjectFlags@@@Z", "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 20 4C 89 44 24 18 57 48 83 EC 40 80 3D ? ? ? ? ?");
            FModuleManager::RegisterImport("??0FObjectInitializer@@QEAA@XZ", "48 89 5C 24 10 48 89 74 24 18 57 48 83 EC 30 33 C0 C6 41 12 01");
            FModuleManager::RegisterImport("??0FObjectInitializer@@QEAA@XZ", "48 89 5C 24 10 48 89 74 24 18 57 48 83 EC 30 33 C0 C6 41 12 01");
            FModuleManager::RegisterImport("??1FObjectInitializer@@QEAA@XZ", "48 83 EC 48 F6 81 48 01 00 00 01");
            FModuleManager::RegisterImport("?GetConvertedDynamicPackageNameToTypeName@@YAAEAV?$TMap@VFName@@V1@VFDefaultSetAllocator@@U?$TDefaultMapHashableKeyFuncs@VFName@@V1@$0A@@@@@XZ", "48 83 EC 28 65 48 8B 04 25 58 00 00 00 8B 0D ? ? ? ? BA 18 00 00 00 48 8B 0C C8 8B 04 0A 39 05 ? ? ? ? 7F ? 48 8D 05 ? ? ? ? 48 83 C4 28 C3 48 8D 0D ? ? ? ? E8 ? ? ? ? 83 3D ? ? ? ? ? 75 ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? EB ? CC CC CC CC CC CC CC CC CC CC CC CC CC CC 48 83 EC 28 65 48 8B 04 25 58 00 00 00 8B 0D ? ? ? ? BA 18 00 00 00 48 8B 0C C8 8B 04 0A 39 05 ? ? ? ? 7F ? 48 8D 05 ? ? ? ? 48 83 C4 28 C3 48 8D 0D ? ? ? ? E8 ? ? ? ? 83 3D ? ? ? ? ? 75 ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? EB ? CC CC CC CC CC CC CC CC CC CC CC CC CC CC 48 83 EC 28 65 48 8B 04 25 58 00 00 00 8B 0D ? ? ? ? BA 18 00 00 00 48 8B 0C C8 8B 04 0A 39 05 ? ? ? ? 7F ? 48 8D 05 ? ? ? ? 48 83 C4 28 C3 48 8D 0D ? ? ? ? E8 ? ? ? ? 83 3D ? ? ? ? ? 75 ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? EB ? CC CC CC CC CC CC CC CC CC CC CC CC CC CC 48 83 EC 28 65 48 8B 04 25 58 00 00 00 8B 0D ? ? ? ? BA 18 00 00 00 48 8B 0C C8 8B 04 0A 39 05 ? ? ? ? 7F ? 48 8D 05 ? ? ? ? 48 83 C4 28 C3 48 8D 0D ? ? ? ? E8 ? ? ? ? 83 3D ? ? ? ? ? 75 ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? EB ? CC CC CC CC CC CC CC CC CC CC CC CC CC CC 48 89 5C 24 18");
            FModuleManager::RegisterImport("?UObjectForceRegistration@@YAXPEAVUObjectBase@@@Z", "48 89 5C 24 18 48 89 74 24 20 41 56 48 83 EC 20 4C 8B F1");
            FModuleManager::RegisterImport("?UClassCompiledInDefer@@YAXPEAUFFieldCompiledInInfo@@PEB_W_KI@Z", "48 89 5C 24 08 57 48 83 EC 30 48 8B F9 41 B8 01 00 00 00");
            FModuleManager::RegisterImport("?UObjectCompiledInDefer@@YAXP6APEAVUClass@@XZ0PEB_W1_N1P6AXAEAV?$TMap@VFName@@V1@VFDefaultSetAllocator@@U?$TDefaultMapHashableKeyFuncs@VFName@@V1@$0A@@@@@@Z@Z", "40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 F8 48 81 EC 08 01 00 00 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 F0 80 7D 70 00");
            FModuleManager::RegisterImport("?UObjectCompiledInDeferStruct@@YAXP6APEAVUScriptStruct@@XZPEB_WVFName@@_N1@Z", "48 8B C4 4C 89 40 18 48 89 48 08 48 83 EC 68 48 89 68 F8 48 8B EA 48 89 70 F0 41 0F B6 F1 45 84 C9 75 ? 48 89 58 10 48 89 78 E8 48 89 48 C8 48 89 50 D0 E8 ? ? ? ? 48 8B D8 48 63 78 08 8D 4F 01 89 48 08 3B 48 0C 7E ? 8B D7 48 8B C8 E8 ? ? ? ? 48 8B CF 48 8B 7C 24 50 48 C1 E1 04 48 03 0B 48 8B 5C 24 78 74 ? 0F 10 44 24 30 0F 11 01 EB ? 48 8B 94 24 90 00 00 00 48 8D 4C 24 30 41 B8 01 00 00 00 E8 ? ? ? ? E8 ? ? ? ? 48 8D 4C 24 30 45 33 C9 48 89 4C 24 40 4C 8D 44 24 40 48 8D 4C 24 70 48 89 4C 24 48 48 8D 94 24 88 00 00 00 48 8B C8 E8 ? ? ? ? 48 8D 54 24 40 48 8D 8C 24 80 00 00 00 E8 ? ? ? ? 83 78 08 00 74 ? 48 8B 10 EB ? 48 8D 15 ? ? ? ? 48 8B 44 24 70 45 33 C9 40 88 74 24 28 48 8B CD 48 89 44 24 20 45 8D 41 02");
            FModuleManager::RegisterImport("?GetStaticStruct@@YAPEAVUScriptStruct@@P6APEAV1@XZPEAVUObject@@PEB_W_KI@Z", "48 89 5C 24 10 48 89 6C 24 18 48 89 74 24 20 57 48 83 EC 40 48 8B 42 20 49 8B E8 48 8B DA 48 8B F9 48 85 C0 74 ? 66 66 0F 1F 84 00 00 00 00 00 48 8B D0 48 8B 40 20 48 85 C0 75 ? 48 8B 42 18 48 8D 4C 24 50 48 8D 54 24 30 48 89 44 24 50 E8 ? ? ? ? 83 7C 24 38 00 48 8D 35 ? ? ? ? 41 B9 01 00 00 00 C6 44 24 28 00 48 8B CE 48 C7 44 24 20 00 00 00 00 48 0F 45 4C 24 30 48 8B D5 45 8D 41 01");
            FModuleManager::RegisterImport("?UObjectCompiledInDeferEnum@@YAXP6APEAVUEnum@@XZPEB_WVFName@@_N1@Z", "48 8B C4 4C 89 40 18 48 89 48 08 48 83 EC 68 48 89 68 F8 48 8B EA 48 89 70 F0 41 0F B6 F1 45 84 C9 75 ? 48 89 58 10 48 89 78 E8 48 89 48 C8 48 89 50 D0 E8 ? ? ? ? 48 8B D8 48 63 78 08 8D 4F 01 89 48 08 3B 48 0C 7E ? 8B D7 48 8B C8 E8 ? ? ? ? 48 8B CF 48 8B 7C 24 50 48 C1 E1 04 48 03 0B 48 8B 5C 24 78 74 ? 0F 10 44 24 30 0F 11 01 EB ? 48 8B 94 24 90 00 00 00 48 8D 4C 24 30 41 B8 01 00 00 00 E8 ? ? ? ? E8 ? ? ? ? 48 8D 4C 24 30 45 33 C9 48 89 4C 24 40 4C 8D 44 24 40 48 8D 4C 24 70 48 89 4C 24 48 48 8D 94 24 88 00 00 00 48 8B C8 E8 ? ? ? ? 48 8D 54 24 40 48 8D 8C 24 80 00 00 00 E8 ? ? ? ? 83 78 08 00 74 ? 48 8B 10 EB ? 48 8D 15 ? ? ? ? 48 8B 44 24 70 45 33 C9 40 88 74 24 28 48 8B CD 48 89 44 24 20 45 8D 41 03");
            FModuleManager::RegisterImport("?GetStaticEnum@@YAPEAVUEnum@@P6APEAV1@XZPEAVUObject@@PEB_W@Z", "48 89 5C 24 10 48 89 6C 24 18 48 89 74 24 20 57 48 83 EC 40 48 8B 42 20 49 8B E8 48 8B DA 48 8B F9 48 85 C0 74 ? 66 66 0F 1F 84 00 00 00 00 00 48 8B D0 48 8B 40 20 48 85 C0 75 ? 48 8B 42 18 48 8D 4C 24 50 48 8D 54 24 30 48 89 44 24 50 E8 ? ? ? ? 83 7C 24 38 00 48 8D 35 ? ? ? ? 41 B9 01 00 00 00 C6 44 24 28 00 48 8B CE 48 C7 44 24 20 00 00 00 00 48 0F 45 4C 24 30 48 8B D5 45 8D 41 02");
            FModuleManager::RegisterImport("?FindExistingStructIfHotReloadOrDynamic@@YAPEAVUScriptStruct@@PEAVUObject@@PEB_W_KI_N@Z", "48 89 74 24 10 57 48 83 EC 50 80 BC 24 80 00 00 00 00 48 8B FA 48 8B F1 0F 84 ? ? ? ? 48 89 5C 24 60 E8 ? ? ? ? 41 B8 01 00 00 00 48 8D 4C 24 40 48 8B D7 48 8B D8 E8 ? ? ? ? C7 44 24 30 00 00 00 00 45 33 C9 C7 44 24 28 00 00 00 00 48 8B D6 48 8B CB C6 44 24 20 00 4C 8B 00 E8 ? ? ? ? 48 8B D8 48 85 C0 74 ? E8 ? ? ? ? 48 8B 4B 10 48 05 88 00 00 00 48 63 50 08 3B 91 90 00 00 00 7F ? 48 8B 89 88 00 00 00 48 39 04 D1 75 ? 48 8B C3 48 8B 5C 24 60 48 8B 74 24 68 48 83 C4 50 5F C3 33 C0 48 8B 5C 24 60 48 8B 74 24 68 48 83 C4 50 5F C3 33 C0 48 8B 74 24 68 48 83 C4 50 5F C3 CC CC CC CC CC CC CC CC CC CC 4C 8B C2");
            FModuleManager::RegisterImport("?FindExistingEnumIfHotReloadOrDynamic@@YAPEAVUEnum@@PEAVUObject@@PEB_W_KI_N@Z", "48 89 74 24 10 57 48 83 EC 50 80 BC 24 80 00 00 00 00 48 8B FA 48 8B F1 0F 84 ? ? ? ? 48 89 5C 24 60 E8 ? ? ? ? 41 B8 01 00 00 00 48 8D 4C 24 40 48 8B D7 48 8B D8 E8 ? ? ? ? C7 44 24 30 00 00 00 00 45 33 C9 C7 44 24 28 00 00 00 00 48 8B D6 48 8B CB C6 44 24 20 00 4C 8B 00 E8 ? ? ? ? 48 8B D8 48 85 C0 74 ? E8 ? ? ? ? 48 8B 4B 10 48 05 88 00 00 00 48 63 50 08 3B 91 90 00 00 00 7F ? 48 8B 89 88 00 00 00 48 39 04 D1 75 ? 48 8B C3 48 8B 5C 24 60 48 8B 74 24 68 48 83 C4 50 5F C3 33 C0 48 8B 5C 24 60 48 8B 74 24 68 48 83 C4 50 5F C3 33 C0 48 8B 74 24 68 48 83 C4 50 5F C3 CC CC CC CC CC CC CC CC CC CC 48 89 74 24 10");
            FModuleManager::RegisterImport("?StaticClass@UObject@@SAPEAVUClass@@XZ", "48 81 EC 98 00 00 00 48 83 3D ? ? ? ? ? 0F 85 ? ? ? ? E8 ? ? ? ? 48 89 44 24 70 E8 ? ? ? ? 48 89 44 24 78 48 8D 0D ? ? ? ? 48 83 C1 02 48 89 8C 24 80 00 00 00 E8 ? ? ? ? C6 44 24 68 00 48 8D 0D ? ? ? ? 48 89 4C 24 60 48 8D 0D ? ? ? ? 48 89 4C 24 58 48 8D 0D ? ? ? ? 48 89 4C 24 50 48 8D 0D ? ? ? ? 48 89 4C 24 48 48 8D 0D ? ? ? ? 48 89 4C 24 40 48 8B 4C 24 70 48 89 4C 24 38 48 8B 4C 24 78 48 89 4C 24 30 C7 44 24 28 01 01 00 00");
            FModuleManager::RegisterImport("?StaticConfigName@UObject@@SAPEB_WXZ", UObject_StaticConfigName);
            FModuleManager::RegisterImport("?AddReferencedObjects@UObject@@SAXPEAV1@AEAVFReferenceCollector@@@Z", VoidSub);
            FModuleManager::RegisterImport("?StaticLink@UStruct@@QEAAX_N@Z", "48 89 5C 24 08 57 48 81 EC B0 00 00 00 48 8B F9 0F B6 DA");
            FModuleManager::RegisterImport("??0ICppStructOps@UScriptStruct@@QEAA@HH@Z", ICppStructOps_Constructor);
            FModuleManager::RegisterImport("??1ICppStructOps@UScriptStruct@@UEAA@XZ", VoidSub);
            FModuleManager::RegisterImport("?GetPrivateStaticClass@UScriptStruct@@CAPEAVUClass@@XZ", "48 83 EC 68 48 8B 05 ? ? ? ? 48 85 C0 0F 85 ? ? ? ? 48 89 5C 24 78 44 8D 40 08 41 B1 01 48 89 7C 24 60 BA 58 02 00 00 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 89 05 ? ? ? ? 48 8B D8 48 8D 3D ? ? ? ? 48 85 C0 74 ? 41 B8 01 00 00 00 48 8D 4C 24 70 48 8B D7 E8 ? ? ? ? 48 8D 0D ? ? ? ? 41 B9 98 00 00 00");
            FModuleManager::RegisterImport("??0UScriptStruct@@QEAA@AEBVFObjectInitializer@@PEAV0@PEAUICppStructOps@0@W4EStructFlags@@_K4@Z", "48 89 5C 24 08 57 48 83 EC 30 49 8B D9 48 8B F9");
            FModuleManager::RegisterImport("?DeferCppStructOps@UScriptStruct@@SAXVFName@@PEAUICppStructOps@1@@Z", "48 89 54 24 10 48 89 4C 24 08 48 83 EC 38 65 48 8B 04 25 58 00 00 00");
            FModuleManager::RegisterImport("??2UEnum@@SAPEAX_KW4EInternal@@PEAVUObject@@VFName@@W4EObjectFlags@@@Z", "4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 85 C9");
            FModuleManager::RegisterImport("??0UEnum@@QEAA@AEBVFObjectInitializer@@@Z", "40 53 48 83 EC 20 48 8B D9 E8 ? ? ? ? 48 8D 05 ? ? ? ? 48 89 03 33 C0 48 89 43 30 48 89 43 38 48 89 43 40 48 89 43 48 89 43 50");
            FModuleManager::RegisterImport("??1UEnum@@UEAA@XZ", UEnum_Destructor);
            FModuleManager::RegisterImport("?SetCppTypeInfo@UClass@@QEAAXPEAUICppClassTypeInfo@@@Z", "48 89 91 08 01 00 00 C3");
            FModuleManager::RegisterImport("?GetPrivateStaticClassBody@@YAXPEB_W0AEAPEAVUClass@@P6AXXZIW4EClassFlags@@_K0P6AXAEBVFObjectInitializer@@@ZP6APEAVUObject@@AEAVFVTableHelper@@@ZP6AXPEAV4@AEAVFReferenceCollector@@@ZP6APEAV1@XZP6APEAV1@XZ_N@Z", "48 89 5C 24 10 48 89 6C 24 18 57 41 56 41 57 48 83 EC 60 80 BC 24 E8 00 00 00 00");
            FModuleManager::RegisterImport("??2UByteProperty@@SAPEAX_KW4EInternal@@PEAVUObject@@VFName@@W4EObjectFlags@@@Z", "4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 85 C9");
            FModuleManager::RegisterImport("??0UByteProperty@@QEAA@AEBVFObjectInitializer@@W4ECppProperty@@H_KPEAVUEnum@@@Z", "40 53 48 83 EC 30 48 8B 44 24 60 48 8B D9 48 B9 00 02 00 40 10 00 08 00 45 33 C0 48 0B C1 48 8B CB 48 89 44 24 20 E8 ? ? ? ? 48 8D 05 ? ? ? ? C7 43 34 01 00 00 00 48 89 03 48 8B 44 24 68");
            FModuleManager::RegisterImport("??1UByteProperty@@UEAA@XZ", "48 89 5C 24 08 57 48 83 EC 20 89 D3 48 89 CF E8 ? ? ? ? F6 C3 01 74 0D BA 80 00 00 00 48 89 F9 E8 ? ? ? ? 48 89  F8 48 8B 5C 24 30 48 83 C4 20 5F C3 2F AB 09 02 73 3E A5 CB F9 DD C6 01");
            FModuleManager::RegisterImport("??2UIntProperty@@SAPEAX_KW4EInternal@@PEAVUObject@@VFName@@W4EObjectFlags@@@Z", "4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 85 C9");
            FModuleManager::RegisterImport("??0UIntProperty@@QEAA@AEBVFObjectInitializer@@W4ECppProperty@@H_K@Z", "40 53 48 83 EC 30 48 8B 44 24 60 48 8B D9 48 B9 00 02 00 40 10 00 08 00 45 33 C0 48 0B C1 48 8B CB 48 89 44 24 20 E8 ? ? ? ? 48 8D 05 ? ? ? ? C7 43 34 04 00 00 00 48 89 03 48 8B C3 48 83 C4 30 5B C3 CC CC CC CC CC CC CC CC CC CC CC 40 53 48 83 EC 30 48 8B 44 24 60 48 8B D9 48 B9 00 02 00 40 10 00 08 00 45 33 C0 48 0B C1 48 8B CB 48 89 44 24 20 E8 ? ? ? ? 48 8B 44 24 68 48 89 43 78 48 8D 05 ? ? ? ? 48 89 03 48 8B C3 C7 43 34 08 00 00 00 48 83 C4 30 5B C3 CC CC 48 89 5C 24 08");
            FModuleManager::RegisterImport("??1UIntProperty@@UEAA@XZ", "48 89 5C 24 08 57 48 83 EC 20 89 D3 48 89 CF E8 ? ? ? ? F6 C3 01 74 ? BA 78 00 00 00 48 89 F9 E8 ? ? ? ? 48 89 F8 48 8B 5C 24 30 48 83 C4 20 5F C3 A1 5A 8C FC 4B 56 1A FF 5E");
            FModuleManager::RegisterImport("??2UFloatProperty@@SAPEAX_KW4EInternal@@PEAVUObject@@VFName@@W4EObjectFlags@@@Z", "4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 85 C9");
            FModuleManager::RegisterImport("??0UFloatProperty@@QEAA@AEBVFObjectInitializer@@W4ECppProperty@@H_K@Z", "40 53 48 83 EC 30 48 8B 44 24 60 48 8B D9 48 B9 00 02 00 40 10 00 08 00 45 33 C0 48 0B C1 48 8B CB 48 89 44 24 20 E8 ? ? ? ? 48 8D 05 ? ? ? ? C7 43 34 04 00 00 00 48 89 03 48 8B C3 48 83 C4 30 5B C3 CC CC CC CC CC CC CC CC CC CC CC 40 53 48 83 EC 30 48 8B 44 24 60 48 8B D9 48 B9 00 02 00 40 10 00 08 00 45 33 C0 48 0B C1 48 8B CB 48 89 44 24 20 E8 ? ? ? ? 48 8D 05 ? ? ? ?");
            FModuleManager::RegisterImport("??1UFloatProperty@@UEAA@XZ", "48 89 5C 24 08 57 48 83 EC 20 89 D3 48 89 CF E8 ? ? ? ? F6 C3 01 74 ? BA 78 00 00 00 48 89 F9 E8 ? ? ? ? 48 89 F8 48 8B 5C 24 30 48 83 C4 20 5F C3 A1 5A 8C FC 4B 56 1A FF 5E");
            FModuleManager::RegisterImport("??2UBoolProperty@@SAPEAX_KW4EInternal@@PEAVUObject@@VFName@@W4EObjectFlags@@@Z", "4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 85 C9");
            FModuleManager::RegisterImport("??0UBoolProperty@@QEAA@AEBVFObjectInitializer@@W4ECppProperty@@H_KII_N@Z", "48 89 5C 24 08 57 48 83 EC 20 41 8B D9 48 8B F9 E8 ? ? ? ? C7 47 30 01 00 00 00 48 8D 05 ? ? ? ? 48 89 07 48 B9 00 00 00 00 00 00 08 00");
            FModuleManager::RegisterImport("??1UBoolProperty@@UEAA@XZ", "48 89 5C 24 08 57 48 83 EC 20 89 D3 48 89 CF E8 ? ? ? ? F6 C3 01 74 ? BA 80 00 00 00 48 89 F9 E8 ? ? ? ? 48 89 F8 48 8B 5C 24 30 48 83 C4 20 5F C3 91");
            FModuleManager::RegisterImport("??2UObjectProperty@@SAPEAX_KW4EInternal@@PEAVUObject@@VFName@@W4EObjectFlags@@@Z", "4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 85 C9");
            FModuleManager::RegisterImport("??0UObjectProperty@@QEAA@AEBVFObjectInitializer@@W4ECppProperty@@H_KPEAVUClass@@@Z", "40 53 48 83 EC 30 48 8B 44 24 60 48 8B D9 48 B9 00 02 00 40 10 00 08 00 45 33 C0 48 0B C1 48 8B CB 48 89 44 24 20 E8 ? ? ? ? 48 8B 44 24 68 48 89 43 78 48 8D 05 ? ? ? ? 48 89 03 48 8B C3 C7 43 34 08 00 00 00 48 83 C4 30 5B C3 CC CC 48 89 5C 24 08");
            FModuleManager::RegisterImport("??1UObjectProperty@@UEAA@XZ", "48 89 5C 24 08 57 48 83 EC 20 89 D3 48 89 CF E8 ? ? ? ? F6 C3 01 74 0D BA 80 00 00 00 48 89 F9 E8 ? ? ? ? 48 89  F8 48 8B 5C 24 30 48 83 C4 20 5F C3 2F AB 09 02 73 3E A5 CB F9 DD C6 01");
            FModuleManager::RegisterImport("??2UNameProperty@@SAPEAX_KW4EInternal@@PEAVUObject@@VFName@@W4EObjectFlags@@@Z", "4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 48 89 5C 24 10 48 89 7C 24 18 55 48 8B EC 48 83 EC 70 48 8B FA 48 8B D9 48 3B CA 0F 84 ? ? ? ? 8B 52 08 2B 57 34 E8 ? ? ? ? 44 8B 5F 28 48 8D 57 10 45 33 C0 C7 45 B4 01 00 00 00 44 89 45 B0 45 8B C8 48 89 55 B8 C7 45 C0 FF FF FF FF 4C 89 45 C4 45 85 DB 0F 84 ? ? ? ? 48 8B 42 10 48 F7 D8 41 8D 43 FF 4D 1B D2 49 F7 DA 49 FF CA 4C 23 D2 4C 0B 52 10 99 83 E2 1F 03 C2 C1 F8 05 41 8B 0A 85 C9 75 ? 0F 1F 84 00 00 00 00 00 41 FF C0 41 83 C1 20 44 89 45 B0 44 89 4D C8 44 3B C0 7F ? 49 63 C8 C7 45 C0 FF FF FF FF 41 8B 0C 8A 85 C9 74 ? 8D 41 FF F7 D0 23 C1 0F BD C8 89 45 B4 74 ? B8 1F 00 00 00 2B C1 EB ? 45 8B C3 44 89 5D C4 EB ? B8 20 00 00 00 45 8B C1 44 2B C0 41 83 C0 1F 45 3B C3 45 0F 4F C3 44 89 45 C4 48 89 7D D8 0F 10 45 B0 0F 10 55 C0 0F 11 45 E0 0F 10 45 D8 0F 11 55 F0 66 0F 15 D2 F2 0F 11 55 D0 0F 10 4D E8 0F 11 45 B0 0F 11 4D C0 45 3B C3 7D ? 8B 4D CC 66 66 0F 1F 84 00 00 00 00 00 48 63 C1 45 33 C9 48 8D 14 40 48 8B 45 B0 48 8B 08 4C 8D 04 D1 48 8B CB 48 8D 55 10 E8 ? ? ? ? 8B 45 BC 48 8D 4D B8 F7 D0 21 45 C8 E8 ? ? ? ? 48 8B 45 C0 8B 4D CC 3B 48 18 7C ? 4C 8D 5C 24 70 48 8B C3 49 8B 5B 18 49 8B 7B 20 49 8B E3 5D C3 CC CC CC CC CC CC CC CC CC CC CC CC CC 8B 42 08");
            FModuleManager::RegisterImport("??0UNameProperty@@QEAA@AEBVFObjectInitializer@@W4ECppProperty@@H_K@Z", "40 53 48 83 EC 30 48 8B 44 24 60 48 8B D9 48 B9 00 02 00 40 10 00 08 00 45 33 C0 48 0B C1 48 8B CB 48 89 44 24 20 E8 ? ? ? ? 48 8D 05 ? ? ? ? C7 43 34 08 00 00 00 48 89 03 48 8B C3 48 83 C4 30 5B C3 CC CC CC CC CC CC CC CC CC CC CC 48 89 5C 24 08");
            FModuleManager::RegisterImport("??1UNameProperty@@UEAA@XZ", "48 89 5C 24 08 57 48 83 EC 20 89 D3 48 89 CF E8 ? ? ? ? F6 C3 01 74 ? BA 78 00 00 00 48 89 F9 E8 ? ? ? ? 48 89 F8 48 8B 5C 24 30 48 83 C4 20 5F C3 A1 5A 8C FC 4B 56 1A FF 5E");
            FModuleManager::RegisterImport("??2UArrayProperty@@SAPEAX_KW4EInternal@@PEAVUObject@@VFName@@W4EObjectFlags@@@Z", "4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20");
            FModuleManager::RegisterImport("??0UArrayProperty@@QEAA@AEBVFObjectInitializer@@W4ECppProperty@@H_K@Z", "40 53 48 83 EC 30 48 8B 44 24 60 45 33 C0 48 0F BA E8 09 48 8B D9 48 89 44 24 20 E8 ? ? ? ? 48 8D 05 ? ? ? ? C7 43 34 10 00 00 00 48 89 03 48 8B C3");
            FModuleManager::RegisterImport("??1UArrayProperty@@UEAA@XZ", "48 89 5C 24 08 57 48 83 EC 20 89 D3 48 89 CF E8 ? ? ? ? F6 C3 01 74 0D BA 80 00 00 00 48 89 F9 E8 ? ? ? ? 48 89  F8 48 8B 5C 24 30 48 83 C4 20 5F C3 2F AB 09 02 73 3E A5 CB F9 DD C6 01");
            FModuleManager::RegisterImport("??2UStructProperty@@SAPEAX_KW4EInternal@@PEAVUObject@@VFName@@W4EObjectFlags@@@Z", "4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 85 C9");
            FModuleManager::RegisterImport("??0UStructProperty@@QEAA@AEBVFObjectInitializer@@W4ECppProperty@@H_KPEAVUScriptStruct@@@Z", "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 48 83 EC 30 48 8B 7C 24 68");
            FModuleManager::RegisterImport("??1UStructProperty@@UEAA@XZ", "48 89 5C 24 08 57 48 83 EC 20 89 D3 48 89 CF E8 ? ? ? ? F6 C3 01 74 ? BA 80 00 00 00 48 89 F9 E8 ? ? ? ? 48 89 F8 48 8B 5C 24 30 48 83 C4 20 5F C3 91");
            FModuleManager::RegisterImport("??0DetermineBitfieldOffsetAndMask@@QEAA@XZ", "48 8D 05 ? ? ? ? 48 89 01 33 C0 48 89 41 08 48 8B C1 C3 CC CC CC CC CC CC CC CC CC CC CC CC 48 8B C4");
            FModuleManager::RegisterImport("?DoDetermineBitfieldOffsetAndMask@DetermineBitfieldOffsetAndMask@@QEAAX_K@Z", "48 89 5C 24 08 48 89 74 24 10 48 89 7C 24 18 41 56 48 83 EC 20 48 8B 3D ? ? ? ?");
            FModuleManager::RegisterImport("??2UEnumProperty@@SAPEAX_KW4EInternal@@PEAVUObject@@VFName@@W4EObjectFlags@@@Z", "4C 89 4C 24 20 53 48 83 EC 40 49 8B D8 E8 ? ? ? ? 44 8B 4C 24 70 33 C9 4C 8B 44 24 68 48 8B D3 48 89 4C 24 30 88 4C 24 28 89 4C 24 20 48 8B C8 E8 ? ? ? ? 48 83 C4 40 5B C3 CC CC CC CC 48 89 5C 24 08");
            FModuleManager::RegisterImport("??0UEnumProperty@@QEAA@AEBVFObjectInitializer@@W4ECppProperty@@H_KPEAVUEnum@@@Z", "40 53 48 83 EC 30 48 8B 44 24 60 48 8B D9 48 B9 00 00 00 00 00 00 08 00 45 33 C0 48 0B C1 48 8B CB 48 89 44 24 20 E8 ? ? ? ? 48 8D 05 ? ? ? ?");
            FModuleManager::RegisterImport("??1UEnumProperty@@UEAA@XZ", "48 89 5C 24 08 57 48 83 EC 20 89 D3 48 89 CF E8 ? ? ? ? F6 C3 01 74 ? BA 88 00 00 00 48 89 F9 E8 ? ? ? ? 48 89 F8 48 8B 5C 24 30 48 83 C4 20 5F C3 33 91 77 FE 04 9D");
            FModuleManager::RegisterImport("?StaticClass@UPackage@@SAPEAVUClass@@XZ", "4C 8B DC 48 83 EC 78 48 8B 05 ? ? ? ? 48 85 C0 0F 85 ? ? ? ? 88 44 24 68 4C 8D 0D ? ? ? ? 48 8D 05 ? ? ? ? 49 89 43 E8 4C 8D 05 ? ? ? ? 49 89 43 E0");
            FModuleManager::RegisterImport("?Z_Construct_UScriptStruct_FVector@@YAPEAVUScriptStruct@@XZ", "48 81 EC 48 06 00 00 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 38 06 00 00 C7 44 24 40 00 00 00 00 E8 ? ? ? ? 48 89 44 24 58 B8 18 00 00 00 8B C0 8B 0D ? ? ? ? 65 48 8B 14 25 58 00 00 00 48 8B 0C CA 8B 04 08 39 05 ? ? ? ? 7E ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 83 3D ? ? ? ? ? 75 ? E8 ? ? ? ? C6 44 24 20 00 44 8B C8 41 B8 0C 00 00 00 48 8D 15 ? ? ? ? 48 8B 4C 24 58 E8 ? ? ? ? 48 89 05 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 83 3D ? ? ? ? ? 0F 85 ? ? ? ? 48 8D 84 24 C0 00 00 00 48 89 84 24 A0 00 00 00 B8 01 00 00 00 83 C8 40 83 C8 04 89 44 24 44 41 B8 01 00 00 00 48 8D 15 ? ? ? ? 48 8B 8C 24 A0 00 00 00 E8 ? ? ? ? 8B 4C 24 44 89 4C 24 20 4C 8B 08 4C 8B 44 24 58 33 D2 B9 98 00 00 00 E8 ? ? ? ? 48 89 44 24 60 48 83 7C 24 60 00 74 ? 83 4C 24 40 01 48 8D 8C 24 F8 00 00 00 E8 ? ? ? ? 48 C7 44 24 30 04 00 00 00 48 C7 44 24 28 0C 00 00 00 C7 44 24 20 38 00 00 00 45 33 C9 45 33 C0 48 8B D0 48 8B 4C 24 60 E8 ? ? ? ? 48 89 44 24 68 EB ? 48 C7 44 24 68 00 00 00 00 48 8B 44 24 68 48 89 05 ? ? ? ? 8B 44 24 40 83 E0 01 85 C0 74 ? 83 64 24 40 FE 48 8D 8C 24 F8 00 00 00 E8 ? ? ? ? 48 8D 84 24 C8 00 00 00 48 89 84 24 A8 00 00 00 B8 01 00 00 00 83 C8 40 83 C8 04 89 44 24 48 41 B8 01 00 00 00 48 8D 15 ? ? ? ? 48 8B 8C 24 A8 00 00 00 E8 ? ? ? ? 8B 4C 24 48 89 4C 24 20 4C 8B 08 4C 8B 05 ? ? ? ? 33 D2 B9 78 00 00 00 E8 ? ? ? ? 48 89 44 24 70 48 83 7C 24 70 00 74 ? 83 4C 24 40 02 48 8D 8C 24 48 02 00 00 E8 ? ? ? ? 48 B9 05 00 00 01 00 00 10 00 48 89 4C 24 20 41 B9 08 00 00 00 45 33 C0 48 8B D0 48 8B 4C 24 70 E8 ? ? ? ? 48 89 44 24 78 EB ? 48 C7 44 24 78 00 00 00 00 48 8B 44 24 78 48 89 84 24 D0 00 00 00 8B 44 24 40 83 E0 02 85 C0 74 ? 83 64 24 40 FD 48 8D 8C 24 48 02 00 00 E8 ? ? ? ? 48 8D 84 24 D8 00 00 00 48 89 84 24 B0 00 00 00 B8 01 00 00 00 83 C8 40 83 C8 04 89 44 24 4C 41 B8 01 00 00 00 48 8D 15 ? ? ? ? 48 8B 8C 24 B0 00 00 00 E8 ? ? ? ? 8B 4C 24 4C 89 4C 24 20 4C 8B 08 4C 8B 05 ? ? ? ? 33 D2 B9 78 00 00 00 E8 ? ? ? ? 48 89 84 24 80 00 00 00 48 83 BC 24 80 00 00 00 00 74 ? 83 4C 24 40 04 48 8D 8C 24 98 03 00 00 E8 ? ? ? ? 48 B9 05 00 00 01 00 00 10 00 48 89 4C 24 20 41 B9 04 00 00 00 45 33 C0 48 8B D0 48 8B 8C 24 80 00 00 00 E8 ? ? ? ? 48 89 84 24 88 00 00 00 EB ? 48 C7 84 24 88 00 00 00 00 00 00 00 48 8B 84 24 88 00 00 00 48 89 84 24 E0 00 00 00 8B 44 24 40 83 E0 04 85 C0 74 ? 83 64 24 40 FB 48 8D 8C 24 98 03 00 00 E8 ? ? ? ? 48 8D 84 24 E8 00 00 00 48 89 84 24 B8 00 00 00 B8 01 00 00 00 83 C8 40 83 C8 04 89 44 24 50 41 B8 01 00 00 00 48 8D 15 ? ? ? ? 48 8B 8C 24 B8 00 00 00 E8 ? ? ? ? 8B 4C 24 50 89 4C 24 20 4C 8B 08 4C 8B 05 ? ? ? ? 33 D2 B9 78 00 00 00 E8 ? ? ? ? 48 89 84 24 90 00 00 00 48 83 BC 24 90 00 00 00 00 74 ? 83 4C 24 40 08 48 8D 8C 24 E8 04 00 00 E8 ? ? ? ? 48 B9 05 00 00 01 00 00 10 00 48 89 4C 24 20 45 33 C9 45 33 C0 48 8B D0 48 8B 8C 24 90 00 00 00 E8 ? ? ? ? 48 89 84 24 98 00 00 00 EB ? 48 C7 84 24 98 00 00 00 00 00 00 00 48 8B 84 24 98 00 00 00 48 89 84 24 F0 00 00 00 8B 44 24 40 83 E0 08 85 C0 74 ? 83 64 24 40 F7 48 8D 8C 24 E8 04 00 00 E8 ? ? ? ? 33 D2 48 8B 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 8B 8C 24 38 06 00 00 48 33 CC E8 ? ? ? ? 48 81 C4 48 06 00 00 C3 CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC 48 89 5C 24 08");
            FModuleManager::RegisterImport("?Z_Construct_UScriptStruct_FQuat@@YAPEAVUScriptStruct@@XZ", "48 81 EC C8 07 00 00 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 B0 07 00 00 C7 44 24 40 00 00 00 00 E8 ? ? ? ? 48 89 44 24 58 B8 18 00 00 00 8B C0 8B 0D ? ? ? ? 65 48 8B 14 25 58 00 00 00 48 8B 0C CA 8B 04 08 39 05 ? ? ? ? 7E ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 83 3D ? ? ? ? ? 75 ? E8 ? ? ? ? C6 44 24 20 00 44 8B C8 41 B8 10 00 00 00 48 8D 15 ? ? ? ? 48 8B 4C 24 58 E8 ? ? ? ? 48 89 05 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 83 3D ? ? ? ? ? 0F 85 ? ? ? ? 48 8D 84 24 D8 00 00 00 48 89 84 24 B0 00 00 00 B8 01 00 00 00 83 C8 40 83 C8 04 89 44 24 44 41 B8 01 00 00 00 48 8D 15 ? ? ? ? 48 8B 8C 24 B0 00 00 00 E8 ? ? ? ? 8B 4C 24 44 89 4C 24 20 4C 8B 08 4C 8B 44 24 58 33 D2 B9 98 00 00 00 E8 ? ? ? ? 48 89 44 24 60 48 83 7C 24 60 00 74 ? 83 4C 24 40 01 48 8D 8C 24 20 01 00 00 E8 ? ? ? ? 48 C7 44 24 30 10 00 00 00 48 C7 44 24 28 10 00 00 00 C7 44 24 20 38 00 00 00 45 33 C9 45 33 C0 48 8B D0 48 8B 4C 24 60 E8 ? ? ? ? 48 89 44 24 68 EB ? 48 C7 44 24 68 00 00 00 00 48 8B 44 24 68 48 89 05 ? ? ? ? 8B 44 24 40 83 E0 01 85 C0 74 ? 83 64 24 40 FE 48 8D 8C 24 20 01 00 00 E8 ? ? ? ? 48 8D 84 24 E0 00 00 00 48 89 84 24 B8 00 00 00 B8 01 00 00 00 83 C8 40 83 C8 04 89 44 24 48 41 B8 01 00 00 00 48 8D 15 ? ? ? ? 48 8B 8C 24 B8 00 00 00 E8 ? ? ? ? 8B 4C 24 48 89 4C 24 20 4C 8B 08 4C 8B 05 ? ? ? ? 33 D2 B9 78 00 00 00 E8 ? ? ? ? 48 89 44 24 70 48 83 7C 24 70 00 74 ? 83 4C 24 40 02 48 8D 8C 24 70 02 00 00 E8 ? ? ? ? 48 B9 05 00 00 01 00 00 10 00 48 89 4C 24 20 41 B9 0C 00 00 00 45 33 C0 48 8B D0 48 8B 4C 24 70 E8 ? ? ? ? 48 89 44 24 78 EB ? 48 C7 44 24 78 00 00 00 00 48 8B 44 24 78 48 89 84 24 E8 00 00 00 8B 44 24 40 83 E0 02 85 C0 74 ? 83 64 24 40 FD 48 8D 8C 24 70 02 00 00 E8 ? ? ? ? 48 8D 84 24 F0 00 00 00 48 89 84 24 C0 00 00 00 B8 01 00 00 00 83 C8 40 83 C8 04 89 44 24 4C 41 B8 01 00 00 00 48 8D 15 ? ? ? ? 48 8B 8C 24 C0 00 00 00 E8 ? ? ? ? 8B 4C 24 4C 89 4C 24 20 4C 8B 08 4C 8B 05 ? ? ? ? 33 D2 B9 78 00 00 00 E8 ? ? ? ? 48 89 84 24 80 00 00 00 48 83 BC 24 80 00 00 00 00 74 ? 83 4C 24 40 04 48 8D 8C 24 C0 03 00 00 E8 ? ? ? ? 48 B9 05 00 00 01 00 00 10 00 48 89 4C 24 20 41 B9 08 00 00 00 45 33 C0 48 8B D0 48 8B 8C 24 80 00 00 00 E8 ? ? ? ? 48 89 84 24 88 00 00 00 EB ? 48 C7 84 24 88 00 00 00 00 00 00 00 48 8B 84 24 88 00 00 00 48 89 84 24 F8 00 00 00 8B 44 24 40 83 E0 04 85 C0 74 ? 83 64 24 40 FB 48 8D 8C 24 C0 03 00 00 E8 ? ? ? ? 48 8D 84 24 00 01 00 00 48 89 84 24 C8 00 00 00 B8 01 00 00 00 83 C8 40 83 C8 04 89 44 24 50 41 B8 01 00 00 00 48 8D 15 ? ? ? ? 48 8B 8C 24 C8 00 00 00 E8 ? ? ? ? 8B 4C 24 50 89 4C 24 20 4C 8B 08 4C 8B 05 ? ? ? ? 33 D2 B9 78 00 00 00 E8 ? ? ? ? 48 89 84 24 90 00 00 00 48 83 BC 24 90 00 00 00 00 74 ? 83 4C 24 40 08 48 8D 8C 24 10 05 00 00 E8 ? ? ? ? 48 B9 05 00 00 01 00 00 10 00 48 89 4C 24 20 41 B9 04 00 00 00 45 33 C0 48 8B D0 48 8B 8C 24 90 00 00 00 E8 ? ? ? ? 48 89 84 24 98 00 00 00 EB ? 48 C7 84 24 98 00 00 00 00 00 00 00 48 8B 84 24 98 00 00 00 48 89 84 24 08 01 00 00 8B 44 24 40 83 E0 08 85 C0 74 ? 83 64 24 40 F7 48 8D 8C 24 10 05 00 00 E8 ? ? ? ? 48 8D 84 24 10 01 00 00 48 89 84 24 D0 00 00 00 B8 01 00 00 00 83 C8 40 83 C8 04 89 44 24 54 41 B8 01 00 00 00 48 8D 15 ? ? ? ? 48 8B 8C 24 D0 00 00 00 E8 ? ? ? ? 8B 4C 24 54 89 4C 24 20 4C 8B 08 4C 8B 05 ? ? ? ? 33 D2 B9 78 00 00 00 E8 ? ? ? ? 48 89 84 24 A0 00 00 00 48 83 BC 24 A0 00 00 00 00 74 ? 83 4C 24 40 10 48 8D 8C 24 60 06 00 00 E8 ? ? ? ? 48 B9 05 00 00 01 00 00 10 00 48 89 4C 24 20 45 33 C9 45 33 C0 48 8B D0 48 8B 8C 24 A0 00 00 00 E8 ? ? ? ? 48 89 84 24 A8 00 00 00 EB ? 48 C7 84 24 A8 00 00 00 00 00 00 00 48 8B 84 24 A8 00 00 00 48 89 84 24 18 01 00 00 8B 44 24 40 83 E0 10 85 C0 74 ? 83 64 24 40 EF 48 8D 8C 24 60 06 00 00 E8 ? ? ? ? 33 D2 48 8B 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 8B 8C 24 B0 07 00 00 48 33 CC E8 ? ? ? ? 48 81 C4 C8 07 00 00 C3 CC CC CC CC CC CC CC CC 48 81 EC C8 04 00 00");
            FModuleManager::RegisterImport("?Z_Construct_UScriptStruct_FPlane@@YAPEAVUScriptStruct@@XZ", "48 81 EC 58 03 00 00 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 48 03 00 00 C7 44 24 40 00 00 00 00 E8 ? ? ? ? 48 89 44 24 50 B8 18 00 00 00 8B C0 8B 0D ? ? ? ? 65 48 8B 14 25 58 00 00 00 48 8B 0C CA 8B 04 08 39 05 ? ? ? ? 7E ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 83 3D ? ? ? ? ? 75 ? E8 ? ? ? ? C6 44 24 20 00 44 8B C8 41 B8 10 00 00 00");
            FModuleManager::RegisterImport("?Z_Construct_UScriptStruct_FGuid@@YAPEAVUScriptStruct@@XZ", "48 81 EC C8 07 00 00 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 B0 07 00 00 C7 44 24 40 00 00 00 00 E8 ? ? ? ? 48 89 44 24 58 B8 18 00 00 00 8B C0 8B 0D ? ? ? ? 65 48 8B 14 25 58 00 00 00 48 8B 0C CA 8B 04 08 39 05 ? ? ? ? 7E ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 83 3D ? ? ? ? ? 75 ? E8 ? ? ? ? C6 44 24 20 00 44 8B C8 41 B8 10 00 00 00 48 8D 15 ? ? ? ? 48 8B 4C 24 58 E8 ? ? ? ? 48 89 05 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 83 3D ? ? ? ? ? 0F 85 ? ? ? ? 48 8D 84 24 D8 00 00 00 48 89 84 24 B0 00 00 00 B8 01 00 00 00 83 C8 40 83 C8 04 89 44 24 44 41 B8 01 00 00 00 48 8D 15 ? ? ? ? 48 8B 8C 24 B0 00 00 00 E8 ? ? ? ? 8B 4C 24 44 89 4C 24 20 4C 8B 08 4C 8B 44 24 58 33 D2 B9 98 00 00 00 E8 ? ? ? ? 48 89 44 24 60 48 83 7C 24 60 00 74 ? 83 4C 24 40 01 48 8D 8C 24 20 01 00 00 E8 ? ? ? ? 48 C7 44 24 30 04 00 00 00 48 C7 44 24 28 10 00 00 00 C7 44 24 20 38 00 00 00 45 33 C9 45 33 C0 48 8B D0 48 8B 4C 24 60 E8 ? ? ? ? 48 89 44 24 68 EB ? 48 C7 44 24 68 00 00 00 00 48 8B 44 24 68 48 89 05 ? ? ? ? 8B 44 24 40 83 E0 01 85 C0 74 ? 83 64 24 40 FE 48 8D 8C 24 20 01 00 00 E8 ? ? ? ? 48 8D 84 24 E0 00 00 00 48 89 84 24 B8 00 00 00 B8 01 00 00 00 83 C8 40 83 C8 04 89 44 24 48 41 B8 01 00 00 00 48 8D 15 ? ? ? ? 48 8B 8C 24 B8 00 00 00 E8 ? ? ? ? 8B 4C 24 48 89 4C 24 20 4C 8B 08 4C 8B 05 ? ? ? ? 33 D2 B9 78 00 00 00 E8 ? ? ? ? 48 89 44 24 70 48 83 7C 24 70 00 74 ? 83 4C 24 40 02 48 8D 8C 24 70 02 00 00 E8 ? ? ? ? 48 B9 01 00 00 01 00 00 10 00");
            FModuleManager::RegisterImport("?Z_Construct_UScriptStruct_FRotator@@YAPEAVUScriptStruct@@XZ", "48 81 EC 48 06 00 00 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 38 06 00 00 C7 44 24 40 00 00 00 00 E8 ? ? ? ? 48 89 44 24 58 B8 18 00 00 00 8B C0 8B 0D ? ? ? ? 65 48 8B 14 25 58 00 00 00 48 8B 0C CA 8B 04 08 39 05 ? ? ? ? 7E ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 83 3D ? ? ? ? ? 75 ? E8 ? ? ? ? C6 44 24 20 00 44 8B C8 41 B8 0C 00 00 00 48 8D 15 ? ? ? ? 48 8B 4C 24 58 E8 ? ? ? ? 48 89 05 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 83 3D ? ? ? ? ? 0F 85 ? ? ? ? 48 8D 84 24 C0 00 00 00 48 89 84 24 A0 00 00 00 B8 01 00 00 00 83 C8 40 83 C8 04 89 44 24 44 41 B8 01 00 00 00 48 8D 15 ? ? ? ? 48 8B 8C 24 A0 00 00 00 E8 ? ? ? ? 8B 4C 24 44 89 4C 24 20 4C 8B 08 4C 8B 44 24 58 33 D2 B9 98 00 00 00 E8 ? ? ? ? 48 89 44 24 60 48 83 7C 24 60 00 74 ? 83 4C 24 40 01 48 8D 8C 24 F8 00 00 00 E8 ? ? ? ? 48 C7 44 24 30 04 00 00 00 48 C7 44 24 28 0C 00 00 00 C7 44 24 20 38 00 00 00 45 33 C9 45 33 C0 48 8B D0 48 8B 4C 24 60 E8 ? ? ? ? 48 89 44 24 68 EB ? 48 C7 44 24 68 00 00 00 00 48 8B 44 24 68 48 89 05 ? ? ? ? 8B 44 24 40 83 E0 01 85 C0 74 ? 83 64 24 40 FE 48 8D 8C 24 F8 00 00 00 E8 ? ? ? ? 48 8D 84 24 C8 00 00 00 48 89 84 24 A8 00 00 00 B8 01 00 00 00 83 C8 40 83 C8 04 89 44 24 48 41 B8 01 00 00 00 48 8D 15 ? ? ? ? 48 8B 8C 24 A8 00 00 00 E8 ? ? ? ? 8B 4C 24 48 89 4C 24 20 4C 8B 08 4C 8B 05 ? ? ? ? 33 D2 B9 78 00 00 00 E8 ? ? ? ? 48 89 44 24 70 48 83 7C 24 70 00 74 ? 83 4C 24 40 02 48 8D 8C 24 48 02 00 00 E8 ? ? ? ? 48 B9 05 00 00 01 00 00 10 00 48 89 4C 24 20 41 B9 08 00 00 00 45 33 C0 48 8B D0 48 8B 4C 24 70 E8 ? ? ? ? 48 89 44 24 78 EB ? 48 C7 44 24 78 00 00 00 00 48 8B 44 24 78 48 89 84 24 D0 00 00 00 8B 44 24 40 83 E0 02 85 C0 74 ? 83 64 24 40 FD 48 8D 8C 24 48 02 00 00 E8 ? ? ? ? 48 8D 84 24 D8 00 00 00 48 89 84 24 B0 00 00 00 B8 01 00 00 00 83 C8 40 83 C8 04 89 44 24 4C 41 B8 01 00 00 00 48 8D 15 ? ? ? ? 48 8B 8C 24 B0 00 00 00 E8 ? ? ? ? 8B 4C 24 4C 89 4C 24 20 4C 8B 08 4C 8B 05 ? ? ? ? 33 D2 B9 78 00 00 00 E8 ? ? ? ? 48 89 84 24 80 00 00 00 48 83 BC 24 80 00 00 00 00 74 ? 83 4C 24 40 04 48 8D 8C 24 98 03 00 00 E8 ? ? ? ? 48 B9 05 00 00 01 00 00 10 00 48 89 4C 24 20 41 B9 04 00 00 00 45 33 C0 48 8B D0 48 8B 8C 24 80 00 00 00 E8 ? ? ? ? 48 89 84 24 88 00 00 00 EB ? 48 C7 84 24 88 00 00 00 00 00 00 00 48 8B 84 24 88 00 00 00 48 89 84 24 E0 00 00 00 8B 44 24 40 83 E0 04 85 C0 74 ? 83 64 24 40 FB 48 8D 8C 24 98 03 00 00 E8 ? ? ? ? 48 8D 84 24 E8 00 00 00 48 89 84 24 B8 00 00 00 B8 01 00 00 00 83 C8 40 83 C8 04 89 44 24 50 41 B8 01 00 00 00 48 8D 15 ? ? ? ? 48 8B 8C 24 B8 00 00 00 E8 ? ? ? ? 8B 4C 24 50 89 4C 24 20 4C 8B 08 4C 8B 05 ? ? ? ? 33 D2 B9 78 00 00 00 E8 ? ? ? ? 48 89 84 24 90 00 00 00 48 83 BC 24 90 00 00 00 00 74 ? 83 4C 24 40 08 48 8D 8C 24 E8 04 00 00 E8 ? ? ? ? 48 B9 05 00 00 01 00 00 10 00 48 89 4C 24 20 45 33 C9 45 33 C0 48 8B D0 48 8B 8C 24 90 00 00 00 E8 ? ? ? ? 48 89 84 24 98 00 00 00 EB ? 48 C7 84 24 98 00 00 00 00 00 00 00 48 8B 84 24 98 00 00 00 48 89 84 24 F0 00 00 00 8B 44 24 40 83 E0 08 85 C0 74 ? 83 64 24 40 F7 48 8D 8C 24 E8 04 00 00 E8 ? ? ? ? 33 D2 48 8B 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 8B 8C 24 38 06 00 00 48 33 CC E8 ? ? ? ? 48 81 C4 48 06 00 00 C3 CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC 48 81 EC 58 03 00 00");
            FModuleManager::RegisterImport("?AddCppProperty@UArrayProperty@@UEAAXPEAVUProperty@@@Z", "48 89 51 78 C3 CC");
            FModuleManager::RegisterImport("?AddCppProperty@UEnumProperty@@UEAAXPEAVUProperty@@@Z", "48 89 51 78 48 85 D2");
            FModuleManager::RegisterImport("?AddCppProperty@UField@@UEAAXPEAVUProperty@@@Z", "48 83 EC 28 4C 8D 05 ? ? ? ? BA BC 00 00 00");
            FModuleManager::RegisterImport("?AllowCrossLevel@UObjectPropertyBase@@MEBA_NXZ", NullSub);
            FModuleManager::RegisterImport("?BeginDestroy@UEnum@@UEAAXXZ", "40 53 48 83 EC 20 48 8B D9 E8 ? ? ? ? 48 8B CB 48 83 C4 20 5B E9 ? ? ? ? CC CC CC CC CC 40 53 48 83 EC 20 48 8B D9 E8 ? ? ? ? 48 8D 8B 58 02 00 00");
            FModuleManager::RegisterImport("?BeginDestroy@UObjectPropertyBase@@UEAAXXZ", "48 89 5C 24 08 57 48 83 EC 20 48 8B 79 78 48 8B D9 48 85 FF 74 ? E8 ? ? ? ? 48 8B 57 10 4C 8D 80 88 00 00 00 49 63 40 08 3B 82 90 00 00 00 7F ? 48 8B C8 48 8B 82 88 00 00 00 4C 39 04 C8 75 ? 48 8D 8F 08 03 00 00");
            FModuleManager::RegisterImport("?Bind@UField@@UEAAXXZ", VoidSub);
            FModuleManager::RegisterImport("?BeginDestroy@UObjectPropertyBase@@UEAAXXZ", VoidSub);
            FModuleManager::RegisterImport("?CanHoldDoubleValueInternal@?$TProperty_Numeric@E@@EEBA_NN@Z", "F2 48 0F 2C C1 0F B6 C8");
            FModuleManager::RegisterImport("?CanHoldDoubleValueInternal@?$TProperty_Numeric@H@@EEBA_NN@Z", "F2 0F 2C C1 66 0F 6E C0");
            FModuleManager::RegisterImport("?CanHoldDoubleValueInternal@?$TProperty_Numeric@M@@EEBA_NN@Z", "0F 57 C0 F2 0F 5A C1 0F 5A D0");
            FModuleManager::RegisterImport("?CanHoldSignedValueInternal@?$TProperty_Numeric@E@@EEBA_N_J@Z", "0F B6 CA 48 3B CA");
            FModuleManager::RegisterImport("?CanHoldSignedValueInternal@?$TProperty_Numeric@H@@EEBA_N_J@Z", "48 63 CA 48 3B CA");
            FModuleManager::RegisterImport("?CanHoldSignedValueInternal@?$TProperty_Numeric@M@@EEBA_N_J@Z", "0F 57 C0 F3 48 0F 2A C2 F3 48 0F 2C C0");
            FModuleManager::RegisterImport("?CanHoldUnsignedValueInternal@?$TProperty_Numeric@E@@EEBA_N_K@Z", "0F B6 CA 48 3B CA");
            FModuleManager::RegisterImport("?CanHoldUnsignedValueInternal@?$TProperty_Numeric@H@@EEBA_N_K@Z", "48 63 CA 48 3B CA");
            FModuleManager::RegisterImport("?CanHoldUnsignedValueInternal@?$TProperty_Numeric@M@@EEBA_N_K@Z", "0F 57 C0 F3 48 0F 2A C2 F3 48 0F 2C C0");
            FModuleManager::RegisterImport("?CheckValidObject@UObjectPropertyBase@@MEBAXPEAX@Z", "48 89 74 24 18 48 89 7C 24 20 41 56 48 83 EC 20 48 8B 01 4C 8B F2");
            FModuleManager::RegisterImport("?ClearValueInternal@?$TProperty@EVUNumericProperty@@@@UEBAXPEAX@Z", "C6 02 00 C3 CC CC CC CC CC CC CC CC CC CC CC CC C7 02 00 00 00 00");
            FModuleManager::RegisterImport("?ClearValueInternal@?$TProperty@HVUNumericProperty@@@@UEBAXPEAX@Z", "C7 02 00 00 00 00 C3 CC CC CC CC CC CC CC CC CC 48 C7 02 00 00 00 00");
            FModuleManager::RegisterImport("?ClearValueInternal@?$TProperty@MVUNumericProperty@@@@UEBAXPEAX@Z", "C7 02 00 00 00 00 C3 CC CC CC CC CC CC CC CC CC 48 C7 02 00 00 00 00");
            FModuleManager::RegisterImport("?ClearValueInternal@?$TProperty@PEAVUObject@@VUObjectPropertyBase@@@@UEBAXPEAX@Z", "48 C7 02 00 00 00 00 C3 CC CC CC CC CC CC CC CC E9 ? ? ? ? CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC");
            FModuleManager::RegisterImport("?ClearValueInternal@?$TProperty@VFName@@VUProperty@@@@UEBAXPEAX@Z", "33 C0 48 89 44 24 10 48 89 02 C3");
            FModuleManager::RegisterImport("?ClearValueInternal@UArrayProperty@@UEBAXPEAX@Z", "40 57 41 56 41 57 48 83 EC 20 48 8B 79 78");
            FModuleManager::RegisterImport("?ClearValueInternal@UBoolProperty@@UEBAXPEAX@Z", "0F B6 41 7B 44 0F B6 41 79");
            FModuleManager::RegisterImport("?ClearValueInternal@UProperty@@MEBAXPEAX@Z", VoidSub);
            FModuleManager::RegisterImport("?ClearValueInternal@UStructProperty@@UEBAXPEAX@Z", "48 8B 49 78 41 B8 01 00 00 00");
            FModuleManager::RegisterImport("?ContainsObjectReference@?$TUObjectPropertyBase@PEAVUObject@@@@UEBA_NAEAV?$TArray@PEBVUStructProperty@@VFDefaultAllocator@@@@@Z", TrueSub);
            FModuleManager::RegisterImport("?ContainsObjectReference@UArrayProperty@@UEBA_NAEAV?$TArray@PEBVUStructProperty@@VFDefaultAllocator@@@@@Z", "48 8B 49 78 48 8B 01 48 FF A0 F8 02 00 00");
            FModuleManager::RegisterImport("?ContainsObjectReference@UProperty@@UEBA_NAEAV?$TArray@PEBVUStructProperty@@VFDefaultAllocator@@@@@Z", NullSub);
            FModuleManager::RegisterImport("?ContainsObjectReference@UStructProperty@@UEBA_NAEAV?$TArray@PEBVUStructProperty@@VFDefaultAllocator@@@@@Z", "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 4C 8B 02 48 8B F1");
            FModuleManager::RegisterImport("?ContainsWeakObjectReference@?$TUObjectPropertyBase@PEAVUObject@@@@UEBA_NXZ", NullSub);
            FModuleManager::RegisterImport("?ContainsWeakObjectReference@?$TUObjectPropertyBase@PEAVUObject@@@@UEBA_NXZ", "48 8B 49 78 48 8B 01 48 FF A0 00 03 00 00");
            FModuleManager::RegisterImport("?ContainsWeakObjectReference@UArrayProperty@@UEBA_NXZ", "48 8B 49 78 48 8B 01 48 FF A0 00 03 00 00");
            FModuleManager::RegisterImport("?ContainsWeakObjectReference@UProperty@@UEBA_NXZ", NullSub);
            FModuleManager::RegisterImport("?ContainsWeakObjectReference@UStructProperty@@UEBA_NXZ", "40 57 48 83 EC 20 65 48 8B 04 25 58 00 00 00 48 8B F9 8B 15 ? ? ? ? B9 18 00 00 00 48 8B 14 D0 8B 04 11 39 05 ? ? ? ? 0F 8F ? ? ? ?");
            FModuleManager::RegisterImport("?ConvertFromType@?$TProperty_Numeric@H@@UEAA_NAEBUFPropertyTag@@AEAVFArchive@@PEAEPEAVUStruct@@AEA_N@Z", "48 89 5C 24 08 48 89 74 24 18 57 48 83 EC 20 48 8B 02 48 8B F9 33 C9 48 C7 44 24 38 1A 00 00 00 49 8B F1 4D 8B D0 48 8B DA 48 3B 44 24 38 75 ? 4C 8B CB 4C 8B C6 49 8B D2 48 8B CF E8 ? ? ? ? B0 01 48 8B 5C 24 30 48 8B 74 24 40 48 83 C4 20 5F C3 48 C7 44 24 38 19 00 00 00 48 3B 44 24 38 75 ? 4C 8B CB 4C 8B C6 49 8B D2 48 8B CF E8 ? ? ? ? B0 01 48 8B 5C 24 30 48 8B 74 24 40 48 83 C4 20 5F C3 48 C7 44 24 38 02 00 00 00 48 3B 44 24 38 75 ? 4C 8B CB 4C 8B C6 49 8B D2 48 8B CF E8 ? ? ? ? B0 01 48 8B 5C 24 30 48 8B 74 24 40 48 83 C4 20 5F C3 48 C7 44 24 38 17 00 00 00 48 3B 44 24 38 75 ? 4C 8B CB 4C 8B C6 49 8B D2 48 8B CF E8 ? ? ? ? B0 01 48 8B 5C 24 30 48 8B 74 24 40 48 83 C4 20 5F C3 48 C7 44 24 38 01 00 00 00 48 3B 44 24 38 75 ? 48 8B C1 48 89 4C 24 38 48 39 4A 20 0F 84 ? ? ? ? 48 8B 54 24 50 4C 8B C3 49 8B CA E8 ? ? ? ? 48 63 57 50 44 0F B6 C0 8B 43 3C 0F AF 47 34 48 98 48 03 C6 44 89 04 02 B0 01 48 8B 5C 24 30 48 8B 74 24 40 48 83 C4 20 5F C3 48 C7 44 24 38 22 00 00 00 48 3B 44 24 38 75 ? 48 8B 54 24 50 4C 8B C3 49 8B CA E8 ? ? ? ? 48 63 57 50 44 0F B6 C0 8B 43 3C 0F AF 47 34 48 98 48 03 C6 44 89 04 02 B0 01 48 8B 5C 24 30 48 8B 74 24 40 48 83 C4 20 5F C3 48 C7 44 24 38 16 00 00 00 48 3B 44 24 38 75 ? 4C 8B CB 4C 8B C6 49 8B D2 48 8B CF E8 ? ? ? ? B0 01 48 8B 5C 24 30 48 8B 74 24 40 48 83 C4 20 5F C3 48 C7 44 24 38 15 00 00 00 48 3B 44 24 38 75 ? 4C 8B CB 4C 8B C6 49 8B D2 48 8B CF E8 ? ? ? ? B0 01 48 8B 5C 24 30 48 8B 74 24 40 48 83 C4 20 5F C3 48 C7 44 24 38 14 00 00 00 48 3B 44 24 38 75 ? 4C 8B CB 4C 8B C6 49 8B D2 48 8B CF E8 ? ? ? ? B0 01 48 8B 5C 24 30 48 8B 74 24 40 48 83 C4 20 5F C3 48 8B 5C 24 30 32 C0 48 8B 74 24 40 48 83 C4 20 5F C3 CC CC CC 48 89 5C 24 08");
            FModuleManager::RegisterImport("?ConvertFromType@?$TProperty_Numeric@M@@UEAA_NAEBUFPropertyTag@@AEAVFArchive@@PEAEPEAVUStruct@@AEA_N@Z", "48 89 5C 24 08 48 89 74 24 18 57 48 83 EC 20 48 8B 02 48 8B F9 33 C9 48 C7 44 24 38 1A 00 00 00 49 8B F1 4D 8B D0 48 8B DA 48 3B 44 24 38 75 ? 4C 8B CB 4C 8B C6 49 8B D2 48 8B CF E8 ? ? ? ? B0 01 48 8B 5C 24 30 48 8B 74 24 40 48 83 C4 20 5F C3 48 C7 44 24 38 19 00 00 00 48 3B 44 24 38 75 ? 4C 8B CB 4C 8B C6 49 8B D2 48 8B CF E8 ? ? ? ? B0 01 48 8B 5C 24 30 48 8B 74 24 40 48 83 C4 20 5F C3 48 C7 44 24 38 02 00 00 00 48 3B 44 24 38 75 ? 4C 8B CB 4C 8B C6 49 8B D2 48 8B CF E8 ? ? ? ? B0 01 48 8B 5C 24 30 48 8B 74 24 40 48 83 C4 20 5F C3 48 C7 44 24 38 17 00 00 00 48 3B 44 24 38 75 ? 4C 8B CB 4C 8B C6 49 8B D2 48 8B CF E8 ? ? ? ? B0 01 48 8B 5C 24 30 48 8B 74 24 40 48 83 C4 20 5F C3 48 C7 44 24 38 01 00 00 00 48 3B 44 24 38 75 ? 48 8B C1 48 89 4C 24 38 48 39 4A 20 0F 84 ? ? ? ? 48 8B 54 24 50 4C 8B C3 49 8B CA E8 ? ? ? ? 48 63 57 50 0F B6 C0 66 0F 6E C0 8B 43 3C 0F AF 47 34 0F 5B C0");
            FModuleManager::RegisterImport("?ConvertFromType@UArrayProperty@@UEAA_NAEBUFPropertyTag@@AEAVFArchive@@PEAEPEAVUStruct@@AEA_N@Z", "48 89 6C 24 20 56 57 41 54 41 55 41 56 48 81 EC C0 00 00 00");
            FModuleManager::RegisterImport("?ConvertFromType@UBoolProperty@@UEAA_NAEBUFPropertyTag@@AEAVFArchive@@PEAEPEAVUStruct@@AEA_N@Z", "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 41 56 41 57 48 83 EC 20 4C 8B 74 24 68");
            FModuleManager::RegisterImport("?ConvertFromType@UByteProperty@@UEAA_NAEBUFPropertyTag@@AEAVFArchive@@PEAEPEAVUStruct@@AEA_N@Z", "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 41 56 41 57 48 83 EC 30 4C 8B 7C 24 78");
            FModuleManager::RegisterImport("?ConvertFromType@UEnumProperty@@UEAA_NAEBUFPropertyTag@@AEAVFArchive@@PEAEPEAVUStruct@@AEA_N@Z", "4C 8B DC 53 56 57 41 56 48 81 EC A8 00 00 00");
            FModuleManager::RegisterImport("?ConvertFromType@UNameProperty@@UEAA_NAEBUFPropertyTag@@AEAVFArchive@@PEAEPEAVUStruct@@AEA_N@Z", "48 89 5C 24 08 48 89 74 24 18 48 89 7C 24 20 41 56 48 83 EC 60 48 8B 02");
            FModuleManager::RegisterImport("?ConvertFromType@UObjectProperty@@UEAA_NAEBUFPropertyTag@@AEAVFArchive@@PEAEPEAVUStruct@@AEA_N@Z", "48 89 5C 24 08 48 89 6C 24 18 56 57 41 56 48 83 EC 60 48 8B 02");
            FModuleManager::RegisterImport("?ConvertFromType@UStructProperty@@UEAA_NAEBUFPropertyTag@@AEAVFArchive@@PEAEPEAVUStruct@@AEA_N@Z", "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 41 56 41 57 48 83 EC 30 48 8B 74 24 ?");
            FModuleManager::RegisterImport("?CopyCompleteValueFromScriptVM@UObjectPropertyBase@@UEBAXPEAXPEBX@Z", "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 48 83 EC 20 31 FF 4C 89 C6");
            FModuleManager::RegisterImport("?CopyCompleteValueFromScriptVM@UProperty@@UEBAXPEAXPEBX@Z", "4D 8B D0 4C 8B DA 49 3B D0 74 ? 8B 41 38 48 0F BA E0 1E 73 ? 8B 41 34");
            FModuleManager::RegisterImport("?CopyCompleteValueToScriptVM@UObjectPropertyBase@@UEBAXPEAXPEBX@Z", "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 48 83 EC 20 31 FF 4C 89 C5");
            FModuleManager::RegisterImport("?CopyCompleteValueToScriptVM@UProperty@@UEBAXPEAXPEBX@Z", "4D 8B D0 4C 8B DA 49 3B D0 74 ? 8B 41 38 48 0F BA E0 1E 73 ? 8B 41 34");
            FModuleManager::RegisterImport("?CopySingleValueFromScriptVM@UObjectPropertyBase@@UEBAXPEAXPEBX@Z", "48 8B 01 4D 8B 00 48 FF A0 28 03 00 00");
            FModuleManager::RegisterImport("?CopySingleValueFromScriptVM@UProperty@@UEBAXPEAXPEBX@Z", "4D 8B D0 4C 8B DA 49 3B D0 74 ? 8B 41 38 48 0F BA E0 1E 73 ? 4C 63 41 34");
            FModuleManager::RegisterImport("?CopySingleValueToScriptVM@UObjectPropertyBase@@UEBAXPEAXPEBX@Z", "53 48 83 EC 20 48 8B 01 48 89 D3 4C 89 C2 FF 90 20 03 00 00");
            FModuleManager::RegisterImport("?CopySingleValueToScriptVM@UProperty@@UEBAXPEAXPEBX@Z", "4D 8B D0 4C 8B DA 49 3B D0 74 ? 8B 41 38 48 0F BA E0 1E 73 ? 4C 63 41 34");
            FModuleManager::RegisterImport("?CopyValuesInternal@?$TProperty@EVUNumericProperty@@@@UEBAXPEAXPEBXH@Z", "49 63 C9 45 85 C9 7E ? 49 29 D0 0F 1F 44 00 00 41 0F B6 04 10 88 02 48 8D 52 01 48 83 E9 01 75 ? C3 2B 36");
            FModuleManager::RegisterImport("?CopyValuesInternal@?$TProperty@HVUNumericProperty@@@@UEBAXPEAXPEBXH@Z", "49 63 C9 45 85 C9 7E ? 49 29 D0 0F 1F 44 00 00 41 8B 04 10 89 02 48 8D 52 04 48 83 E9 01 75 ? C3 8D 07");
            FModuleManager::RegisterImport("?CopyValuesInternal@?$TProperty@MVUNumericProperty@@@@UEBAXPEAXPEBXH@Z", "48 89 5C 24 08 4D 63 D1 45 31 DB 48 89 D3 49 83 FA 04 7C ? 48 8D 4A 04");
            FModuleManager::RegisterImport("?CopyValuesInternal@?$TProperty@PEAVUObject@@VUObjectPropertyBase@@@@UEBAXPEAXPEBXH@Z", "49 63 C9 45 85 C9 7E ? 49 29 D0 0F 1F 44 00 00 49 8B 04 10 48 89 02 48 8D 52 08 48 83 E9 01 75 ? C3 FB");
            FModuleManager::RegisterImport("?CopyValuesInternal@?$TProperty@VFName@@VUProperty@@@@UEBAXPEAXPEBXH@Z", "49 63 C9 45 85 C9 7E ? 49 29 D0 0F 1F 44 00 00 49 8B 04 10 48 89 02 48 8D 52 08 48 83 E9 01 75 ? C3 FB");
            FModuleManager::RegisterImport("?CopyValuesInternal@UArrayProperty@@UEBAXPEAXPEBXH@Z", "48 8B C4 48 89 58 20 4C 89 40 18 48 89 48 08");
            FModuleManager::RegisterImport("?CopyValuesInternal@UBoolProperty@@UEBAXPEAXPEBXH@Z", "48 89 5C 24 08 45 31 D2 48 89 D3");
            FModuleManager::RegisterImport("?CopyValuesInternal@UProperty@@MEBAXPEAXPEBXH@Z", VoidSub);
            FModuleManager::RegisterImport("?CopyValuesInternal@UStructProperty@@UEBAXPEAXPEBXH@Z", "48 8B 49 78 E9 ? ? ? ? CC CC CC CC CC CC CC 48 8B C4");
            FModuleManager::RegisterImport("?DestroyValueInternal@?$TProperty@EVUNumericProperty@@@@UEBAXPEAX@Z", VoidSub);
            FModuleManager::RegisterImport("?DestroyValueInternal@?$TProperty@HVUNumericProperty@@@@UEBAXPEAX@Z", VoidSub);
            FModuleManager::RegisterImport("?DestroyValueInternal@?$TProperty@MVUNumericProperty@@@@UEBAXPEAX@Z", VoidSub);
            FModuleManager::RegisterImport("?DestroyValueInternal@?$TProperty@PEAVUObject@@VUObjectPropertyBase@@@@UEBAXPEAX@Z", VoidSub);
            FModuleManager::RegisterImport("?DestroyValueInternal@?$TProperty@VFName@@VUProperty@@@@UEBAXPEAX@Z", VoidSub);
            FModuleManager::RegisterImport("?DestroyValueInternal@UArrayProperty@@UEBAXPEAX@Z", "48 83 EC 38 8B 42 08 48 89 74 24 50");
            FModuleManager::RegisterImport("?DestroyValueInternal@UProperty@@MEBAXPEAX@Z", VoidSub);
            FModuleManager::RegisterImport("?DestroyValueInternal@UStructProperty@@UEBAXPEAX@Z", "48 8B 41 78 44 8B 41 30 48 8B C8 4C 8B 08 49 FF A1 68 02 00 00");
            FModuleManager::RegisterImport("?EmitReferenceInfo@UArrayProperty@@UEAAXAEAVUClass@@HAEAV?$TArray@PEBVUStructProperty@@VFDefaultAllocator@@@@@Z", "48 89 6C 24 20 57 41 54 41 56 48 83 EC 50 48 8B F9");
            FModuleManager::RegisterImport("?EmitReferenceInfo@UObjectProperty@@UEAAXAEAVUClass@@HAEAV?$TArray@PEBVUStructProperty@@VFDefaultAllocator@@@@@Z", "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 40 44 8B 49 30");
            FModuleManager::RegisterImport("?EmitReferenceInfo@UProperty@@UEAAXAEAVUClass@@HAEAV?$TArray@PEBVUStructProperty@@VFDefaultAllocator@@@@@Z", VoidSub);
            FModuleManager::RegisterImport("?EmitReferenceInfo@UStructProperty@@UEAAXAEAVUClass@@HAEAV?$TArray@PEBVUStructProperty@@VFDefaultAllocator@@@@@Z", "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 48 89 7C 24 20 41 56 48 83 EC 40 48 8B 41 78");
            FModuleManager::RegisterImport("?ExportTextItem@UArrayProperty@@UEBAXAEAVFString@@PEBX1PEAVUObject@@H2@Z", "4C 8B DC 4D 89 4B 20 4D 89 43 18 49 89 4B 08 53 55 56 57 41 56");
            FModuleManager::RegisterImport("?ExportTextItem@UBoolProperty@@UEBAXAEAVFString@@PEBX1PEAVUObject@@H2@Z", "56 48 83 EC 30 0F B6 41 79");
            FModuleManager::RegisterImport("?ExportTextItem@UByteProperty@@UEBAXAEAVFString@@PEBX1PEAVUObject@@H2@Z", "48 8B C4 48 89 58 10 48 89 70 18 55 57 41 54 41 56 41 57 48 8D 68 C8");
            FModuleManager::RegisterImport("?ExportTextItem@UEnumProperty@@UEBAXAEAVFString@@PEBX1PEAVUObject@@H2@Z", "48 89 5C 24 10 48 89 74 24 18 48 89 7C 24 20 55 41 54 41 55 41 56 41 57 48 8D 6C 24 E1 48 81 EC 00 01 00 00");
            FModuleManager::RegisterImport("?ExportTextItem@UFloatProperty@@UEBAXAEAVFString@@PEBX1PEAVUObject@@H2@Z", "48 89 5C 24 08 57 48 83 EC 40 48 8B 84 24 80 00 00 00");
            FModuleManager::RegisterImport("?ExportTextItem@UNameProperty@@UEBAXAEAVFString@@PEBX1PEAVUObject@@H2@Z", "4C 8B DC 49 89 5B 20 55 56 57 41 55 41 56 48 8B EC");
            FModuleManager::RegisterImport("?ExportTextItem@UNumericProperty@@UEBAXAEAVFString@@PEBX1PEAVUObject@@H2@Z", "48 83 EC 38 48 8B 01 48 89 5C 24 40");
            FModuleManager::RegisterImport("?ExportTextItem@UObjectPropertyBase@@UEBAXAEAVFString@@PEBX1PEAVUObject@@H2@Z", "48 89 5C 24 18 48 89 74 24 20 55 57 41 55 41 56 41 57 48 8D 6C 24 E1");
            FModuleManager::RegisterImport("?ExportTextItem@UStructProperty@@UEBAXAEAVFString@@PEBX1PEAVUObject@@H2@Z", "48 83 EC 48 48 8B 84 24 80 00 00 00 48 8B 49 78");
            FModuleManager::RegisterImport("?GenerateFullEnumName@UEnum@@UEBA?AVFString@@PEB_W@Z", "48 89 5C 24 10 48 89 74 24 18 57 48 83 EC 30 83 79 50 00");
            FModuleManager::RegisterImport("?GetCPPMacroType@UArrayProperty@@UEBA?AVFString@@AEAV2@@Z", "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 48 83 EC 30 48 8B 49 78");
            FModuleManager::RegisterImport("?GetCPPMacroType@UBoolProperty@@UEBA?AVFString@@AEAV2@@Z", "48 89 5C 24 18 57 48 83 EC 40 80 79 7B FF 48 8B DA 48 8B F9 75 ?");
            FModuleManager::RegisterImport("?GetCPPMacroType@UEnumProperty@@UEBA?AVFString@@AEAV2@@Z", "48 89 5C 24 10 57 48 83 EC 40 48 8B 81 80 00 00 00");
            FModuleManager::RegisterImport("?GetCPPMacroType@UObjectProperty@@UEBA?AVFString@@AEAV2@@Z", "48 89 5C 24 10 48 89 74 24 18 48 89 7C 24 20 41 56 48 83 EC 50 48 8B 41 78 48 89 D6 48 89 CF 48 8D 54 24 20 48 8D 4C 24 60 4D 89 C6 4C 8B 48 18 4C 89 4C 24 60 E8 ? ? ? ? 48 8B 4F 78 48 8D 1D ? ? ? ? 83 7C 24 28 00 48 0F 45 5C 24 20 48 8B 01 FF 90 78 02 00 00 49 89 D9 48 8D 15 ? ? ? ? 49 89 C0 48 8D 4C 24 30 E8 ? ? ? ? 48 8D 44 24 30 31 DB 49 39 C6 0F 84 ? ? ? ? 49 8B 0E 48 85 C9 74 ? E8 ? ? ? ? 48 8B 44 24 30 49 89 06 8B 44 24 38 41 89 46 08 8B 44 24 3C 41 89 46 0C 48 89 5C 24 38 48 89 5C 24 30 EB ? B1 8D");
            FModuleManager::RegisterImport("?GetCPPMacroType@UProperty@@UEBA?AVFString@@AEAV2@@Z", "48 89 5C 24 10 48 89 6C 24 18 48 89 74 24 20 57 41 56 41 57 48 83 EC 30 48 8D 05 ? ? ? ? 45 31 FF");
            FModuleManager::RegisterImport("?GetCPPMacroType@UStructProperty@@UEBA?AVFString@@AEAV2@@Z", "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 48 83 EC 30 48 8B 01 4C 89 C6");
            FModuleManager::RegisterImport("?GetCPPType@?$TProperty@HVUNumericProperty@@@@UEBA?AVFString@@PEAV2@I@Z", "53 48 83 EC 20 48 89 D3 31 C0 48 89 02 48 89 D9 48 C7 42 08 06 00 00 00 31 D2 E8 ? ? ? ? 48 8B 0B 48 8D 15 ? ? ? ? 41 B8 0C 00 00 00 E8 ? ? ? ? 48 89 D8 48 83 C4 20 5B C3 8A 9B 01 FE 00 51");
            FModuleManager::RegisterImport("?GetCPPType@?$TProperty@MVUNumericProperty@@@@UEBA?AVFString@@PEAV2@I@Z", "53 48 83 EC 20 48 89 D3 31 C0 48 89 02 48 89 D9 48 C7 42 08 06 00 00 00 31 D2 E8 ? ? ? ? 48 8B 0B 48 8D 15 ? ? ? ? 41 B8 0C 00 00 00 E8 ? ? ? ? 48 89 D8 48 83 C4 20 5B C3 E6 45");
            FModuleManager::RegisterImport("?GetCPPType@?$TProperty@VFName@@VUProperty@@@@UEBA?AVFString@@PEAV2@I@Z", "53 48 83 EC 20 48 89 D3 31 C0 48 89 02 48 89 D9 48 C7 42 08 06 00 00 00 31 D2 E8 ? ? ? ? 48 8B 0B 48 8D 15 ? ? ? ? 41 B8 0C 00 00 00 E8 ? ? ? ? 48 89 D8 48 83 C4 20 5B C3 4C E7 64");
            FModuleManager::RegisterImport("?GetCPPType@?$TUObjectPropertyBase@PEAVUObject@@@@UEBA?AVFString@@PEAV2@I@Z", "48 89 5C 24 10 48 89 6C 24 18 56 57 41 56 48 83 EC 50 48 8B 41 78 48 89 D5 49 89 CE 48 8D 54 24 30 48 8D 4C 24 70 44 89 CF 4C 89 C6 4C 8B 50 18 4C 89 54 24 70 E8 ? ? ? ? 49 8B 4E 78 48 8D 1D ? ? ? ? 83 7C 24 38 00 48 0F 45 5C 24 30 48 8B 01 FF 90 78 02 00 00 49 89 D9 48 8D 15 ? ? ? ? 49 89 C0 48 8D 4C 24 40 E8 ? ? ? ? 49 8B 06 48 8D 4C 24 40 48 89 4C 24 20 41 89 F9 4C 89 F1 49 89 F0 48 89 EA FF 90 18 03 00 00 48 8B 4C 24 40 48 85 C9 74 ? E8 ? ? ? ? 48 8B 4C 24 30 48 85 C9 74 ? E8 ? ? ? ? 48 8B 5C 24 78 48 89 E8 48 8B AC 24 80 00 00 00 48 83 C4 50 41 5E 5F 5E C3 DF 45 AD");
            FModuleManager::RegisterImport("?GetCPPType@UArrayProperty@@UEBA?AVFString@@PEAV2@I@Z", "49 89 E3 49 89 5B 08 49 89 6B 10 49 89 73 18 57 41 56");
            FModuleManager::RegisterImport("?GetCPPType@UBoolProperty@@UEBA?AVFString@@PEAV2@I@Z", "48 89 5C 24 18 57 48 83 EC 40 80 79 7B FF 48 8B DA 48 8B F9 0F 84 ? ? ? ?");
            FModuleManager::RegisterImport("?GetCPPType@UByteProperty@@UEBA?AVFString@@PEAV2@I@Z", "48 8B C4 53 57 41 55 41 57");
            FModuleManager::RegisterImport("?GetCPPType@UEnumProperty@@UEBA?AVFString@@PEAV2@I@Z", "48 89 5C 24 10 48 89 6C 24 18 48 89 74 24 20 57 48 83 EC 50 48 8B 81 80 00 00 00");
            FModuleManager::RegisterImport("?GetCPPType@UStructProperty@@UEBA?AVFString@@PEAV2@I@Z", "53 48 83 EC 20 48 8B 49 78 48 89 D3 48 8B 01 FF 90 C8 02 00 00");
            FModuleManager::RegisterImport("?GetCPPTypeCustom@UObjectProperty@@UEBA?AVFString@@PEAV2@IAEBV2@@Z", "53 48 83 EC 20 4C 8B 44 24 50 48 89 D3 41 83 78 08 00 74 ? 4D 8B 00 EB ? 5C");
            FModuleManager::RegisterImport("?GetCPPTypeForwardDeclaration@?$TProperty_Numeric@E@@UEBA?AVFString@@XZ", "33 C0 48 89 02 48 89 42 08 48 8B C2 C3 CC CC CC E9 ? ? ? ? CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC");
            FModuleManager::RegisterImport("?GetCPPTypeForwardDeclaration@?$TProperty_Numeric@H@@UEBA?AVFString@@XZ", "33 C0 48 89 02 48 89 42 08 48 8B C2 C3 CC CC CC E9 ? ? ? ? CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC");
            FModuleManager::RegisterImport("?GetCPPTypeForwardDeclaration@?$TProperty_Numeric@M@@UEBA?AVFString@@XZ", "33 C0 48 89 02 48 89 42 08 48 8B C2 C3 CC CC CC E9 ? ? ? ? CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC");
            FModuleManager::RegisterImport("?GetCPPTypeForwardDeclaration@UArrayProperty@@UEBA?AVFString@@XZ", "53 48 83 EC 20 48 8B 49 78 48 89 D3 48 8B 01 FF 90 58 02 00 00");
            FModuleManager::RegisterImport("?TagSubobjects@UObject@@UEAAXW4EObjectFlags@@@Z", "48 89 5C 24 18 48 89 74 24 20 57 48 81 EC E0 00 00 00 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 D0 00 00 00 8B FA");
            FModuleManager::RegisterImport("?GetCPPTypeForwardDeclaration@UEnumProperty@@UEBA?AVFString@@XZ", "48 89 5C 24 10 48 89 6C 24 18 48 89 74 24 20 57 48 83 EC 40 48 89 CD");
            FModuleManager::RegisterImport("?GetCPPTypeForwardDeclaration@UNameProperty@@UEBA?AVFString@@XZ", "33 C0 48 89 02 48 89 42 08 48 8B C2 C3 CC CC CC E9 ? ? ? ? CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC");
            FModuleManager::RegisterImport("?GetCPPTypeForwardDeclaration@UObjectProperty@@UEBA?AVFString@@XZ", "48 89 5C 24 10 48 89 74 24 18 57 48 83 EC 30 48 8B 41 78 48 89 D6 48 89 CF 48 8D 54 24 20 48 8D 4C 24 40 4C 8B 40 18 4C 89 44 24 40 E8 ? ? ? ? 48 8B 4F 78 48 8D 1D ? ? ? ? 83 7C 24 28 00 48 0F 45 5C 24 20 48 8B 01 FF 90 78 02 00 00 49 89 D9 48 8D 15 ? ? ? ? 49 89 C0 48 89 F1 E8 ? ? ? ? 48 8B 4C 24 20 48 85 C9 74 ? E8 ? ? ? ? 48 8B 5C 24 48 48 89 F0 48 8B 74 24 50 48 83 C4 30 5F C3 AE");
            FModuleManager::RegisterImport("?GetCPPTypeForwardDeclaration@UStructProperty@@UEBA?AVFString@@XZ", "53 48 83 EC 30 48 8B 41 78 48 89 D3 48 8D 54 24 20");
            FModuleManager::RegisterImport("?GetDisplayNameTextByIndex@UEnum@@UEBA?AVFText@@H@Z", "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 60 48 8B FA 41 8B F0");
            FModuleManager::RegisterImport("?GetFloatingPointPropertyValue@?$TProperty_Numeric@E@@UEBANPEBX@Z", "0F B6 02 66 0F 6E C0");
            FModuleManager::RegisterImport("?GetFloatingPointPropertyValue@?$TProperty_Numeric@H@@UEBANPEBX@Z", "66 0F 6E 02 F3 0F E6 C0");
            FModuleManager::RegisterImport("?GetFloatingPointPropertyValue@?$TProperty_Numeric@M@@UEBANPEBX@Z", "F3 0F 10 02 0F 5A C0 C3");
            FModuleManager::RegisterImport("?GetID@UObjectPropertyBase@@UEBA?AVFName@@XZ", "48 C7 02 05 00 00 00");
            FModuleManager::RegisterImport("?GetID@UProperty@@UEBA?AVFName@@XZ", "48 8B 41 10 48 8B 48 18 48 8B C2 48 89 0A C3 CC 48 89 5C 24 08");
            FModuleManager::RegisterImport("?GetIntPropertyEnum@UByteProperty@@UEBAPEAVUEnum@@XZ", "48 8B 41 78 C3 CC CC CC CC CC CC CC CC CC CC CC E9 ? ? ? ?");
            FModuleManager::RegisterImport("?GetIntPropertyEnum@UNumericProperty@@UEBAPEAVUEnum@@XZ", NullSub);
            FModuleManager::RegisterImport("?GetMinAlignment@?$TProperty@EVUNumericProperty@@@@UEBAHXZ", UNumericProperty_Char_GetMinAlignment);
            FModuleManager::RegisterImport("?GetMinAlignment@?$TProperty@HVUNumericProperty@@@@UEBAHXZ", UNumericProperty_Int_GetMinAlignment);
            FModuleManager::RegisterImport("?GetMinAlignment@?$TProperty@MVUNumericProperty@@@@UEBAHXZ", UNumericProperty_Int_GetMinAlignment);
            FModuleManager::RegisterImport("?GetMinAlignment@?$TProperty@PEAVUObject@@VUObjectPropertyBase@@@@UEBAHXZ", UObjectProperty_GetMinAlignment);
            FModuleManager::RegisterImport("?GetMinAlignment@?$TProperty@VFName@@VUProperty@@@@UEBAHXZ", UObjectProperty_GetMinAlignment);
            FModuleManager::RegisterImport("?GetMinAlignment@?$TProperty@VFScriptArray@@VUProperty@@@@UEBAHXZ", UObjectProperty_GetMinAlignment);
            FModuleManager::RegisterImport("?GetMinAlignment@UBoolProperty@@UEBAHXZ", "40 53 48 83 EC 40 8B 51 34");
            FModuleManager::RegisterImport("?GetMinAlignment@UEnumProperty@@UEBAHXZ", "48 8B 49 78 48 8B 01 48 FF A0 F0 02 00 00");
            FModuleManager::RegisterImport("?GetMinAlignment@UStructProperty@@UEBAHXZ", "48 8B 41 78 8B 40 44");
            FModuleManager::RegisterImport("?GetNumericPropertyValueToString@?$TProperty_Numeric@E@@UEBA?AVFString@@PEBX@Z", "53 48 83 EC 20 45 0F B6 00");
            FModuleManager::RegisterImport("?GetNumericPropertyValueToString@?$TProperty_Numeric@H@@UEBA?AVFString@@PEBX@Z", "53 48 83 EC 20 45 8B 00 48 89 D3 48 89 D9 48 8D 15 ? ? ? ? E8 ? ? ? ? 48 89 D8 48 83 C4 20 5B C3 4C 10 7B 02");
            FModuleManager::RegisterImport("?GetNumericPropertyValueToString@?$TProperty_Numeric@M@@UEBA?AVFString@@PEBX@Z", "53 48 83 EC 20 F3 41 0F 10 10");
            FModuleManager::RegisterImport("?GetObjectPropertyValue@UObjectProperty@@UEBAPEAVUObject@@PEBX@Z", "48 8B 02 C3 CC");
            FModuleManager::RegisterImport("?GetPreloadDependencies@UArrayProperty@@UEAAXAEAV?$TArray@PEAVUObject@@VFDefaultAllocator@@@@@Z", "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 48 89 D7 48 89 CB E8 ? ? ? ? 48 8B 73 78");
            FModuleManager::RegisterImport("?GetPreloadDependencies@UByteProperty@@UEAAXAEAV?$TArray@PEAVUObject@@VFDefaultAllocator@@@@@Z", "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 48 89 D7 48 89 CB E8 ? ? ? ? 48 8B 73 78");
            FModuleManager::RegisterImport("?GetNativePropertyValues@UObject@@UEBA_NAEAV?$TMap@VFString@@V1@VFDefaultSetAllocator@@U?$TDefaultMapHashableKeyFuncs@VFString@@V1@$0A@@@@@I@Z", NullSub);
            FModuleManager::RegisterImport("?GetPreloadDependencies@UStructProperty@@UEAAXAEAV?$TArray@PEAVUObject@@VFDefaultAllocator@@@@@Z", "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 48 89 D7 48 89 CB E8 ? ? ? ? 48 8B 73 78");
            FModuleManager::RegisterImport("?GetSignedIntPropertyValue@?$TProperty_Numeric@E@@UEBA_JPEBX@Z", "0F B6 02 C3 CC CC CC CC CC CC CC CC CC CC CC CC 48 63 02");
            FModuleManager::RegisterImport("?GetSignedIntPropertyValue@?$TProperty_Numeric@H@@UEBA_JPEBX@Z", "F3 48 0F 2C 02");
            FModuleManager::RegisterImport("?GetSignedIntPropertyValue@?$TProperty_Numeric@M@@UEBA_JPEBX@Z", "F3 48 0F 2C 02");
            FModuleManager::RegisterImport("?GetUnsignedIntPropertyValue@?$TProperty_Numeric@E@@UEBA_KPEBX@Z", "0F B6 02 C3 CC CC CC CC CC CC CC CC CC CC CC CC 48 63 02");
            FModuleManager::RegisterImport("?GetUnsignedIntPropertyValue@?$TProperty_Numeric@H@@UEBA_KPEBX@Z", "F3 48 0F 2C 02");
            FModuleManager::RegisterImport("?GetUnsignedIntPropertyValue@?$TProperty_Numeric@M@@UEBA_KPEBX@Z", "F3 48 0F 2C 02");
            FModuleManager::RegisterImport("?GetValueTypeHashInternal@?$TProperty_Numeric@E@@UEBAIPEBX@Z", "0F BE 02 C3 CC CC CC CC CC CC CC CC CC CC CC CC");
            FModuleManager::RegisterImport("?GetValueTypeHashInternal@?$TProperty_Numeric@H@@UEBAIPEBX@Z", "8B 02 C3 CC CC CC CC CC CC CC CC CC CC CC CC CC F3 0F 10 02");
            FModuleManager::RegisterImport("?GetValueTypeHashInternal@?$TProperty_Numeric@M@@UEBAIPEBX@Z", "8B 02 C3 CC CC CC CC CC CC CC CC CC CC CC CC CC F3 0F 10 02");
            FModuleManager::RegisterImport("?GetValueTypeHashInternal@UBoolProperty@@UEBAIPEBX@Z", "0F BE 02 C3 CC CC CC CC CC CC CC CC CC CC CC CC");
            FModuleManager::RegisterImport("?GetValueTypeHashInternal@UEnumProperty@@EEBAIPEBX@Z", "48 8B 49 78 E9 ? ? ? ? CC CC CC CC CC CC CC 40 57");
            FModuleManager::RegisterImport("?GetValueTypeHashInternal@UNameProperty@@UEBAIPEBX@Z", "48 8B 0A 48 8B C1 48 C1 E8 20 03 C1");
            FModuleManager::RegisterImport("?GetValueTypeHashInternal@UObjectProperty@@EEBAIPEBX@Z", "4C 8B 02 41 B9 B9 79 37 9E");
            FModuleManager::RegisterImport("?GetValueTypeHashInternal@UProperty@@MEBAIPEBX@Z", NullSub);
            FModuleManager::RegisterImport("?GetValueTypeHashInternal@UStructProperty@@EEBAIPEBX@Z", "48 8B 49 78 48 8B 01 48 FF A0 B0 02 00 00");
            FModuleManager::RegisterImport("?Identical@?$TProperty_WithEqualityAndSerializer@EVUNumericProperty@@@@UEBA_NPEBX0I@Z", "4D 85 C0 74 58 41 0F B6 00 38 02 0F 94 D0 C3 D4");
            FModuleManager::RegisterImport("?Identical@?$TProperty_WithEqualityAndSerializer@HVUNumericProperty@@@@UEBA_NPEBX0I@Z", "4D 85 C0 74 ? 41 8B 00 39 02 0F 94 D0 C3 69 94 F8 5B 4E 5B FE 03 F0 11 21");
            FModuleManager::RegisterImport("?Identical@?$TProperty_WithEqualityAndSerializer@MVUNumericProperty@@@@UEBA_NPEBX0I@Z", "4D 85 C0 74 ? 41 8B 00 39 02 0F 94 D0 C3 69 94 F8 5B 4E 5B FE 03 F0 11 21");
            FModuleManager::RegisterImport("?Identical@?$TProperty_WithEqualityAndSerializer@VFName@@VUProperty@@@@UEBA_NPEBX0I@Z", "4D 85 C0 74 ? 49 8B 00 48 39 02 0F 94 D0 C3 54");
            FModuleManager::RegisterImport("?Identical@UArrayProperty@@UEBA_NPEBX0I@Z", "44 89 4C 24 20 48 89 4C 24 08 55 41 55");
            FModuleManager::RegisterImport("?Identical@UBoolProperty@@UEBA_NPEBX0I@Z", "0F B6 41 79 49 89 C9");
            FModuleManager::RegisterImport("?Identical@UEnumProperty@@UEBA_NPEBX0I@Z", "48 8B 49 78 48 8B 01 48 FF A0 70 02 00 00");
            FModuleManager::RegisterImport("?Identical@UObjectPropertyBase@@UEBA_NPEBX0I@Z", "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 48 89 7C 24 20 41 56 48 83 EC 20 33 FF 45 8B F1 49 8B E8");
            FModuleManager::RegisterImport("?Identical@UStructProperty@@UEBA_NPEBX0I@Z", "48 8B 49 78 E9 ? ? ? ? CC CC CC CC CC CC CC 48 89 5C 24 10");
            FModuleManager::RegisterImport("?ImportText_Internal@UArrayProperty@@UEBAPEB_WPEB_WPEAXHPEAVUObject@@PEAVFOutputDevice@@@Z", "48 89 5C 24 10 44 89 4C 24 20 48 89 4C 24 08");
            FModuleManager::RegisterImport("?ImportText_Internal@UBoolProperty@@UEBAPEB_WPEB_WPEAXHPEAVUObject@@PEAVFOutputDevice@@@Z", "48 8B C4 55 41 54 48 8B EC 48 83 EC 58");
            FModuleManager::RegisterImport("?ImportText_Internal@UByteProperty@@UEBAPEB_WPEB_WPEAXHPEAVUObject@@PEAVFOutputDevice@@@Z", "44 89 4C 24 20 53 55 56 57 41 55");
            FModuleManager::RegisterImport("?ImportText_Internal@UEnumProperty@@UEBAPEB_WPEB_WPEAXHPEAVUObject@@PEAVFOutputDevice@@@Z", "4C 8B DC 49 89 5B 08 49 89 6B 10 49 89 73 18 49 89 7B 20 41 56 48 83 EC 50 41 8B E9");
            FModuleManager::RegisterImport("?ImportText_Internal@UNameProperty@@UEBAPEB_WPEB_WPEAXHPEAVUObject@@PEAVFOutputDevice@@@Z", "48 89 5C 24 08 57 48 83 EC 40 4C 89 C7");
            FModuleManager::RegisterImport("?ImportText_Internal@UNumericProperty@@UEBAPEB_WPEB_WPEAXHPEAVUObject@@PEAVFOutputDevice@@@Z", "48 89 5C 24 08 48 89 74 24 18 48 89 54 24 10 57 48 83 EC 20 49 8B F0 48 8B DA");
            FModuleManager::RegisterImport("?ImportText_Internal@UObjectProperty@@UEBAPEB_WPEB_WPEAXHPEAVUObject@@PEAVFOutputDevice@@@Z", "48 89 5C 24 10 48 89 6C 24 18 48 89 7C 24 20 41 56 48 83 EC 40 48 8B 44 24 78");
            FModuleManager::RegisterImport("?ImportText_Internal@UStructProperty@@UEBAPEB_WPEB_WPEAXHPEAVUObject@@PEAVFOutputDevice@@@Z", "48 89 5C 24 10 48 89 6C 24 18 48 89 74 24 20 57 48 83 EC 60 48 89 D6");
            FModuleManager::RegisterImport("?InitializeValueInternal@?$TProperty@EVUNumericProperty@@@@UEBAXPEAX@Z", "31 C0 39 41 30 7E ? 66 0F 1F 84 00 00 00 00 00 41 89 C0 44 0F AF 41 34 4D 63 C8 49 01 D1 74 ? 45 31 C0 45 88 01 FF C0 3B 41 30 7C ? C3 AE");
            FModuleManager::RegisterImport("?InitializeValueInternal@?$TProperty@HVUNumericProperty@@@@UEBAXPEAX@Z", "31 C0 39 41 30 7E ? 66 0F 1F 84 00 00 00 00 00 41 89 C0 44 0F AF 41 34 4D 63 C8 49 01 D1 74 ? 45 31 C0 45 89 01 FF C0 3B 41 30 7C ? C3 B8 C9 F9 32 E9");
            FModuleManager::RegisterImport("?InitializeValueInternal@?$TProperty@MVUNumericProperty@@@@UEBAXPEAX@Z", "31 C0 39 41 30 7E ? 66 0F 1F 84 00 00 00 00 00 41 89 C0 44 0F AF 41 34 4D 63 C8 49 01 D1 74 ? 45 31 C0 45 89 01 FF C0 3B 41 30 7C ? C3 B8 C9 F9 32 E9");
            FModuleManager::RegisterImport("?InitializeValueInternal@?$TProperty@PEAVUObject@@VUObjectPropertyBase@@@@UEBAXPEAX@Z", "31 C0 39 41 30 7E ? 66 0F 1F 84 00 00 00 00 00 41 89 C0 44 0F AF 41 34 4D 63 C8 49 01 D1 74 ? 45 31 C0 4D 89 01 FF C0 3B 41 30 7C ? C3 F6 7E 8E");
            FModuleManager::RegisterImport("?InitializeValueInternal@?$TProperty@VFName@@VUProperty@@@@UEBAXPEAX@Z", "45 31 D2 49 89 D1 44 89 D0 44 39 51 30");
            FModuleManager::RegisterImport("?InitializeValueInternal@?$TProperty@VFScriptArray@@VUProperty@@@@UEBAXPEAX@Z", "45 31 C9 45 89 C8 44 39 49 30 7E ? 0F 1F 40 00 44 89 C0 0F AF 41 34 48 98 48 01 D0 74 ? 4C 89 08 4C 89 48 08 41 FF C0 44 3B 41 30 7C ? C3 45 E2 12");
            FModuleManager::RegisterImport("?InitializeValueInternal@UBoolProperty@@UEBAXPEAX@Z", "0F B6 41 7B 44 0F B6 41 79");
            FModuleManager::RegisterImport("?InitializeValueInternal@UProperty@@MEBAXPEAX@Z", VoidSub);
            FModuleManager::RegisterImport("?InitializeValueInternal@UStructProperty@@UEBAXPEAX@Z", "48 8B 41 78 44 8B 41 30 48 8B C8 4C 8B 08 49 FF A1 60 02 00 00");
            FModuleManager::RegisterImport("?InstanceSubobjects@UArrayProperty@@UEAAXPEAXPEBXPEAVUObject@@PEAUFObjectInstancingGraph@@@Z", "40 55 41 54 41 55 41 56 41 57 48 83 EC 60 48 8D 6C 24 30 48 89 5D 60 48 89 75 68 48 89 7D 70 48 8B 05 ? ? ? ? 48 33 C5 48 89 45 20 4C 89 4D 18");
            FModuleManager::RegisterImport("?InstanceSubobjects@UObjectPropertyBase@@UEAAXPEAXPEBXPEAVUObject@@PEAUFObjectInstancingGraph@@@Z", "48 89 5C 24 18 48 89 74 24 20 57 41 54 41 56 48 83 EC 40");
            FModuleManager::RegisterImport("?InstanceSubobjects@UProperty@@UEAAXPEAXPEBXPEAVUObject@@PEAUFObjectInstancingGraph@@@Z", VoidSub);
            FModuleManager::RegisterImport("?InstanceSubobjects@UStructProperty@@UEAAXPEAXPEBXPEAVUObject@@PEAUFObjectInstancingGraph@@@Z", "48 89 5C 24 10 48 89 74 24 18 57 41 56 41 57 48 83 EC 30 31 DB");
            FModuleManager::RegisterImport("?IsFloatingPoint@?$TProperty_Numeric@E@@UEBA_NXZ", NullSub);
            FModuleManager::RegisterImport("?IsFloatingPoint@?$TProperty_Numeric@H@@UEBA_NXZ", NullSub);
            FModuleManager::RegisterImport("?IsFloatingPoint@?$TProperty_Numeric@M@@UEBA_NXZ", TrueSub);
            FModuleManager::RegisterImport("?IsInteger@?$TProperty_Numeric@E@@UEBA_NXZ", TrueSub);
            FModuleManager::RegisterImport("?IsInteger@?$TProperty_Numeric@H@@UEBA_NXZ", TrueSub);
            FModuleManager::RegisterImport("?IsInteger@?$TProperty_Numeric@M@@UEBA_NXZ", NullSub);
            FModuleManager::RegisterImport("?LinkInternal@?$TProperty@EVUNumericProperty@@@@UEAAXAEAVFArchive@@@Z", "48 B8 00 02 00 40 10 00 08 00 C7 41 34 01 00 00 00 48 09 41 38 C3 52");
            FModuleManager::RegisterImport("?LinkInternal@?$TProperty@HVUNumericProperty@@@@UEAAXAEAVFArchive@@@Z", "48 B8 00 02 00 40 10 00 08 00 C7 41 34 04 00 00 00 48 09 41 38 C3 46 6E");
            FModuleManager::RegisterImport("?LinkInternal@?$TProperty@MVUNumericProperty@@@@UEAAXAEAVFArchive@@@Z", "48 B8 00 02 00 40 10 00 08 00 C7 41 34 04 00 00 00 48 09 41 38 C3 46 6E");
            FModuleManager::RegisterImport("?LinkInternal@?$TProperty@PEAVUObject@@VUObjectPropertyBase@@@@UEAAXAEAVFArchive@@@Z", "48 B8 00 02 00 40 10 00 08 00 C7 41 34 08 00 00 00");
            FModuleManager::RegisterImport("?LinkInternal@?$TProperty@VFName@@VUProperty@@@@UEAAXAEAVFArchive@@@Z", "48 B8 00 02 00 40 10 00 08 00 C7 41 34 08 00 00 00");
            FModuleManager::RegisterImport("?LinkInternal@UArrayProperty@@UEAAXAEAVFArchive@@@Z", "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 48 89 D6 48 89 CB E8 ? ? ? ? 48 85 C0");
            FModuleManager::RegisterImport("?LinkInternal@UBoolProperty@@UEAAXAEAVFArchive@@@Z", "80 79 7B FF 0F B6 41 78");
            FModuleManager::RegisterImport("?LinkInternal@UEnumProperty@@UEAAXAEAVFArchive@@@Z", "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 48 8B 02 48 89 D7 48 8B 51 78");
            FModuleManager::RegisterImport("?LinkInternal@UStructProperty@@UEAAXAEAVFArchive@@@Z", "48 89 5C 24 08 57 48 83 EC 20 8B 41 08 48 89 D7");
            FModuleManager::RegisterImport("?NetSerializeItem@UArrayProperty@@UEBA_NAEAVFArchive@@PEAVUPackageMap@@PEAXPEAV?$TArray@EVFDefaultAllocator@@@@@Z", "48 83 EC 28 4C 8D 05 ? ? ? ? BA EF 00 00 00");
            FModuleManager::RegisterImport("?NetSerializeItem@UBoolProperty@@UEBA_NAEAVFArchive@@PEAVUPackageMap@@PEAXPEAV?$TArray@EVFDefaultAllocator@@@@@Z", "48 89 5C 24 10 57 48 83 EC 20 0F B6 59 79 49 89 D2");
            FModuleManager::RegisterImport("?NetSerializeItem@UByteProperty@@UEBA_NAEAVFArchive@@PEAVUPackageMap@@PEAXPEAV?$TArray@EVFDefaultAllocator@@@@@Z", "48 89 5C 24 08 57 48 83 EC 20 48 8B 49 78");
            FModuleManager::RegisterImport("?NetSerializeItem@UEnumProperty@@UEBA_NAEAVFArchive@@PEAVUPackageMap@@PEAXPEAV?$TArray@EVFDefaultAllocator@@@@@Z", "48 83 EC 28 48 8B 81 80 00 00 00");
            FModuleManager::RegisterImport("?NetSerializeItem@UObjectPropertyBase@@UEBA_NAEAVFArchive@@PEAVUPackageMap@@PEAXPEAV?$TArray@EVFDefaultAllocator@@@@@Z", "48 89 5C 24 10 48 89 6C 24 18 48 89 74 24 20 57 48 83 EC 30 48 8B 01 48 89 D3");
            FModuleManager::RegisterImport("?NetSerializeItem@UProperty@@UEBA_NAEAVFArchive@@PEAVUPackageMap@@PEAXPEAV?$TArray@EVFDefaultAllocator@@@@@Z", "48 83 EC 28 48 8B 01 4D 89 C8");
            FModuleManager::RegisterImport("?NetSerializeItem@UStructProperty@@UEBA_NAEAVFArchive@@PEAVUPackageMap@@PEAXPEAV?$TArray@EVFDefaultAllocator@@@@@Z", "48 83 EC 38 48 8B 49 78");
            FModuleManager::RegisterImport("?PassCPPArgsByRef@?$TProperty@EVUNumericProperty@@@@UEBA_NXZ", NullSub);
            FModuleManager::RegisterImport("?PassCPPArgsByRef@?$TProperty@HVUNumericProperty@@@@UEBA_NXZ", NullSub);
            FModuleManager::RegisterImport("?PassCPPArgsByRef@?$TProperty@MVUNumericProperty@@@@UEBA_NXZ", NullSub);
            FModuleManager::RegisterImport("?PassCPPArgsByRef@?$TProperty@PEAVUObject@@VUObjectPropertyBase@@@@UEBA_NXZ", NullSub);
            FModuleManager::RegisterImport("?PassCPPArgsByRef@?$TProperty@VFName@@VUProperty@@@@UEBA_NXZ", NullSub);
            FModuleManager::RegisterImport("?PassCPPArgsByRef@UArrayProperty@@UEBA_NXZ", TrueSub);
            FModuleManager::RegisterImport("?PassCPPArgsByRef@UProperty@@UEBA_NXZ", NullSub);
            FModuleManager::RegisterImport("?PostLoad@UField@@UEAAXXZ", "40 53 48 83 EC 20 48 8B D9 E8 ? ? ? ? 48 8B 03 48 8B CB 48 83 C4 20 5B 48 FF A0 38 02 00 00");
            FModuleManager::RegisterImport("?ResolveEnumerator@UEnum@@UEBA_JAEAVFArchive@@_J@Z", "49 8B C0 C3 CC CC CC CC CC CC CC CC CC CC CC CC 40 53 48 83 EC 20 8B 51 50");
            FModuleManager::RegisterImport("?SameType@UArrayProperty@@UEBA_NPEBVUProperty@@@Z", "48 83 EC 28 48 85 D2 0F 84 ? ? ? ? 48 8B 42 10");
            FModuleManager::RegisterImport("?SameType@UEnumProperty@@UEBA_NPEBVUProperty@@@Z", "48 89 5C 24 08 57 48 83 EC 20 48 89 D3 48 89 CF E8 ? ? ? ? 84 C0 74 ? 48 8B 87 80 00 00 00");
            FModuleManager::RegisterImport("?SameType@UObjectPropertyBase@@UEBA_NPEBVUProperty@@@Z", "48 85 D2 74 ? 48 8B 42 10 48 39 41 10 75 ? 48 8B 42 78 48 39 41 78 75 ? B0 01");
            FModuleManager::RegisterImport("?SameType@UProperty@@UEBA_NPEBV1@@Z", "48 85 D2 74 ? 48 8B 42 10 48 39 41 10 75 ? B0 01");
            FModuleManager::RegisterImport("?SameType@UStructProperty@@UEBA_NPEBVUProperty@@@Z", "48 85 D2 74 ? 48 8B 42 10 48 39 41 10 75 ? 48 8B 42 78 48 39 41 78 75 ? B0 01");
            FModuleManager::RegisterImport("?Serialize@UArrayProperty@@UEAAXAEAVFArchive@@@Z", "48 89 5C 24 08 57 48 83 EC 20 48 8B FA 48 8B D9 E8 ? ? ? ? 48 8B 07 48 8D 53 78 48 8B CF 48 8B 5C 24 30");
            FModuleManager::RegisterImport("?Serialize@UBoolProperty@@UEAAXAEAVFArchive@@@Z", "48 89 5C 24 18 57 48 83 EC 20 48 8B FA 48 8B D9 E8 ? ? ? ? 48 8B 4F 08");
            FModuleManager::RegisterImport("?Serialize@UByteProperty@@UEAAXAEAVFArchive@@@Z", "48 89 5C 24 08 57 48 83 EC 20 48 89 D7 48 89 CB E8 ? ? ? ? 48 8B 07");
            FModuleManager::RegisterImport("?Serialize@UEnum@@UEAAXAEAVFArchive@@@Z", "40 56 41 57 48 83 EC 48");
            FModuleManager::RegisterImport("?Serialize@UEnumProperty@@UEAAXAEAVFArchive@@@Z", "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 48 89 D7 48 89 CE");
            FModuleManager::RegisterImport("?Serialize@UObjectPropertyBase@@UEAAXAEAVFArchive@@@Z", "48 89 5C 24 18 48 89 74 24 20 57 48 83 EC 20 48 8B DA 48 8B F1 E8 ? ? ? ? 48 8B 03 48 8D 56 78 48 8B CB FF 50 28 F6 43 28 01 75 ? F6 43 2A 04 74 ? 48 8B 5E 78 48 85 DB 74 ? E8 ? ? ? ? 48 8B 53 10 4C 8D 80 88 00 00 00 49 63 40 08 3B 82 90 00 00 00 7F ? 48 8B C8 48 8B 82 88 00 00 00 4C 39 04 C8 75 ? 48 8D 8B 08 03 00 00");
            FModuleManager::RegisterImport("?Serialize@UProperty@@UEAAXAEAVFArchive@@@Z", "48 89 5C 24 10 48 89 6C 24 18 48 89 74 24 20 57 48 83 EC 20 48 8B DA 48 8B F9 E8 ? ? ? ? 4C 8B 43 08 48 BD FF FD FF BF EF FF F7 FF");
            FModuleManager::RegisterImport("?Serialize@UStructProperty@@UEAAXAEAVFArchive@@@Z", "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 48 8B FA 48 8B F1 E8 ? ? ? ? 65 48 8B 04 25 58 00 00 00");
            FModuleManager::RegisterImport("?SerializeItem@?$TProperty_WithEqualityAndSerializer@HVUNumericProperty@@@@UEBAXAEAVFArchive@@PEAXPEBX@Z", "48 8B 4A 08 4C 8B CA");
            FModuleManager::RegisterImport("?SerializeItem@?$TProperty_WithEqualityAndSerializer@MVUNumericProperty@@@@UEBAXAEAVFArchive@@PEAXPEBX@Z", "48 89 5C 24 08 57 48 83 EC 20 48 8B 4A 08 48 89 D3 4C 89 C7 48 8B 11 48 8D 42 04 48 3B 41 08 77 ? 8B 02 41 89 00 48 8B 43 08 48 83 00 04 48 8B 5C 24 30");
            FModuleManager::RegisterImport("?SerializeItem@?$TProperty_WithEqualityAndSerializer@VFName@@VUProperty@@@@UEBAXAEAVFArchive@@PEAXPEBX@Z", "48 8B 02 48 8B CA 49 8B D0 48 FF 60 38 CC CC CC 44 88 44 24 18");
            FModuleManager::RegisterImport("?SerializeItem@UArrayProperty@@UEBAXAEAVFArchive@@PEAXPEBX@Z", "4C 89 4C 24 20 48 89 4C 24 08 55 53 57 41 54 41 55 41 56 41 57 48 8D 6C 24 D9");
            FModuleManager::RegisterImport("?SerializeItem@UBoolProperty@@UEBAXAEAVFArchive@@PEAXPEBX@Z", "48 89 5C 24 10 57 48 83 EC 20 0F B6 59 79 48 89 CF");
            FModuleManager::RegisterImport("?SerializeItem@UByteProperty@@UEBAXAEAVFArchive@@PEAXPEBX@Z", "48 89 5C 24 18 48 89 74 24 20 57 48 83 EC 30 48 83 79 78 00");
            FModuleManager::RegisterImport("?SerializeItem@UEnumProperty@@UEBAXAEAVFArchive@@PEAXPEBX@Z", "48 89 5C 24 10 48 89 6C 24 18 48 89 74 24 20 57 48 83 EC 20 48 83 B9 80 00 00 00 00");
            FModuleManager::RegisterImport("?SerializeItem@UObjectProperty@@UEBAXAEAVFArchive@@PEAXPEBX@Z", "48 89 5C 24 08 48 89 74 24 18 57 48 83 EC 20 F6 42 2A 04");
            FModuleManager::RegisterImport("?SerializeItem@UStructProperty@@UEBAXAEAVFArchive@@PEAXPEBX@Z", "48 89 5C 24 10 48 89 6C 24 18 48 89 74 24 20 57 48 83 EC 20 48 89 D6 48 89 CD");
            FModuleManager::RegisterImport("?SetEnums@UEnum@@UEAA_NAEAV?$TArray@U?$TTuple@VFName@@_J@@VFDefaultAllocator@@@@W4ECppForm@1@_N@Z", "40 53 56 57 41 56 41 57 48 83 EC 60 83 79 48 00");
            FModuleManager::RegisterImport("?SetFloatingPointPropertyValue@?$TProperty_Numeric@E@@UEBAXPEAXN@Z", "F2 0F 2C C2 88 02");
            FModuleManager::RegisterImport("?SetFloatingPointPropertyValue@?$TProperty_Numeric@H@@UEBAXPEAXN@Z", "F2 0F 2C C2 89 02");
            FModuleManager::RegisterImport("?SetFloatingPointPropertyValue@?$TProperty_Numeric@M@@UEBAXPEAXN@Z", "0F 57 C0 F2 0F 5A C2 F3 0F 11 02");
            FModuleManager::RegisterImport("?SetIntPropertyValue@?$TProperty_Numeric@E@@UEBAXPEAX_J@Z", "44 88 02 C3 CC CC CC CC CC CC CC CC CC CC CC CC 44 89 02");
            FModuleManager::RegisterImport("?SetIntPropertyValue@?$TProperty_Numeric@E@@UEBAXPEAX_K@Z", "44 88 02 C3 CC CC CC CC CC CC CC CC CC CC CC CC 44 89 02");
            FModuleManager::RegisterImport("?SetIntPropertyValue@?$TProperty_Numeric@H@@UEBAXPEAX_J@Z", "44 89 02 C3 CC CC CC CC CC CC CC CC CC CC CC CC 0F 57 C0");
            FModuleManager::RegisterImport("?SetIntPropertyValue@?$TProperty_Numeric@H@@UEBAXPEAX_K@Z", "44 89 02 C3 CC CC CC CC CC CC CC CC CC CC CC CC 0F 57 C0");
            FModuleManager::RegisterImport("?SetIntPropertyValue@?$TProperty_Numeric@M@@UEBAXPEAX_J@Z", "0F 57 C0 F3 49 0F 2A C0 F3 0F 11 02");
            FModuleManager::RegisterImport("?SetIntPropertyValue@?$TProperty_Numeric@M@@UEBAXPEAX_K@Z", "0F 57 C0 F3 49 0F 2A C0 F3 0F 11 02");
            FModuleManager::RegisterImport("?SetNumericPropertyValueFromString@?$TProperty_Numeric@E@@UEBAXPEAXPEB_W@Z", "53 48 83 EC 20 4C 89 C1 48 89 D3 FF 15 ? ? ? ? 88 03 48 83 C4 20 5B C3 D9 48 CE FD");
            FModuleManager::RegisterImport("?SetNumericPropertyValueFromString@?$TProperty_Numeric@H@@UEBAXPEAXPEB_W@Z", "53 48 83 EC 20 4C 89 C1 48 89 D3 FF 15 ? ? ? ? 89 03 48 83 C4 20 5B C3 84 6C 4A E0");
            FModuleManager::RegisterImport("?SetNumericPropertyValueFromString@?$TProperty_Numeric@M@@UEBAXPEAXPEB_W@Z", "53 48 83 EC 20 4C 89 C1 48 89 D3 FF 15 ? ? ? ? 0F 57 C9");
            FModuleManager::RegisterImport("?SetObjectPropertyValue@UObjectProperty@@UEBAXPEAXPEAVUObject@@@Z", "4C 89 02 C3 CC CC CC CC CC CC CC CC");
            FModuleManager::RegisterImport("?StaticAllocateObject@@YAPEAVUObject@@PEAVUClass@@PEAV1@VFName@@W4EObjectFlags@@W4EInternalObjectFlags@@_NPEA_N@Z", "4C 89 44 24 18 48 89 54 24 10 55 53 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 F9");
            FModuleManager::RegisterImport("?Get@FObjectInitializer@@SAAEAV1@XZ", "40 53 48 83 EC 30 33 C0 48 8D 15 ? ? ? ? 88 44 24 40 48 8D 4C 24 20");
            FModuleManager::RegisterImport("??1UObject@@UEAA@XZ", "40 53 48 83 EC 30 48 8D 05 ? ? ? ? 48 8B D9 48 89 01 80 3D ? ? ? ? ?");
            FModuleManager::RegisterImport("??4FWeakObjectPtr@@QEAAXPEBVUObject@@@Z", "40 53 48 83 EC 20 48 8B D9 48 85 D2 74 ? 8B 52 0C");
            FModuleManager::RegisterImport("?IsValid@FWeakObjectPtr@@QEBA_NXZ", "44 8B 41 04 45 85 C0 74 ? 48 63 01");
            FModuleManager::RegisterImport("?AddToCluster@UObjectBaseUtility@@UEAAXPEAV1@_N@Z", "40 55 41 56 48 8D AC 24 D8 FE FF FF");
            FModuleManager::RegisterImport("?AreNativePropertiesIdenticalTo@UObject@@UEBA_NPEAV1@@Z", TrueSub);
            FModuleManager::RegisterImport("?BeginDestroy@UObject@@UEAAXXZ", "40 53 48 83 EC 40 8B 41 08 48 8B D9 C1 E8 0F");
            FModuleManager::RegisterImport("?BuildSubobjectMapping@UObject@@UEBAXPEAV1@AEAV?$TMap@PEAVUObject@@PEAV1@VFDefaultSetAllocator@@U?$TDefaultMapHashableKeyFuncs@PEAVUObject@@PEAV1@$0A@@@@@@Z", "48 89 5C 24 08 48 89 54 24 10 57 48 83 EC 60 49 8B D8");
            FModuleManager::RegisterImport("?CallRemoteFunction@UObject@@UEAA_NPEAVUFunction@@PEAXPEAUFOutParmRec@@PEAUFFrame@@@Z", NullSub);
            FModuleManager::RegisterImport("?CanBeClusterRoot@UObjectBaseUtility@@UEBA_NXZ", NullSub);
            FModuleManager::RegisterImport("?CanBeInCluster@UObjectBaseUtility@@UEBA_NXZ", "48 8B 49 20 48 85 C9 74 ? 48 8B 01 48 FF 60 20");
            FModuleManager::RegisterImport("?CheckDefaultSubobjectsInternal@UObject@@MEAA_NXZ", "48 85 C9 44 0F B6 D2");
            FModuleManager::RegisterImport("?CreateCluster@UObjectBaseUtility@@UEAAXXZ", "40 55 56 57 41 55 48 8D AC 24 F8 FE FF FF");
            FModuleManager::RegisterImport("?DeferredRegister@UObjectBase@@MEAAXPEAVUClass@@PEB_W1@Z", "48 89 5C 24 10 48 89 74 24 18 57 48 83 EC 20 48 8B DA 48 8B F1 49 8B D0");
            FModuleManager::RegisterImport("?ExportCustomProperties@UObject@@UEAAXAEAVFOutputDevice@@I@Z", VoidSub);
            FModuleManager::RegisterImport("?FinishDestroy@UObject@@UEAAXXZ", "40 53 48 83 EC 40 F6 41 0A 01");
            FModuleManager::RegisterImport("?GetAssetRegistryTags@UObject@@UEBAXAEAV?$TArray@UFAssetRegistryTag@UObject@@VFDefaultAllocator@@@@@Z", "48 89 5C 24 10 48 89 74 24 18 48 89 7C 24 20 55 41 54 41 55 41 56 41 57 48 8B EC 48 83 EC 70 48 8B 01");
            FModuleManager::RegisterImport("?GetConfigOverridePlatform@UObject@@UEBAPEB_WXZ", NullSub);
            FModuleManager::RegisterImport("?GetDesc@UObject@@UEAA?AVFString@@XZ", UObject_GetDesc);
            FModuleManager::RegisterImport("?GetDetailedInfoInternal@UObject@@MEBA?AVFString@@XZ", "53 48 83 EC 20 48 89 D3 31 C0 48 89 02 48 89 D9 48 C7 42 08 1B 00 00 00 31 D2 E8 ? ? ? ? 48 8B 0B 48 8D 15 ? ? ? ? 41 B8 36 00 00 00 E8 ? ? ? ? 48 89 D8 48 83 C4 20 5B C3 E3 ?");
            FModuleManager::RegisterImport("?GetExporterName@UObject@@UEAA?AVFName@@XZ", UObject_GetExporterName);
            FModuleManager::RegisterImport("?GetFunctionCallspace@UObject@@UEAAHPEAVUFunction@@PEAXPEAUFFrame@@@Z", Return2);
            FModuleManager::RegisterImport("?GetLifetimeReplicatedProps@UObject@@UEBAXAEAV?$TArray@VFLifetimeProperty@@VFDefaultAllocator@@@@@Z", VoidSub);
            FModuleManager::RegisterImport("?ShutdownAfterError@UObject@@UEAAXXZ", VoidSub);
            FModuleManager::RegisterImport("?Serialize@UObject@@UEAAXAEAVFArchive@@@Z", "48 89 5C 24 20 57 48 83 EC 20 48 8B 41 20");
            FModuleManager::RegisterImport("?Rename@UObject@@UEAA_NPEB_WPEAV1@I@Z", "48 89 5C 24 08 44 89 4C 24 20 55 56 57 41 54 41 55 41 56 41 57 48 8B EC 48 83 EC 70");
            FModuleManager::RegisterImport("?RegisterDependencies@UObjectBase@@MEAAXXZ", "48 8B 49 30 48 85 C9 74 ? 48 8B 01 48 FF 60 08");
            FModuleManager::RegisterImport("?RegenerateClass@UObject@@UEAAPEAVUClass@@PEAV2@PEAV1@AEAV?$TArray@PEAVUObject@@VFDefaultAllocator@@@@@Z", NullSub);
            FModuleManager::RegisterImport("?ProcessConsoleExec@UObject@@UEAA_NPEB_WAEAVFOutputDevice@@PEAV1@@Z", ProcessConsoleExec);
            FModuleManager::RegisterImport("?PreSaveRoot@UObject@@UEAA_NPEB_W@Z", NullSub);
            FModuleManager::RegisterImport("?PreSave@UObject@@UEAAXPEBVITargetPlatform@@@Z", VoidSub);
            FModuleManager::RegisterImport("?PreNetReceive@UObject@@UEAAXXZ", VoidSub);
            FModuleManager::RegisterImport("?PreDestroyFromReplication@UObject@@UEAAXXZ", VoidSub);
            FModuleManager::RegisterImport("?PostSaveRoot@UObject@@UEAAX_N@Z", VoidSub);
            FModuleManager::RegisterImport("?PostRepNotifies@UObject@@UEAAXXZ", VoidSub);
            FModuleManager::RegisterImport("?PostRename@UObject@@UEAAXPEAV1@VFName@@@Z", VoidSub);
            FModuleManager::RegisterImport("?PostReloadConfig@UObject@@UEAAXPEAVUProperty@@@Z", VoidSub);
            FModuleManager::RegisterImport("?PostNetReceive@UObject@@UEAAXXZ", VoidSub);
            FModuleManager::RegisterImport("?PostLoadSubobjects@UObject@@UEAAXPEAUFObjectInstancingGraph@@@Z", "40 55 53 41 56 48 8D 6C 24 E0");
            FModuleManager::RegisterImport("?PostLoad@UObject@@UEAAXXZ", "40 53 48 83 EC 30 48 8B 41 10 48 8B D9 8B 90 B4 00 00 00");
            FModuleManager::RegisterImport("?PostInterpChange@UObject@@UEAAXPEAVUProperty@@@Z", VoidSub);
            FModuleManager::RegisterImport("?PostInitProperties@UObject@@UEAAXXZ", "48 83 EC 38 48 8B D1 48 C7 44 24 20 00 00 00 00");
            FModuleManager::RegisterImport("?PostEditImport@UObject@@UEAAXXZ", VoidSub);
            FModuleManager::RegisterImport("?PostDuplicate@UObject@@UEAAX_N@Z", VoidSub);
            FModuleManager::RegisterImport("?PostDuplicate@UObject@@UEAAXW4Type@EDuplicateMode@@@Z", "48 8B 01 83 FA 02 0F 94 C2");
            FModuleManager::RegisterImport("?PostCDOContruct@UObject@@UEAAXXZ", VoidSub);
            FModuleManager::RegisterImport("?OnClusterMarkedAsPendingKill@UObjectBaseUtility@@UEAAXXZ", VoidSub);
            FModuleManager::RegisterImport("?NeedsLoadForServer@UObject@@UEBA_NXZ", "48 83 EC 28 48 8B 51 10 48 8D 0D ? ? ? ? E8 ? ? ? ? 84 C0 0F 94 C0 48 83 C4 28 C3 CC CC 48 89 5C 24 08");
            FModuleManager::RegisterImport("?NeedsLoadForEditorGame@UObject@@UEBA_NXZ", NullSub);
            FModuleManager::RegisterImport("?NeedsLoadForClient@UObject@@UEBA_NXZ", "48 83 EC 28 48 8B 51 10 48 8D 0D ? ? ? ? E8 ? ? ? ? 84 C0 0F 94 C0 48 83 C4 28 C3 CC CC 48 83 EC 28");
            FModuleManager::RegisterImport("?Modify@UObject@@UEAA_N_N@Z", "48 89 74 24 10 57 48 83 EC 20 8B 41 08 0F B6 F2");
            FModuleManager::RegisterImport("?MarkAsEditorOnlySubobject@UObject@@UEAAXXZ", VoidSub);
            FModuleManager::RegisterImport("?IsSupportedForNetworking@UObject@@UEBA_NXZ", "48 8B 01 48 FF A0 B0 01 00 00");
            FModuleManager::RegisterImport("?IsSafeForRootSet@UObject@@UEBA_NXZ", "53 48 83 EC 20 48 89 CB E8 ? ? ? ? 84 C0 74 54");
            FModuleManager::RegisterImport("?IsReadyForFinishDestroy@UObject@@UEAA_NXZ", TrueSub);
            FModuleManager::RegisterImport("?IsPostLoadThreadSafe@UObject@@UEBA_NXZ", NullSub);
            FModuleManager::RegisterImport("?IsNameStableForNetworking@UObject@@UEBA_NXZ", "48 83 EC 28 F7 41 08 00 00 0C 00");
            FModuleManager::RegisterImport("?IsLocalizedResource@UObject@@UEBA_NXZ", "53 48 83 EC 30 48 89 7C 24 40 31 FF");
            FModuleManager::RegisterImport("?IsFullNameStableForNetworking@UObject@@UEBA_NXZ", "40 53 48 83 EC 20 48 8B D9 48 8B 49 20 48 85 C9 74 ? 48 8B 01");
            FModuleManager::RegisterImport("?IsEditorOnly@UObject@@UEBA_NXZ", "48 89 5C 24 08 57 48 83 EC 20 8B 41 08 48 8B D9 A8 50");
            FModuleManager::RegisterImport("?IsAsset@UObject@@UEBA_NXZ", "48 89 5C 24 08 57 48 83 EC 20 8B 41 08 48 8B D9 A8 50");
            FModuleManager::RegisterImport("?ImportCustomProperties@UObject@@UEAAXPEB_WPEAVFFeedbackContext@@@Z", VoidSub);
            FModuleManager::RegisterImport("?GetWorld@UObject@@UEBAPEAVUWorld@@XZ", NullSub);
            FModuleManager::RegisterImport("?GetSubobjectsWithStableNamesForNetworking@UObject@@UEAAXAEAV?$TArray@PEAVUObject@@VFDefaultAllocator@@@@@Z", VoidSub);
            FModuleManager::RegisterImport("?GetRestoreForUObjectOverwrite@UObject@@UEAAPEAVFRestoreForUObjectOverwrite@@XZ", NullSub);
            FModuleManager::RegisterImport("?GetResourceSizeEx@UObject@@UEAAXAEAUFResourceSizeEx@@@Z", VoidSub);
            FModuleManager::RegisterImport("?GetResourceSize@UObject@@UEAA_KW4Type@EResourceSizeMode@@@Z", "48 83 EC 58 31 C0 89 54 24 20 48 89 44 24 28 48 8D 54 24 20 48 89 44 24 30 48 89 44 24 38 48 89 44 24 40 48 89 44 24 48 48 8B 01 FF 90 50 01 00 00");
            FModuleManager::RegisterImport("?GetPrimaryAssetId@UObject@@UEBA?AUFPrimaryAssetId@@XZ", "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 83 3D ? ? ? ? ? 48 8B DA");
            FModuleManager::RegisterImport("?GetPrestreamPackages@UObject@@UEAAXAEAV?$TArray@PEAVUObject@@VFDefaultAllocator@@@@@Z", VoidSub);
            FModuleManager::RegisterImport("?GetCPPTypeForwardDeclaration@UBoolProperty@@UEBA?AVFString@@XZ", "33 C0 48 89 02 48 89 42 08 48 8B C2 C3 CC CC CC E9 ? ? ? ? CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC");
            FModuleManager::RegisterImport("?GetPreloadDependencies@UObject@@UEAAXAEAV?$TArray@PEAVUObject@@VFDefaultAllocator@@@@@Z", "48 89 5C 24 10 48 89 6C 24 18 57 48 83 EC 20 48 8B 79 10");
            FModuleManager::RegisterImport("?GetPreloadDependencies@UEnumProperty@@UEAAXAEAV?$TArray@PEAVUObject@@VFDefaultAllocator@@@@@Z", "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 48 83 EC 20 48 89 D3 48 89 CE");
            FModuleManager::RegisterImport("??0UDataAsset@@QEAA@AEAVFVTableHelper@@@Z", "40 53 48 83 EC 20 48 8B D9 E8 ? ? ? ? 48 8D 05 ? ? ? ? 48 89 03 48 C7 43 28 00 00 00 00");
            FModuleManager::RegisterImport("?GetPrivateStaticClass@UDataAsset@@CAPEAVUClass@@XZ", "48 81 EC 98 00 00 00 48 83 3D ? ? ? ? ? 0F 85 ? ? ? ? E8 ? ? ? ? 48 89 44 24 70 E8 ? ? ? ? 48 89 44 24 78 48 8D 0D ? ? ? ? 48 83 C1 02 48 89 8C 24 80 00 00 00 E8 ? ? ? ? C6 44 24 68 00 48 8D 0D ? ? ? ? 48 89 4C 24 60 48 8D 0D ? ? ? ? 48 89 4C 24 58 48 8D 0D ? ? ? ? 48 89 4C 24 50 48 8D 0D ? ? ? ? 48 89 4C 24 48 48 8D 0D ? ? ? ? 48 89 4C 24 40 48 8B 4C 24 70 48 89 4C 24 38 48 8B 4C 24 78 48 89 4C 24 30 C7 44 24 28 01 00 00 10 C7 44 24 20 30 00 00 00 4C 8D 0D ? ? ? ? 4C 8D 05 ? ? ? ? 48 8B 8C 24 80 00 00 00 48 8B D1 48 8B C8 E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 81 C4 98 00 00 00 C3 CC CC CC CC CC CC CC CC CC CC 48 81 EC 98 00 00 00 48 83 3D ? ? ? ? ? 0F 85 ? ? ? ? E8 ? ? ? ? 48 89 44 24 70 E8 ? ? ? ? 48 89 44 24 78 48 8D 0D ? ? ? ? 48 83 C1 02 48 89 8C 24 80 00 00 00 E8 ? ? ? ? C6 44 24 68 00 48 8D 0D ? ? ? ? 48 89 4C 24 60 48 8D 0D ? ? ? ? 48 89 4C 24 58 48 8D 0D ? ? ? ? 48 89 4C 24 50 48 8D 0D ? ? ? ? 48 89 4C 24 48 48 8D 0D ? ? ? ? 48 89 4C 24 40 48 8B 4C 24 70 48 89 4C 24 38 48 8B 4C 24 78 48 89 4C 24 30 C7 44 24 28 00 00 00 10 C7 44 24 20 80 00 00 00");
            FModuleManager::RegisterImport("?Z_Construct_UClass_UDataAsset@@YAPEAVUClass@@XZ", "48 81 EC E8 01 00 00 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 D0 01 00 00 C7 44 24 40 00 00 00 00 48 83 3D ? ? ? ? ? 0F 85 ? ? ? ? E8 ? ? ? ? E8 ? ? ? ? E8 ? ? ? ? 48 89 05 ? ? ? ? BA 00 00 00 20 48 8B 05 ? ? ? ? 8B 88 B4 00 00 00 E8 ? ? ? ? 8B C8 E8 ? ? ? ? 0F B6 C0 85 C0 0F 84 ? ? ? ? 48 8B 0D ? ? ? ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 05 B4 00 00 00 BA 81 00 08 20");
            FModuleManager::RegisterImport("?Z_Construct_UClass_UCurveFloat_NoRegister@@YAPEAVUClass@@XZ", "48 81 EC 98 00 00 00 48 83 3D ? ? ? ? ? 0F 85 ? ? ? ? E8 ? ? ? ? 48 89 44 24 70 E8 ? ? ? ? 48 89 44 24 78 48 8D 0D ? ? ? ? 48 83 C1 02 48 89 8C 24 80 00 00 00 E8 ? ? ? ? C6 44 24 68 00 48 8D 0D ? ? ? ? 48 89 4C 24 60 48 8D 0D ? ? ? ? 48 89 4C 24 58 48 8D 0D ? ? ? ? 48 89 4C 24 50 48 8D 0D ? ? ? ? 48 89 4C 24 48 48 8D 0D ? ? ? ? 48 89 4C 24 40 48 8B 4C 24 70 48 89 4C 24 38 48 8B 4C 24 78 48 89 4C 24 30 C7 44 24 28 00 00 00 10 C7 44 24 20 A8 00 00 00 4C 8D 0D ? ? ? ? 4C 8D 05 ? ? ? ? 48 8B 8C 24 80 00 00 00 48 8B D1 48 8B C8 E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 81 C4 98 00 00 00 C3 CC CC CC CC CC CC CC CC CC CC 48 81 EC 98 00 00 00 48 83 3D ? ? ? ? ? 0F 85 ? ? ? ? E8 ? ? ? ? 48 89 44 24 70 E8 ? ? ? ? 48 89 44 24 78 48 8D 0D ? ? ? ? 48 83 C1 02 48 89 8C 24 80 00 00 00 E8 ? ? ? ? C6 44 24 68 00 48 8D 0D ? ? ? ? 48 89 4C 24 60 48 8D 0D ? ? ? ? 48 89 4C 24 58 48 8D 0D ? ? ? ? 48 89 4C 24 50 48 8D 0D ? ? ? ? 48 89 4C 24 48 48 8D 0D ? ? ? ? 48 89 4C 24 40 48 8B 4C 24 70 48 89 4C 24 38 48 8B 4C 24 78 48 89 4C 24 30 C7 44 24 28 00 00 00 10 C7 44 24 20 F0 01 00 00");
            FModuleManager::RegisterImport("?Z_Construct_UScriptStruct_FBoneReference@@YAPEAVUScriptStruct@@XZ", "48 81 EC 68 03 00 00 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 50 03 00 00 C7 44 24 40 00 00 00 00 E8 ? ? ? ? 48 89 44 24 50 B8 18 00 00 00 8B C0 8B 0D ? ? ? ? 65 48 8B 14 25 58 00 00 00 48 8B 0C CA 8B 04 08 39 05 ? ? ? ? 7E ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 83 3D ? ? ? ? ? 75 ? E8 ? ? ? ? C6 44 24 20 00 44 8B C8 41 B8 18 00 00 00 48 8D 15 ? ? ? ? 48 8B 4C 24 50 E8 ? ? ? ? 48 89 05 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 83 3D ? ? ? ? ? 0F 85 ? ? ? ? 48 8D 84 24 98 00 00 00 48 89 84 24 88 00 00 00 B8 01 00 00 00 83 C8 40 83 C8 04 89 44 24 44 41 B8 01 00 00 00 48 8D 15 ? ? ? ? 48 8B 8C 24 88 00 00 00 E8 ? ? ? ? 8B 4C 24 44 89 4C 24 20 4C 8B 08 4C 8B 44 24 50 33 D2 B9 98 00 00 00 E8 ? ? ? ? 48 89 44 24 68 48 83 7C 24 68 00 74 ? B9 10 00 00 00 E8 ? ? ? ? 48 89 44 24 58 48 83 7C 24 58 00 74 ? 48 8B 4C 24 58 E8 ? ? ? ? 48 89 44 24 60 EB ? 48 C7 44 24 60 00 00 00 00 83 4C 24 40 01 48 8D 8C 24 B0 00 00 00 E8 ? ? ? ? 48 C7 44 24 30 00 00 00 00 48 C7 44 24 28 00 00 00 00 C7 44 24 20 01 00 00 00 4C 8B 4C 24 60 45 33 C0 48 8B D0 48 8B 4C 24 68 E8 ? ? ? ? 48 89 44 24 70 EB ? 48 C7 44 24 70 00 00 00 00 48 8B 44 24 70 48 89 05 ? ? ? ? 8B 44 24 40 83 E0 01 85 C0 74 ? 83 64 24 40 FE 48 8D 8C 24 B0 00 00 00 E8 ? ? ? ? 48 8D 84 24 A0 00 00 00 48 89 84 24 90 00 00 00 B8 01 00 00 00 83 C8 40 83 C8 04 89 44 24 48 41 B8 01 00 00 00 48 8D 15 ? ? ? ? 48 8B 8C 24 90 00 00 00 E8 ? ? ? ? 8B 4C 24 48 89 4C 24 20 4C 8B 08 4C 8B 05 ? ? ? ? 33 D2 B9 78 00 00 00 E8 ? ? ? ? 48 89 44 24 78 48 83 7C 24 78 00 74 ? 83 4C 24 40 02 48 8D 8C 24 00 02 00 00 E8 ? ? ? ? 48 B9 01 00 00 00 00 00 10 00");
            FModuleManager::RegisterImport("?Z_Construct_UScriptStruct_FBodyInstance@@YAPEAVUScriptStruct@@XZ", "B8 D8 47 00 00");
            FModuleManager::RegisterImport("?Z_Construct_UScriptStruct_FRuntimeFloatCurve@@YAPEAVUScriptStruct@@XZ", "48 81 EC E8 04 00 00 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 D8 04 00 00 C7 44 24 40 00 00 00 00 E8 ? ? ? ? 48 89 44 24 50 B8 18 00 00 00 8B C0 8B 0D ? ? ? ? 65 48 8B 14 25 58 00 00 00 48 8B 0C CA 8B 04 08 39 05 ? ? ? ? 7E ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 83 3D ? ? ? ? ? 75 ? E8 ? ? ? ? C6 44 24 20 00 44 8B C8 41 B8 78 00 00 00");
            FModuleManager::RegisterImport("?Update@FAnimNode_Base@@UEAAXAEBUFAnimationUpdateContext@@@Z", VoidSub);
            FModuleManager::RegisterImport("?RootInitialize@FAnimNode_Base@@MEAAXPEBUFAnimInstanceProxy@@@Z", VoidSub);
            FModuleManager::RegisterImport("?ResetDynamics@FAnimNode_Base@@UEAAXXZ", VoidSub);
            FModuleManager::RegisterImport("?PostCompile@FAnimNode_Base@@UEAAXPEBVUSkeleton@@@Z", VoidSub);
            FModuleManager::RegisterImport("?OverrideAsset@FAnimNode_Base@@UEAAXPEAVUAnimationAsset@@@Z", VoidSub);
            FModuleManager::RegisterImport("?OnInitializeAnimInstance@FAnimNode_Base@@MEAAXPEBUFAnimInstanceProxy@@PEBVUAnimInstance@@@Z", "48 8B 01 48 FF A0 98 00 00 00 CC CC CC CC CC CC 4C 89 44 24 18");
            FModuleManager::RegisterImport("?Initialize@FAnimNode_Base@@UEAAXAEBUFAnimationInitializeContext@@@Z", "4C 8B 02 48 8B C1");
            FModuleManager::RegisterImport("?GatherDebugData@FAnimNode_Base@@UEAAXAEAUFNodeDebugData@@@Z", VoidSub);
            FModuleManager::RegisterImport("?EvaluateComponentSpace@FAnimNode_Base@@UEAAXAEAUFComponentSpacePoseContext@@@Z", "48 8B 01 48 FF A0 88 00 00 00 CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC E9 ? ? ? ?");
            FModuleManager::RegisterImport("?Evaluate_AnyThread@FAnimNode_Base@@UEAAXAEAUFPoseContext@@@Z", VoidSub);
            FModuleManager::RegisterImport("?Evaluate@FAnimNode_Base@@UEAAXAEAUFPoseContext@@@Z", VoidSub);
            FModuleManager::RegisterImport("?CanUpdateInWorkerThread@FAnimNode_Base@@UEBA_NXZ", TrueSub);
            FModuleManager::RegisterImport("?CacheBones@FAnimNode_Base@@UEAAXAEBUFAnimationCacheBonesContext@@@Z", VoidSub);
            FModuleManager::RegisterImport("??0UDataAsset@@QEAA@AEBVFObjectInitializer@@@Z", "40 53 48 83 EC 20 48 8B D9 E8 ? ? ? ? 48 8D 05 ? ? ? ? 48 89 03 48 C7 43 28 00 00 00 00");
            FModuleManager::RegisterImport("?ConvertBoneSpaceTransformToCS@FAnimationRuntime@@SAXAEBUFTransform@@AEAU?$FCSPose@UFCompactPose@@@@AEAU2@UFCompactPoseBoneIndex@@W4EBoneControlSpace@@@Z", "48 8B C4 48 89 58 08 48 89 70 10 48 89 78 18 44 89 48 20 55 48 8D 68 E8");
            FModuleManager::RegisterImport("?ConvertCSTransformToBoneSpace@FAnimationRuntime@@SAXAEBUFTransform@@AEAU?$FCSPose@UFCompactPose@@@@AEAU2@UFCompactPoseBoneIndex@@W4EBoneControlSpace@@@Z", "48 89 5C 24 08 48 89 74 24 10 48 89 7C 24 18 44 89 4C 24 ?");
            FModuleManager::RegisterImport("?GetParentBoneIndex@?$FBaseCompactPose@VFAnimStackAllocator@@@@QEBA?AUFCompactPoseBoneIndex@@AEBU2@@Z", FBaseCompactPose_GetParentBoneIndex);
            FModuleManager::RegisterImport("?GetBoneContainer@?$FBaseCompactPose@VFAnimStackAllocator@@@@QEBAAEBUFBoneContainer@@XZ", "48 8B 41 10 C3 CC CC CC CC");
            FModuleManager::RegisterImport("?IsValidToEvaluate@FBoneReference@@QEBA_NAEBUFBoneContainer@@@Z", "83 79 08 FF 74 ?");
            FModuleManager::RegisterImport("?Initialize@FBoneReference@@QEAA_NAEBUFBoneContainer@@@Z", "48 89 5C 24 10 57 48 83 EC 50 48 8B FA 48 8B D9 48 8D 54 24 40");
            FModuleManager::RegisterImport("?GetCompactPoseIndexFromSkeletonIndex@FBoneContainer@@QEBA?AUFCompactPoseBoneIndex@@H@Z", FBoneContainer_GetCompactPoseIndexFromSkeletonIndex);
            FModuleManager::RegisterImport("?GetRefPoseCompactArray@FBoneContainer@@QEBAAEBV?$TArray@UFTransform@@VFDefaultAllocator@@@@XZ", FBoneContainer_GetRefPoseCompactArray);
            FModuleManager::RegisterImport("?GetSkeletonAsset@FBoneContainer@@QEBAPEAVUSkeleton@@XZ", FBoneContainer_GetSkeletonAsset);
            FModuleManager::RegisterImport("??0FHitResult@@QEAA@XZ", "40 53 48 83 EC 20 C7 41 5C FF FF FF FF");
            FModuleManager::RegisterImport("?GetWorld@UActorComponent@@UEBAPEAVUWorld@@XZ", "48 8B 81 C8 01 00 00 48 85 C0 0F 84 ? ? ? ? C3 CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC");
            FModuleManager::RegisterImport("??0FCollisionQueryParams@@QEAA@VFName@@AEBUTStatId@@_NPEBVAActor@@@Z", "48 89 5C 24 08 57 48 83 EC 20 48 8B 7C 24 50 33 C0");
            FModuleManager::RegisterImport("?AddIgnoredComponent@FCollisionQueryParams@@QEAAXPEBVUPrimitiveComponent@@@Z", "48 85 D2 0F 84 ? ? ? ? 48 89 5C 24 18 57 48 83 EC 20 48 8B 02 48 8B F9 48 8B CA 48 8B DA FF 90 80 04 00 00 FF C8 A9 FD FF FF FF 75 ? 48 89 6C 24 30");
            FModuleManager::RegisterImport("??1FCollisionQueryParams@@QEAA@XZ", FCollisionQueryParams_Destructor);
            FModuleManager::RegisterImport("??0FCollisionResponseParams@@QEAA@AEBUFCollisionResponseContainer@@@Z", FCollisionResponseParams_Constructor);
            FModuleManager::RegisterImport("?HasAnyData@FRichCurve@@QEBA_NXZ", FRichCurve_HasAnyData);
            FModuleManager::RegisterImport("?Eval@FRichCurve@@QEBAMMM@Z", "F3 0F 11 4C 24 10 53 48 83 EC 40");
            FModuleManager::RegisterImport("??0FRuntimeFloatCurve@@QEAA@XZ", "33 D2 48 8D 05 ? ? ? ? 48 89 51 08 48 89 51 10 48 89 51 28");
            FModuleManager::RegisterImport("?GetRichCurve@FRuntimeFloatCurve@@QEAAPEAUFRichCurve@@XZ", "48 8B 41 70 48 85 C0 74 ? 48 83 C0 30");
            FModuleManager::RegisterImport("??1FRuntimeFloatCurve@@QEAA@XZ", FRuntimeFloatCurve_Destructor);
            FModuleManager::RegisterImport("??0FRuntimeFloatCurve@@QEAA@AEBU0@@Z", FRuntimeCurveFloat_CopyConstructor);
            FModuleManager::RegisterImport("??0FRuntimeFloatCurve@@QEAA@$$QEAU0@@Z", FRuntimeCurveFloat_CopyConstructor);
            FModuleManager::RegisterImport("??4FRuntimeFloatCurve@@QEAAAEAU0@AEBU0@@Z", FRuntimeCurveFloat_OperatorEquals);
            FModuleManager::RegisterImport("??4FRuntimeFloatCurve@@QEAAAEAU0@$$QEAU0@@Z", FRuntimeCurveFloat_OperatorEquals);
            FModuleManager::RegisterImport("?SweepSingleByChannel@UWorld@@QEBA_NAEAUFHitResult@@AEBUFVector@@1AEBUFQuat@@W4ECollisionChannel@@AEBUFCollisionShape@@AEBUFCollisionQueryParams@@AEBUFCollisionResponseParams@@@Z", "48 83 EC 78 4C 8B DA 48 8B 94 24 B0 00 00 00 44 8B 12 41 83 EA 01 0F 84 ? ? ? ? 41 83 EA 01 74 ? 41 83 FA 01 0F 85 ? ? ? ? F3 0F 10 05 ? ? ? ? 0F 2F 42 04 0F 93 C0 84 C0 0F 85 ? ? ? ? 41 8B 41 08 F2 41 0F 10 01 4D 8B CB 89 44 24 68 41 8B 40 08 89 44 24 58 48 8D 05 ? ? ? ?");
            FModuleManager::RegisterImport("?SweepMultiByChannel@UWorld@@QEBA_NAEAV?$TArray@UFHitResult@@VFDefaultAllocator@@@@AEBUFVector@@1AEBUFQuat@@W4ECollisionChannel@@AEBUFCollisionShape@@AEBUFCollisionQueryParams@@AEBUFCollisionResponseParams@@@Z", "48 83 EC 78 4C 8B DA 48 8B 94 24 B0 00 00 00 44 8B 12 41 83 EA 01 0F 84 ? ? ? ? 41 83 EA 01 74 ? 41 83 FA 01 0F 85 ? ? ? ? F3 0F 10 05 ? ? ? ? 0F 2F 42 04 0F 93 C0 84 C0 0F 85 ? ? ? ? 41 8B 41 08 F2 41 0F 10 01 4D 8B CB 89 44 24 58");
            FModuleManager::RegisterImport("?GetResponseContainer@FCollisionResponse@@QEBAAEBUFCollisionResponseContainer@@XZ", "48 8B C1 C3 CC CC CC CC CC CC CC CC CC CC CC CC 48 89 5C 24 10");
            FModuleManager::RegisterImport("??0FBodyInstance@@QEAA@XZ", "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 48 83 EC 20 83 CE FF");
            FModuleManager::RegisterImport("??1FBodyInstance@@QEAA@XZ", "48 89 5C 24 08 57 48 83 EC 20 48 8B 99 20 02 00 00");
            FModuleManager::RegisterImport("??0FBodyInstance@@QEAA@AEBU0@@Z", "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 48 83 EC 20 0F B7 02 48 8B FA");
            FModuleManager::RegisterImport("??0FBodyInstance@@QEAA@$$QEAU0@@Z", "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 48 83 EC 20 0F B7 02 48 8B FA");
            FModuleManager::RegisterImport("??4FBodyInstance@@QEAAAEAU0@AEBU0@@Z", "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 48 83 EC 20 0F B7 02 48 8B FA");
            FModuleManager::RegisterImport("??4FBodyInstance@@QEAAAEAU0@$$QEAU0@@Z", "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 48 83 EC 20 0F B7 02 48 8B FA");
            FModuleManager::RegisterImport("??4FBodyInstance@@QEAAAEAU0@$$QEAU0@@Z", "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 48 83 EC 20 0F B7 02 48 8B FA");
            FModuleManager::RegisterImport("?ZeroVector@FVector@@2U1@B", &ZeroVector);
            FModuleManager::RegisterImport("?GetNames@FName@@CAAEAV?$TStaticIndirectArrayThreadSafeRead@UFNameEntry@@$0CAAAAA@$0EAAA@@@XZ", "48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? B9 08 04 00 00");
            FModuleManager::RegisterImport("?Quaternion@FRotator@@QEBA?AUFQuat@@XZ", "48 83 EC 18 F3 0F 10 41 08");
            FModuleManager::RegisterImport("?FindBetweenVectors@FQuat@@SA?AU1@AEBUFVector@@0@Z", "48 8B C4 48 83 EC 58 F3 0F 10 2A");
            FModuleManager::RegisterImport("?SetupSingleton@IConsoleManager@@CAXXZ", "48 83 EC 38 48 83 3D ? ? ? ? ? 75 ? B9 B8 00 00 00");
            FModuleManager::RegisterImport("??0FAutoConsoleObject@@IEAA@PEAVIConsoleObject@@@Z", "48 8D 05 ? ? ? ? 48 89 51 08 48 89 01 48 8B C1 C3 CC CC CC CC CC CC CC CC CC CC CC CC CC CC 40 57");
            FModuleManager::RegisterImport("??1FAutoConsoleObject@@MEAA@XZ", VoidSub);
            FModuleManager::RegisterImport("?Realloc@FMemory@@SAPEAXPEAX_KI@Z", FMemory::Realloc);
            FModuleManager::RegisterImport("?OneVector@FVector@@2U1@B", &OneVector);
            FModuleManager::RegisterImport("?Identity@FTransform@@2U1@B", &FTransformIdentity);
            FModuleManager::RegisterImport("?NewGuid@FGuid@@SA?AU1@XZ", NewGuid);
            FModuleManager::RegisterImport("?ZeroRotator@FRotator@@2U1@B", &ZeroRotator);
            FModuleManager::RegisterImport("?Free@FMemory@@SAXPEAX@Z", FMemory::Free);
            FModuleManager::RegisterImport("?QuantizeSize@FMemory@@SA_K_KI@Z", FMemory::QuantizeSize);
            FModuleManager::RegisterImport("??0ForAnyElementType@FHeapAllocator@@QEAA@XZ", "48 C7 01 00 00 00 00 48 8B C1 C3 CC CC CC CC CC 48 8D 05 ? ? ? ?");
            FModuleManager::RegisterImport("?Memcpy@FGenericPlatformString@@CAPEAXPEAXPEBX_K@Z", static_cast<void*(*)(void*,const void*,SIZE_T)>(FMemory::Memcpy));
            FModuleManager::RegisterImport("??1FString@@QEAA@XZ", FStringDestructor);
            FModuleManager::RegisterImport("??0FName@@QEAA@PEB_WW4EFindName@@@Z", FName_Constructor);
            FModuleManager::RegisterImport("?GCoreObjectArrayForDebugVisualizers@@3PEAVFFixedUObjectArray@@EA", (void*)((char*)Container::GetUObjectArray() + 0x10));
            FModuleManager::RegisterImport("?Malloc@FMemory@@SAPEAX_KI@Z", FMemory::Malloc);
            FModuleManager::RegisterImport("?Atan2@FGenericPlatformMath@@SAMMM@Z", "48 83 EC 18 0F 28 D9");
            FModuleManager::RegisterImport("?ClosestPointOnSegment@FMath@@SA?AUFVector@@AEBU2@00@Z", "48 83 EC 18 F3 0F 10 42 08");
            FModuleManager::RegisterImport("?PointDistToSegmentSquared@FMath@@SAMAEBUFVector@@00@Z", "40 53 48 83 EC 30 4D 8B C8 48 8B D9 4C 8B C2 48 8B D1 48 8D 4C 24 20 E8 ? ? ? ? F3 0F 10 13");
            FModuleManager::RegisterImport("?SegmentPlaneIntersection@FMath@@SA_NAEBUFVector@@0AEBUFPlane@@AEAU2@@Z", SegmentPlaneIntersection);
            FModuleManager::RegisterImport("?StartsWith@FString@@QEBA_NAEBV1@W4Type@ESearchCase@@@Z", "48 83 EC 28 8B 42 08 41 83 F8 01");
            FModuleManager::RegisterImport("?ToString@FName@@QEBA?AVFString@@XZ", "48 89 5C 24 08 57 48 83 EC 40 83 79 04 00");
            FModuleManager::RegisterImport("?GetNameTableForDebuggerVisualizers_MT@FName@@SAPEAPEAPEAUFNameEntry@@XZ", "48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 75 ? B9 08 04 00 00");
            FModuleManager::RegisterImport("??0FName@@QEAA@PEBDW4EFindName@@@Z", "40 53 48 83 EC 30 48 8B D9 48 85 D2 74 ? 45 8B C8 C7 44 24 28 FF FF FF FF 45 33 C0 C6 44 24 20 01 E8 ? ? ? ? 48 8B C3 48 83 C4 30 5B C3 33 C0 48 89 44 24 40 48 89 01 48 8B C3 48 83 C4 30 5B C3 CC CC CC CC CC CC CC CC CC CC CC CC CC CC 40 53");
            FModuleManager::RegisterImport("?Z_Construct_UScriptStruct_FAnimNode_SkeletalControlBase@@YAPEAVUScriptStruct@@XZ", "48 81 EC 68 09 00 00 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 58 09 00 00 C7 44 24 40 00 00 00 00 E8 ? ? ? ? 48 89 44 24 60 B8 18 00 00 00 8B C0 8B 0D ? ? ? ? 65 48 8B 14 25 58 00 00 00 48 8B 0C CA 8B 04 08 39 05 ? ? ? ? 7E ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 83 3D ? ? ? ? ? 75 ? E8 ? ? ? ? C6 44 24 20 00 44 8B C8 41 B8 70 00 00 00 48 8D 15 ? ? ? ? 48 8B 4C 24 60 E8 ? ? ? ? 48 89 05 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 83 3D ? ? ? ? ? 0F 85 ? ? ? ? 48 8D 84 24 20 01 00 00 48 89 84 24 D8 00 00 00 B8 01 00 00 00 83 C8 40 83 C8 04 89 44 24 44 41 B8 01 00 00 00 48 8D 15 ? ? ? ? 48 8B 8C 24 D8 00 00 00 E8 ? ? ? ? 8B 4C 24 44 89 4C 24 20 4C 8B 08 4C 8B 44 24 60 33 D2 B9 98 00 00 00 E8 ? ? ? ? 48 89 44 24 78 48 83 7C 24 78 00 0F 84 ? ? ? ?");
            FModuleManager::RegisterImport("?Update_AnyThread@FAnimNode_SkeletalControlBase@@UEAAXAEBUFAnimationUpdateContext@@@Z", "48 89 5C 24 08 57 48 83 EC 20 48 89 CB 48 89 D7 48 83 C1 30");
            FModuleManager::RegisterImport("?EvaluateComponentSpace_AnyThread@FAnimNode_SkeletalControlBase@@UEAAXAEAUFComponentSpacePoseContext@@@Z", "48 89 5C 24 10 56 48 83 EC 20 48 8B D9 48 8B F2 48 83 C1 30");
            FModuleManager::RegisterImport("?EvaluateComponentSpaceInternal@FAnimNode_SkeletalControlBase@@MEAAXAEAUFComponentSpacePoseContext@@@Z", VoidSub);
            FModuleManager::RegisterImport("?EvaluateBoneTransforms@FAnimNode_SkeletalControlBase@@MEAAXPEAVUSkeletalMeshComponent@@AEAU?$FCSPose@UFCompactPose@@@@AEAV?$TArray@UFBoneTransform@@VFDefaultAllocator@@@@@Z", VoidSub);
            FModuleManager::RegisterImport("??4FAnimNode_SkeletalControlBase@@QEAAAEAU0@$$QEAU0@@Z", "48 89 5C 24 08 57 48 83 EC 20 48 8B 42 08 48 8B DA 48 89 41 08 48 8B F9 48 83 C1 10 48 83 C2 10 E8 ? ? ? ? 48 8B 43 20 48 8D 53 60");
            FModuleManager::RegisterImport("??4FAnimNode_SkeletalControlBase@@QEAAAEAU0@AEBU0@@Z", "48 89 5C 24 08 57 48 83 EC 20 48 8B 42 08 48 8B DA 48 89 41 08 48 8B F9 48 83 C1 10 48 83 C2 10 E8 ? ? ? ? 48 8B 43 20 48 8D 53 60");
            FModuleManager::RegisterImport("??0FAnimNode_SkeletalControlBase@@QEAA@$$QEAU0@@Z", "48 89 5C 24 08 57 48 83 EC 20 48 8B 42 08 48 8B DA 48 89 41 08 48 8B F9 48 83 C1 10 48 83 C2 10 E8 ? ? ? ? 48 8B 43 20 48 8D 53 60");
            FModuleManager::RegisterImport("??0FAnimNode_SkeletalControlBase@@QEAA@AEBU0@@Z", "48 89 5C 24 08 57 48 83 EC 20 48 8B 42 08 48 8B DA 48 89 41 08 48 8B F9 48 83 C1 10 48 83 C2 10 E8 ? ? ? ? 48 8B 43 20 48 8D 53 60");
            FModuleManager::RegisterImport("??1FAnimNode_SkeletalControlBase@@UEAA@XZ", VoidSub);
            FModuleManager::RegisterImport("?UpdateInternal@FAnimNode_SkeletalControlBase@@MEAAXAEBUFAnimationUpdateContext@@@Z", VoidSub);
            FModuleManager::RegisterImport("?CacheBones_AnyThread@FAnimNode_SkeletalControlBase@@UEAAXAEBUFAnimationCacheBonesContext@@@Z", "48 89 5C 24 08 57 48 83 EC 20 48 8B 01 48 8B FA 48 8B 12");
            FModuleManager::RegisterImport("?Initialize_AnyThread@FAnimNode_SkeletalControlBase@@UEAAXAEBUFAnimationInitializeContext@@@Z", "48 89 5C 24 08 57 48 83 EC 20 48 8B FA 48 8B D9 E8 ? ? ? ? 48 8D 4B 30 48 8B D7 48 8B 5C 24 30");
            FModuleManager::RegisterImport("??0FAnimNode_SkeletalControlBase@@QEAA@XZ", FAnimNode_SkeletalControlBase_Constructor);
            FModuleManager::RegisterImport("?Z_Construct_UEnum_AnimGraphRuntime_ESphericalLimitType@@YAPEAVUEnum@@XZ", "40 56 57 48 81 EC 98 02 00 00 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 88 02 00 00 C7 44 24 30 00 00 00 00 E8 ? ? ? ? 48 89 84 24 B0 00 00 00 B8 18 00 00 00 8B C0 8B 0D ? ? ? ? 65 48 8B 14 25 58 00 00 00 48 8B 0C CA 8B 04 08 39 05 ? ? ? ? 7E ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 83 3D ? ? ? ? ? 75 ? E8 ? ? ? ? C6 44 24 20 00 44 8B C8 45 33 C0 48 8D 15 ? ? ? ? 48 8B 8C 24 B0 00 00 00 E8 ? ? ? ? 48 89 05 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 83 3D ? ? ? ? ? 0F 85 ? ? ? ? 48 8D 84 24 30 01 00 00 48 89 84 24 E0 00 00 00 B8 01 00 00 00 83 C8 40 83 C8 04 89 84 24 90 00 00 00 41 B8 01 00 00 00 48 8D 15 ? ? ? ? 48 8B 8C 24 E0 00 00 00 E8 ? ? ? ? 8B 8C 24 90 00 00 00 89 4C 24 20 4C 8B 08 4C 8B 84 24 B0 00 00 00 33 D2 B9 60 00 00 00 E8 ? ? ? ? 48 89 84 24 B8 00 00 00 48 83 BC 24 B8 00 00 00 00 74 ? 83 4C 24 30 01 48 8D 8C 24 38 01 00 00 E8 ? ? ? ? 48 8B D0 48 8B 8C 24 B8 00 00 00 E8 ? ? ? ? 48 89 84 24 C0 00 00 00 EB ? 48 C7 84 24 C0 00 00 00 00 00 00 00 48 8B 84 24 C0 00 00 00 48 89 05 ? ? ? ? 8B 44 24 30 83 E0 01 85 C0 74 ? 83 64 24 30 FE 48 8D 8C 24 38 01 00 00 E8 ? ? ? ? 48 8D 44 24 38 48 8B C8 E8 ? ? ? ? C7 44 24 40 00 00 00 00 C7 44 24 44 00 00 00 00 C7 84 24 98 00 00 00 00 00 00 00 8B 44 24 40 89 44 24 50 8B 44 24 40 FF C0 89 44 24 4C 8B 44 24 4C 89 44 24 40 8B 44 24 44 39 44 24 4C 7E ? 8B 54 24 50 48 8D 4C 24 38 E8 ? ? ? ? 8B 44 24 50 89 84 24 94 00 00 00 48 8B 44 24 38 48 89 84 24 E8 00 00 00 48 63 84 24 94 00 00 00 48 6B C0 10 48 8B 8C 24 E8 00 00 00 48 03 C8 48 8B C1 48 8B D0 B9 10 00 00 00 E8 ? ? ? ? 48 89 84 24 C8 00 00 00 48 83 BC 24 C8 00 00 00 00 74 ? 4C 8D 84 24 98 00 00 00 48 8D 15 ? ? ? ? 48 8B 8C 24 C8 00 00 00 E8 ? ? ? ? 48 89 84 24 F0 00 00 00 EB ? 48 C7 84 24 F0 00 00 00 00 00 00 00 C7 84 24 A0 00 00 00 01 00 00 00 8B 44 24 40 89 44 24 58 8B 44 24 40 FF C0 89 44 24 54 8B 44 24 54 89 44 24 40 8B 44 24 44 39 44 24 54 7E ? 8B 54 24 58 48 8D 4C 24 38 E8 ? ? ? ? 8B 44 24 58 89 84 24 9C 00 00 00 48 8B 44 24 38 48 89 84 24 F8 00 00 00 48 63 84 24 9C 00 00 00 48 6B C0 10 48 8B 8C 24 F8 00 00 00 48 03 C8 48 8B C1 48 8B D0 B9 10 00 00 00 E8 ? ? ? ? 48 89 84 24 D0 00 00 00 48 83 BC 24 D0 00 00 00 00 74 ? 4C 8D 84 24 A0 00 00 00 48 8D 15 ? ? ? ? 48 8B 8C 24 D0 00 00 00 E8 ? ? ? ? 48 89 84 24 00 01 00 00 EB ? 48 C7 84 24 00 01 00 00 00 00 00 00 C7 84 24 A8 00 00 00 02 00 00 00 8B 44 24 40 89 44 24 60 8B 44 24 40 FF C0 89 44 24 5C 8B 44 24 5C 89 44 24 40 8B 44 24 44 39 44 24 5C 7E ? 8B 54 24 60 48 8D 4C 24 38 E8 ? ? ? ? 8B 44 24 60 89 84 24 A4 00 00 00 48 8B 44 24 38 48 89 84 24 08 01 00 00 48 63 84 24 A4 00 00 00 48 6B C0 10 48 8B 8C 24 08 01 00 00 48 03 C8 48 8B C1 48 8B D0 B9 10 00 00 00 E8 ? ? ? ? 48 89 84 24 D8 00 00 00 48 83 BC 24 D8 00 00 00 00 74 ? 4C 8D 84 24 A8 00 00 00 48 8D 15 ? ? ? ? 48 8B 8C 24 D8 00 00 00 E8 ? ? ? ? 48 89 84 24 10 01 00 00 EB ? 48 C7 84 24 10 01 00 00 00 00 00 00 48 8B 05 ? ? ? ? 48 8B 00 41 B1 01 41 B8 02 00 00 00 48 8D 54 24 38 48 8B 0D ? ? ? ? FF 90 58 02 00 00 48 8D 05 ? ? ? ? 48 89 44 24 70 48 8B 05 ? ? ? ? 48 83 C0 30 48 89 84 24 80 00 00 00 48 8B 84 24 80 00 00 00 48 8B 00 48 89 84 24 18 01 00 00 48 8B 44 24 70 48 39 84 24 18 01 00 00 0F 84 ? ? ? ? 48 83 7C 24 70 00 74 ? 48 8B 44 24 70 0F B7 00 85 C0 74 ? 48 8B 44 24 70 48 89 84 24 20 01 00 00 48 C7 84 24 88 00 00 00 FF FF FF FF 48 FF 84 24 88 00 00 00 48 8B 84 24 20 01 00 00 48 8B 8C 24 88 00 00 00 66 83 3C 48 00 75 ? 48 8B 84 24 88 00 00 00 FF C0 89 44 24 64 EB ? C7 44 24 64 00 00 00 00 8B 44 24 64 89 44 24 48 48 8B 84 24 80 00 00 00 8B 54 24 48 48 8B C8 E8 ? ? ? ? 48 8B 84 24 80 00 00 00 48 89 44 24 78 48 8B 44 24 78 8B 40 08 89 84 24 AC 00 00 00 48 8B 44 24 78 8B 4C 24 48 8B 40 08 03 C1 89 44 24 68 48 8B 44 24 78 8B 4C 24 68 89 48 08 48 8B 44 24 78 8B 40 0C 39 44 24 68 7E ? 8B 94 24 AC 00 00 00 48 8B 4C 24 78 E8 ? ? ? ? 83 7C 24 48 00 74 ? 48 8B 84 24 80 00 00 00 48 8B 00 48 89 84 24 28 01 00 00 48 63 44 24 48 48 D1 E0 48 8B BC 24 28 01 00 00 48 8B 74 24 70 48 8B C8 F3 A4 48 8D 4C 24 38 E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 8B 8C 24 88 02 00 00 48 33 CC E8 ? ? ? ? 48 81 C4 98 02 00 00 5F 5E C3 CC CC CC CC CC CC CC CC CC 40 56 57 48 81 EC B8 02 00 00 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 A8 02 00 00 C7 44 24 40 00 00 00 00 E8 ? ? ? ? 48 89 84 24 E8 00 00 00 B8 18 00 00 00 8B C0 8B 0D ? ? ? ? 65 48 8B 14 25 58 00 00 00 48 8B 0C CA 8B 04 08 39 05 ? ? ? ? 7E ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 83 3D ? ? ? ? ? 75 ? E8 ? ? ? ? C6 44 24 20 00 44 8B C8 45 33 C0 48 8D 15 ? ? ? ? 48 8B 8C 24 E8 00 00 00 E8 ? ? ? ? 48 89 05 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 83 3D ? ? ? ? ? 0F 85 ? ? ? ? 48 8D 84 24 50 01 00 00 48 89 84 24 F0 00 00 00 B8 01 00 00 00 83 C8 40 83 C8 04 89 84 24 B0 00 00 00 41 B8 01 00 00 00 48 8D 15 ? ? ? ? 48 8B 8C 24 F0 00 00 00 E8 ? ? ? ? 8B 8C 24 B0 00 00 00 89 4C 24 20 4C 8B 08 4C 8B 84 24 E8 00 00 00 33 D2 B9 60 00 00 00 E8 ? ? ? ? 48 89 84 24 B8 00 00 00 48 83 BC 24 B8 00 00 00 00 74 ? 83 4C 24 40 01 48 8D 8C 24 58 01 00 00 E8 ? ? ? ? 48 8B D0 48 8B 8C 24 B8 00 00 00 E8 ? ? ? ? 48 89 84 24 C0 00 00 00 EB ? 48 C7 84 24 C0 00 00 00 00 00 00 00 48 8B 84 24 C0 00 00 00 48 89 05 ? ? ? ? 8B 44 24 40 83 E0 01 85 C0 74 ? 83 64 24 40 FE 48 8D 8C 24 58 01 00 00 E8 ? ? ? ? 48 8D 44 24 30 48 8B C8 E8 ? ? ? ? C7 44 24 38 00 00 00 00 C7 44 24 3C 00 00 00 00 C7 84 24 90 00 00 00 01 00 00 00 8B 44 24 38 89 44 24 4C 8B 44 24 38 FF C0 89 44 24 48 8B 44 24 48 89 44 24 38 8B 44 24 3C 39 44 24 48 7E ? 8B 54 24 4C 48 8D 4C 24 30 E8 ? ? ? ? 8B 44 24 4C 89 84 24 B4 00 00 00 48 8B 44 24 30 48 89 84 24 F8 00 00 00 48 63 84 24 B4 00 00 00 48 6B C0 10 48 8B 8C 24 F8 00 00 00 48 03 C8 48 8B C1 48 8B D0 B9 10 00 00 00 E8 ? ? ? ? 48 89 84 24 C8 00 00 00 48 83 BC 24 C8 00 00 00 00 74 ? 4C 8D 84 24 90 00 00 00 48 8D 15 ? ? ? ? 48 8B 8C 24 C8 00 00 00 E8 ? ? ? ? 48 89 84 24 00 01 00 00 EB ? 48 C7 84 24 00 01 00 00 00 00 00 00 C7 84 24 98 00 00 00 02 00 00 00 8B 44 24 38 89 44 24 54 8B 44 24 38 FF C0 89 44 24 50 8B 44 24 50 89 44 24 38 8B 44 24 3C 39 44 24 50 7E ? 8B 54 24 54 48 8D 4C 24 30 E8 ? ? ? ? 8B 44 24 54 89 84 24 94 00 00 00 48 8B 44 24 30 48 89 84 24 08 01 00 00 48 63 84 24 94 00 00 00 48 6B C0 10 48 8B 8C 24 08 01 00 00 48 03 C8 48 8B C1 48 8B D0 B9 10 00 00 00 E8 ? ? ? ? 48 89 84 24 D0 00 00 00 48 83 BC 24 D0 00 00 00 00 74 ? 4C 8D 84 24 98 00 00 00 48 8D 15 ? ? ? ? 48 8B 8C 24 D0 00 00 00 E8 ? ? ? ? 48 89 84 24 10 01 00 00 EB ? 48 C7 84 24 10 01 00 00 00 00 00 00 C7 84 24 AC 00 00 00 03 00 00 00");

            SignatureContainer game_thread_id_initialized {
                {{"80 3D ? ? ? ? ? 48 8B D9 0F 84 ? ? ? ? FF 15 ? ? ? ?"}},
                    [&](const SignatureContainer & self) {
                        const intptr_t address = reinterpret_cast<intptr_t>(self.get_match_address() + 0x2);
                        FModuleManager::RegisterImport("?GIsGameThreadIdInitialized@@3_NA", (void*)(*reinterpret_cast<int*>(address) + address + 0x4));
                        return true;
                },
                [](SignatureContainer & self) {},
            };
            SignatureContainer consolemanager_singleton {
                {{"48 89 05 ? ? ? ? 48 8B C8 48 85 C0"}},
                    [&](const SignatureContainer & self) {
                        const intptr_t address = reinterpret_cast<intptr_t>(self.get_match_address() + 0x3);
                        FModuleManager::RegisterImport("?Singleton@IConsoleManager@@0PEAU1@EA", (void*)(*reinterpret_cast<int*>(address) + address + 0x4));
                        return true;
                },
                [](SignatureContainer & self) {},
            };
            SignatureContainer game_thread_id {
                {{"3B 05 ? ? ? ? 0F 94 C0 84 C0 0F 85 ? ? ? ? 48 8B 4B 08 48 8D 54 24 20"}},
                    [&](const SignatureContainer & self) {
                        const intptr_t address = reinterpret_cast<intptr_t>(self.get_match_address() + 0x2);
                        FModuleManager::RegisterImport("?GGameThreadId@@3IA", (void*)(*reinterpret_cast<int*>(address) + address + 0x4));
                        return true;
                },
                [](SignatureContainer & self) {},
            };
            SignatureContainer process_event{
                {{"7D ? 48 8D 0C 40 48 8B 05 ? ? ? ? 48 8D 14 C8 EB ? 49 8B D6 8B 42 08 C1 E8 1D A8 01 0F 85 ? ? ? ? F7 86 88 00 00 00 00 04 00 00"}},
                    [&](const SignatureContainer & self) {
                        if (!self.get_match_address())
                            Output::send(std::format(STR("Failed to resolve symbol '?ProcessEvent@UObject@@UEAAXPEAVUFunction@@PEAX@Z': Signature not found\n")));
                        else
                            FModuleManager::RegisterImport("?ProcessEvent@UObject@@UEAAXPEAVUFunction@@PEAX@Z", self.get_match_address() - 0x46);
                        return true;
                },
                [](SignatureContainer & self) {},
            };
            SignatureContainer fquat_identity {
                {{"0F 28 05 ? ? ? ? 0F 29 01 E9 ? ? ? ? 0F 28 CD"}},
                    [&](const SignatureContainer & self) {
                        const intptr_t address = reinterpret_cast<intptr_t>(self.get_match_address() + 0x3);
                        FModuleManager::RegisterImport("?Identity@FQuat@@2U1@B", (void*)(*reinterpret_cast<int*>(address) + address + 0x4));
                        return true;
                },
                [](SignatureContainer & self) {},
            };
            
            SinglePassScanner::SignatureContainerMap signature_containers = {
                {ScanTarget::MainExe, {game_thread_id_initialized, consolemanager_singleton, game_thread_id, process_event, fquat_identity}},
            };

            SinglePassScanner::start_scan(signature_containers);
        }
        
        auto on_unreal_init() -> void override
        {
            Hook::RegisterProcessEventPreCallback(&process_event_hook);
        }

        static inline bool bModuleLoaded;
        
        auto static process_event_hook(UObject* Context, UFunction* Function, void* Parms) -> void
        {
            if (bModuleLoaded) return;
            bModuleLoaded = true;
            if (!FModuleManager::InitializeWrapper()) return;
            
            SignatureContainer uobjectbase_destructor{
                {{"40 53 48 83 EC 30 48 8D 05 ? ? ? ? 48 8B D9 48 89 01 80 3D ? ? ? ? ?"}},
                    [&](const SignatureContainer & self) {
                        UObjectBase_Destuctor.assign_address(self.get_match_address());
                        return true;
                },
                [](SignatureContainer & self)
                {
                    if (!UObjectBase_Destuctor.get_function_address())
                    {
                        Output::send(std::format(STR("Failed to resolve symbol '??1UObjectBase@@UEAA@XZ': Signature not found\n")));
                    }
                },
            };
            
            SignatureContainer getparentboneindex_container{
                {{"48 8B 81 A0 00 00 00 4D 63 00"}},
                [&](const SignatureContainer & self) {
                    GetParentBoneIndex.assign_address(self.get_match_address());
                    return true;
                },
                [](SignatureContainer & self)
                {
                    if (!GetParentBoneIndex.get_function_address())
                    {
                        Output::send(std::format(STR("Failed to resolve symbol 'FBoneContainer::GetParentBoneIndex': Signature not found\n")));
                    }
                },
            };
            
            SignatureContainer frichcurve_copyconstructor{
                {{"48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 48 83 EC 20 48 8B D9 48 8D 05 ? ? ? ? 33 C9 48 89 4B 08"}},
                    [&](const SignatureContainer & self) {
                        FRichCurve_CopyConstructor.assign_address(self.get_match_address());
                        return true;
                },
                [](SignatureContainer & self)
                {
                    if (!FRichCurve_CopyConstructor.get_function_address())
                    {
                        Output::send(std::format(STR("Failed to resolve symbol 'FRichCurve(FRichCurve&)': Signature not found\n")));
                    }
                },
            };
            
            SignatureContainer fanimnode_skeletalcontrolbase_constructor{
                {{"48 85 D2 74 ? 31 C9 48 8D 05 ? ? ? ? 48 89 4A 08 48 89 4A 10 48 89 4A 18 48 89 4A 20 88 4A 28 48 89 02 C7 42 30 FF FF FF FF 48 89 4A 38 88 4A 40 C7 42 48 00 00 80 3F"}},
                    [&](const SignatureContainer & self) {
                        FAnimNode_SkeletalControlBase_Construct.assign_address(self.get_match_address());
                        return true;
                },
                [](SignatureContainer & self)
                {
                    if (!FAnimNode_SkeletalControlBase_Construct.get_function_address())
                    {
                        Output::send(std::format(STR("Failed to resolve symbol 'UScriptStruct::TCppStructOps<FAnimNode_SkeletalControlBase>::Construct': Signature not found\n")));
                    }
                },
            };
            
            SignatureContainer callfunctionbyname_container{
                {{"E8 ? ? ? ? 84 C0 0F 84 ? ? ? ? 44 39 B5 E8 00 00 00"}},
                    [&](const SignatureContainer & self) {
                        CallFunctionByNameWithArguments.assign_address(self.get_match_address() - 0x65);
                        return true;
                },
                [](SignatureContainer & self)
                {
                    if (!CallFunctionByNameWithArguments.get_function_address())
                    {
                        Output::send(std::format(STR("Failed to resolve symbol 'UObject::CallFunctionByNameWithArguments': Signature not found\n")));
                    }
                },
            };
            
            SinglePassScanner::SignatureContainerMap signature_containers = {
                {ScanTarget::MainExe, {uobjectbase_destructor, getparentboneindex_container, frichcurve_copyconstructor,
                    fanimnode_skeletalcontrolbase_constructor, callfunctionbyname_container}},
            };

            SinglePassScanner::start_scan(signature_containers);

            
            register_symbols();
            
            const auto module_directory = std::filesystem::path(UE4SSProgram::get_program().get_module_directory());
            const auto plugin_directory = module_directory.parent_path().parent_path().parent_path() / "Plugins" / "KawaiiPhysics" / "Binaries" / "Win64";

            if (!exists(plugin_directory)) return;
            
            SetDllDirectory(plugin_directory.c_str());

            const auto dll_path = plugin_directory.wstring() + STR("\\RED-KawaiiPhysics-Win64-Shipping.dll");

            FModuleManager::Get().AddModule(FName(STR("KawaiiPhysics"), FNAME_Add), FString(dll_path.c_str()));
            FModuleManager::Get().LoadModule(FName(STR("KawaiiPhysics")));
        }
    };//class
}

/**
* export the start_mod() and uninstall_mod() functions to
* be used by the core ue4ss system to load in our dll mod
*/
#define KAWAIIPHYSICS_API __declspec(dllexport) 
extern "C" {
    KAWAIIPHYSICS_API CppUserModBase* start_mod(){ return new KawaiiPhysics::KawaiiPhysicsDBFZ(); }
    KAWAIIPHYSICS_API void uninstall_mod(CppUserModBase* mod) { delete mod; }
}