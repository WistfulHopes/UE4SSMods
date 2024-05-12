#include <DynamicOutput/Output.hpp>
#include <Mod/CppUserModBase.hpp>
#include <polyhook2/Detour/x64Detour.hpp>
#include <SigScanner/SinglePassSigScanner.hpp>
#include <Unreal/UObjectGlobals.hpp>

#include "AActor.hpp"
#include "BPMacros.hpp"
#include "UAssetRegistryHelpers.hpp"
#include "UE4SSProgram.hpp"
#include "UFunction.hpp"
#include "World.hpp"

namespace PLH
{
    class x64Detour;
}

namespace GGSTMods
{
    using namespace RC;
    using namespace Unreal;
    
    enum SIDE_ID : int32
    {
        SIDE_BEGIN = 0x0,
        SIDE_1P = 0x0,
        SIDE_2P = 0x1,
        SIDE_ID_NUM = 0x2,
        SIDE_COMMON = 0x2,
        SIDE_ID_NUM_WITH_COMMON = 0x3,
        SIDE_ID_INVALID = 0x4,
    };
    
    enum EMemberID : int32
    {
        MemberID_Begin = 0x0,
        MemberID_01 = 0x0,
        MemberID_02 = 0x1,
        MemberID_03 = 0x2,
        MemberID_MAX = 0x3,
        MemberID_INVALID = 0x4,
      };

    struct IDConvertTable
    {
        int SourceID;
        int DestID;
    };
    
    enum EBattleScript : int32
    {
        BATTLE_SCRIPT_DEFAULT = 0x0,
        BATTLE_SCRIPT_BLACK = 0x1,
        BATTLE_SCRIPT_BOSS = 0x2,
        BATTLE_SCRIPT_EXBOSS = 0x3,
        BATTLE_SCRIPT_MAX = 0x4,
    };

    struct FSpawnPlayerInfo
    {
        SIDE_ID SideID;
        EMemberID MemberID;
        FString CharaID;
        TArray<FString> SelfTeamMemberCharaID;
        TArray<FString> EnemyTeamMemberCharaID;
        TArray<FString> BGLayerNames;
        int CostumeID;
        int ColorID;
        EBattleScript ScriptType;
        FString VoiceLoc;
        bool bREV2ModelFlag;
        const IDConvertTable *CostumeIDConvTable;
        const IDConvertTable *ColorIDConvTable;
        int CostumeIDConvTableNum;
        int ColorIDConvTableNum;
    };
    
    struct FREDBattleVoiceCueInfo
    {
        bool bEnable;
        UObject* DefaultCue;
        TArray<UObject*> VsCue;
    };

    /**
    * GGSTBPPlayer: UE4SS c++ mod class defintion
    */
    class GGSTBPPlayer : public CppUserModBase
    {
    public:
        // constructor
        GGSTBPPlayer() {
            ModVersion = STR("0.1");
            ModName = STR("GGSTBPPlayer");
            ModAuthors = STR("UE4SS");
            ModDescription = STR("A basic template C++ mod");
            // Do not change this unless you want to target a UE4SS version
            // other than the one you're currently building with somehow.
            //ModIntendedSDKVersion = STR("2.6");
        }
        
        static inline void(*AREDPawnPlayer_AllocInstance)(AActor*);
        static inline void(*UREDCharaDarkAreaTableManager_SetTableData)(UObject*, UObject*);
        static inline void(*AREDPawnPlayer_InitReservedEffectPawns)(AActor*, int);
        static inline void(*UREDSoundPlayer_InitializeBattleVoice)(UObject*, unsigned int);
        static inline void(*AREDPawn_SetupMesh)(AActor*, UObject*, UObject*, UObject*, UObject*, TArray<UObject*>, UObject*, TArray<UObject*>);
        static inline void(*AREDPawn_SetupPTCColorAndMaterial)(AActor*, UObject*, UObject*);
        static inline void(*AREDPawn_SetBoundsScale)(AActor*, float);
        static inline void(*AREDPawn_SetupLinkBone)(AActor*);

        PLH::x64Detour* ChangeAnime_Detour {};
        static inline bool(*ChangeAnime_Orig)(AActor*, FName, bool, float, float);
        static bool ChangeAnime_Hook(AActor* pThis, FName Name, bool isLoop, float StartTime, float BlendTime)
        {
            if (const auto Function = pThis->GetFunctionByNameInChain(STR("SetFlipbook")))
            {
                static auto ParamStructSize = Function->GetParmsSize();
                const auto ParamData = static_cast<uint8*>(_malloca(ParamStructSize));
                FMemory::Memzero(ParamData, ParamStructSize);

                UE_COPY_PROPERTY(Name, FName)

                pThis->ProcessEvent(Function, ParamData);
                return true;
            }
            return ChangeAnime_Orig(pThis, Name, isLoop, StartTime, BlendTime);
        }
        
        PLH::x64Detour* SetAnimeFrame_Detour {};
        static inline void(*SetAnimeFrame_Orig)(AActor*, int);
        static void SetAnimeFrame_Hook(AActor* pThis, int frame)
        {
            if (const auto Function = pThis->GetFunctionByNameInChain(STR("SetFrame")))
            {
                static auto ParamStructSize = Function->GetParmsSize();
                const auto ParamData = static_cast<uint8*>(_malloca(ParamStructSize));
                FMemory::Memzero(ParamData, ParamStructSize);

                UE_COPY_PROPERTY(frame, int)

                pThis->ProcessEvent(Function, ParamData);
            }
            return SetAnimeFrame_Orig(pThis, frame);
        }
        
        PLH::x64Detour* SpawnPlayer_Detour {};
        static inline AActor*(*SpawnPlayer_Orig)(AActor*, const FSpawnPlayerInfo*);
        static AActor* SpawnPlayer_Hook(AActor* pGameState, const FSpawnPlayerInfo* info)
        {
            const auto AssetName = FName(std::format(STR("/Game/Chara/{}/Costume{:02}/CharaBP.CharaBP_C"), *((TArray<TCHAR>*)&info->CharaID)->GetData(), info->CostumeID + 1), FNAME_Add);
            auto AssetData = FAssetData();
            AssetData.SetObjectPath(AssetName);
            const auto Asset = UAssetRegistryHelpers::GetAsset(AssetData);

            if (!Asset) return SpawnPlayer_Orig(pGameState, info);

            const auto Player = pGameState->GetWorld()->SpawnActor(reinterpret_cast<UClass*>(Asset), nullptr, nullptr);
            if (!Player) return SpawnPlayer_Orig(pGameState, info);

            if (Player->GetPropertyByNameInChain(STR("Colors")))
            {
                const auto Colors = *Player->GetPropertyByNameInChain(STR("Colors"))->ContainerPtrToValuePtr<TArray<UObject*>>(Player);
                if (Colors.Num() > info->ColorID)
                {
                    *Player->GetPropertyByNameInChain(STR("PawnMaterials"))->ContainerPtrToValuePtr<UObject*>(Player) = Colors[info->ColorID];
                }
            }
            if (Player->GetPropertyByNameInChain(STR("PTC")))
            {
                const auto PTC = *Player->GetPropertyByNameInChain(STR("PTC"))->ContainerPtrToValuePtr<TArray<UObject*>>(Player);
                if (PTC.Num() > info->ColorID)
                {
                    *Player->GetPropertyByNameInChain(STR("PTCColorAndMaterial"))->ContainerPtrToValuePtr<UObject*>(Player) = PTC[info->ColorID];
                }
            }
            
            AREDPawnPlayer_AllocInstance(Player);
            
            if (const auto DarkAreaTableManagerClass = reinterpret_cast<UClass*>(UObjectGlobals::StaticFindObject(nullptr, nullptr,
                STR("/Script/RED.REDCharaDarkAreaTableManager"))))
            {
                const auto Params = FStaticConstructObjectParameters(DarkAreaTableManagerClass, Player);
                const auto DarkAreaTableManager = UObjectGlobals::StaticConstructObject(Params);
                *Player->GetPropertyByNameInChain(STR("DarkAreaSetting"))->ContainerPtrToValuePtr<UObject*>(Player) = DarkAreaTableManager;
                UREDCharaDarkAreaTableManager_SetTableData(DarkAreaTableManager, *Player->GetPropertyByNameInChain(STR("DarkAreaTable"))->ContainerPtrToValuePtr<UObject*>(Player));
            }
            AREDPawnPlayer_InitReservedEffectPawns(Player, info->MemberID + 3 * info->SideID);
            UREDSoundPlayer_InitializeBattleVoice(*Player->GetPropertyByNameInChain(STR("Voice"))->ContainerPtrToValuePtr<UObject*>(Player), 10);
            
            if (Player->GetPropertyByNameInChain(STR("VoiceCues")))
            {
                const auto Voice = *Player->GetPropertyByNameInChain(STR("Voice"))->ContainerPtrToValuePtr<UObject*>(Player);
                const auto BattleVoice = Voice->GetPropertyByNameInChain(STR("BattleVoice"))->ContainerPtrToValuePtr<TArray<FREDBattleVoiceCueInfo>>(Voice);
                auto VoiceCues = *Player->GetPropertyByNameInChain(STR("VoiceCues"))->ContainerPtrToValuePtr<TArray<UObject*>>(Player);
                for (int i = 0; i < 10; i++)
                {
                    if (i >= VoiceCues.Num()) break;
                    FREDBattleVoiceCueInfo VoiceCueInfo;
                    VoiceCueInfo.bEnable = true;
                    VoiceCueInfo.DefaultCue = VoiceCues[i];
                    VoiceCueInfo.VsCue = TArray<UObject*>();
                    (*BattleVoice)[i] = VoiceCueInfo;
                }
            }
            if (Player->GetPropertyByNameInChain(STR("PrivateSECues")))
            {
                const auto PrivateSE = *Player->GetPropertyByNameInChain(STR("PrivateSE"))->ContainerPtrToValuePtr<UObject*>(Player);
                *PrivateSE->GetPropertyByNameInChain(STR("Cues"))->ContainerPtrToValuePtr<UObject*>(PrivateSE) 
                    = *Player->GetPropertyByNameInChain(STR("PrivateSECues"))->ContainerPtrToValuePtr<UObject*>(Player);
            }
            
            AREDPawn_SetupMesh(Player,
                *pGameState->GetPropertyByNameInChain(STR("MaterialInstancePool"))->ContainerPtrToValuePtr<UObject*>(pGameState),
                *Player->GetPropertyByNameInChain(STR("MeshArray"))->ContainerPtrToValuePtr<UObject*>(Player),
                *Player->GetPropertyByNameInChain(STR("AnimArray"))->ContainerPtrToValuePtr<UObject*>(Player),
                *Player->GetPropertyByNameInChain(STR("PawnMaterials"))->ContainerPtrToValuePtr<UObject*>(Player),
                *Player->GetPropertyByNameInChain(STR("VSAnimArrayList"))->ContainerPtrToValuePtr<TArray<UObject*>>(Player),
                *Player->GetPropertyByNameInChain(STR("HandAnimArray"))->ContainerPtrToValuePtr<UObject*>(Player),
                TArray<UObject*>());
            
            const auto REDPTCMaterialAppendDataClass = reinterpret_cast<UClass*>(UObjectGlobals::StaticFindObject(nullptr, nullptr,
                STR("/Script/RED.REDPTCMaterialAppendData")));
            auto Append = UObjectGlobals::StaticFindObject(REDPTCMaterialAppendDataClass, reinterpret_cast<UObject*>(-1), STR("/Game/Common/CMN/Common/Effect/Particles/AppendMaterialData.AppendMaterialData"));
            AREDPawn_SetupPTCColorAndMaterial(Player, *Player->GetPropertyByNameInChain(STR("PTCColorAndMaterial"))->ContainerPtrToValuePtr<UObject*>(Player), Append);
            AREDPawn_SetBoundsScale(Player, 40000.0);
            AREDPawn_SetupLinkBone(Player);
            
            return Player;
        }

        // destructor
        ~GGSTBPPlayer() override {
            // fill when required
        }
        
        auto on_program_start() -> void override
        {
        }

        auto on_dll_load(std::wstring_view dll_name) -> void override
        {
        }
        
        auto on_unreal_init() -> void override
        {
            const SignatureContainer alloc_instance{
                {{"48 89 5C 24 10 57 48 83 EC 50 48 8D 15 ? ? ? ? 48 8B D9 E8 ? ? ? ? 33 FF 48 89 7C 24 60 E8 ? ? ? ? 40 88 7C 24 40 45 33 C9 48 89 7C 24 38 44 8B C7 40 88 7C 24 30 48 8B D3 48 89 7C 24 28 48 8B C8 89 7C 24 20 E8 ? ? ? ? 41 B0 01"}},
                    [&](const SignatureContainer & self) {
                        AREDPawnPlayer_AllocInstance = reinterpret_cast<void(*)(AActor*)>(self.get_match_address());
                        return true;
                },
                [](SignatureContainer & self)
                {
                },
            };
            const SignatureContainer set_table_data{
                {{"40 53 55 57 41 54 41 55 41 56 41 57 48 81 EC F0 00 00 00"}},
                    [&](const SignatureContainer & self) {
                        UREDCharaDarkAreaTableManager_SetTableData = reinterpret_cast<void(*)(UObject*, UObject*)>(self.get_match_address());
                        return true;
                },
                [](SignatureContainer & self)
                {
                },
            };
            const SignatureContainer init_reserved_effect_pawns{
                {{"40 56 48 83 EC 40 89 91 88 9C 02 00"}},
                    [&](const SignatureContainer & self) {
                        AREDPawnPlayer_InitReservedEffectPawns = reinterpret_cast<void(*)(AActor*, int)>(self.get_match_address());
                        return true;
                },
                [](SignatureContainer & self)
                {
                },
            };
            const SignatureContainer init_battle_voice{
                {{"48 8B C4 41 56 48 83 EC 60 48 89 58 08"}},
                    [&](const SignatureContainer & self) {
                        UREDSoundPlayer_InitializeBattleVoice = reinterpret_cast<void(*)(UObject*, unsigned int)>(self.get_match_address());
                        return true;
                },
                [](SignatureContainer & self)
                {
                },
            };
            const SignatureContainer setup_mesh{
                {{"4C 89 44 24 18 55 53 57 41 55 41 57 48 8D 6C 24 C9 48 81 EC 90 00 00 00"}},
                    [&](const SignatureContainer & self) {
                        AREDPawn_SetupPTCColorAndMaterial = reinterpret_cast<void(*)(AActor*, UObject*, UObject*)>(self.get_match_address());
                        return true;
                },
                [](SignatureContainer & self)
                {
                },
            };
            const SignatureContainer setup_ptc{
                {{"4D 85 C0 0F 84 ? ? ? ? 4C 8B DC 41 57"}},
                    [&](const SignatureContainer & self) {
                        AREDPawn_SetupMesh = reinterpret_cast<void(*)(AActor*, UObject*, UObject*, UObject*, UObject*, TArray<UObject*>, UObject*, TArray<UObject*>)>(self.get_match_address());
                        return true;
                },
                [](SignatureContainer & self)
                {
                },
            };
            const SignatureContainer set_bounds_scale{
                {{"33 C0 39 81 08 92 02 00"}},
                    [&](const SignatureContainer & self) {
                        AREDPawn_SetBoundsScale = reinterpret_cast<void(*)(AActor*, float)>(self.get_match_address());
                        return true;
                },
                [](SignatureContainer & self)
                {
                },
            };
            const SignatureContainer setup_link_bone{
                {{"41 54 41 55 41 57 48 83 EC 40 45 33 ED 4C 8B F9"}},
                    [&](const SignatureContainer & self) {
                        AREDPawn_SetupLinkBone = reinterpret_cast<void(*)(AActor*)>(self.get_match_address());
                        return true;
                },
                [](SignatureContainer & self)
                {
                },
            };
            const SignatureContainer spawn_player{
                {{"48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 60 48 8B 01 48 8B FA 48 8B F1"}},
                    [&](const SignatureContainer & self) {
                        SpawnPlayer_Detour = new PLH::x64Detour(
                            reinterpret_cast<uint64_t>(self.get_match_address()), reinterpret_cast<uint64_t>(&SpawnPlayer_Hook),
                            reinterpret_cast<uint64_t*>(&SpawnPlayer_Orig));
                        SpawnPlayer_Detour->hook();
                        return true;
                },
                [](SignatureContainer & self)
                {
                },
            };
            const SignatureContainer change_anime{
                {{"48 8B C4 53 56 41 56 48 83 EC 60"}},
                    [&](const SignatureContainer & self) {
                        ChangeAnime_Detour = new PLH::x64Detour(
                            reinterpret_cast<uint64_t>(self.get_match_address()), reinterpret_cast<uint64_t>(&ChangeAnime_Hook),
                            reinterpret_cast<uint64_t*>(&ChangeAnime_Orig));
                        ChangeAnime_Detour->hook();
                        return true;
                },
                [](SignatureContainer & self)
                {
                },
            };
            const SignatureContainer set_anime_frame{
                {{"48 89 6C 24 18 48 89 74 24 20 41 56 48 83 EC 20 33 F6 44 8B F2"}},
                    [&](const SignatureContainer & self) {
                        SetAnimeFrame_Detour = new PLH::x64Detour(
                            reinterpret_cast<uint64_t>(self.get_match_address()), reinterpret_cast<uint64_t>(&SetAnimeFrame_Hook),
                            reinterpret_cast<uint64_t*>(&SetAnimeFrame_Orig));
                        SetAnimeFrame_Detour->hook();
                        return true;
                },
                [](SignatureContainer & self)
                {
                },
            };
            
            std::vector<SignatureContainer> signature_containers;
            signature_containers.push_back(alloc_instance);
            signature_containers.push_back(set_table_data);
            signature_containers.push_back(init_reserved_effect_pawns);
            signature_containers.push_back(init_battle_voice);
            signature_containers.push_back(setup_mesh);
            signature_containers.push_back(setup_ptc);
            signature_containers.push_back(set_bounds_scale);
            signature_containers.push_back(setup_link_bone);
            signature_containers.push_back(spawn_player);
            signature_containers.push_back(change_anime);
            signature_containers.push_back(set_anime_frame);
      
            SinglePassScanner::SignatureContainerMap signature_containers_map;
            signature_containers_map.emplace(ScanTarget::MainExe, signature_containers);
            
            SinglePassScanner::start_scan(signature_containers_map);
        }
    };//class
}

/**
* export the start_mod() and uninstall_mod() functions to
* be used by the core ue4ss system to load in our dll mod
*/
#define GGSTBPPLAYER_EXPORT __declspec(dllexport) 
extern "C" {
    GGSTBPPLAYER_EXPORT CppUserModBase* start_mod(){ return new GGSTMods::GGSTBPPlayer(); }
    GGSTBPPLAYER_EXPORT void uninstall_mod(CppUserModBase* mod) { delete mod; }
}