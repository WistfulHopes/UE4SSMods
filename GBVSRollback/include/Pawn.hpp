#pragma once

#include "AActor.hpp"
#include "CXXBYTE.hpp"
#include "struct_util.hpp"

class APawn : public RC::Unreal::AActor
{
};

class AREDPawn : public APawn
{
public:
    FIELD(0xde98, int, MeshControlNum);
    FIELD(0xdead, bool, bIgnoreTick);
    FIELD(0xe388, RC::Unreal::FName, CurrentMeshSetName);
    FIELD(0xe390, RC::Unreal::FName, CurrentMaterialSetName);
    FIELD(0xe3b0, bool, bPlayingCutSceneAnime);
    FIELD(0xe3bc, float, CutSceneAnimeTime);
    FIELD(0xe3c0, class OBJ_CBase*, ObjPtr);
    FIELD(0xe3d4, int, CutSceneStepFrame);
};

class AREDPawnEffect : public AREDPawn
{
public:
    static RC::Unreal::UClass* StaticClass;
};

struct FPawnCacheKey
{
    RC::Unreal::int32 player;
    RC::Unreal::uint32 GameFrame;
    CXXBYTE<32> actname;
    CXXBYTE<32> PawnName;
    RC::Unreal::int32 Count;

    bool operator<(const FPawnCacheKey& rhs) const noexcept
    {
        constexpr std::hash<std::string> hasher;

        if (player < rhs.player) return true;
        if (player > rhs.player) return false;

        if (GameFrame < rhs.GameFrame) return true;
        if (GameFrame > rhs.GameFrame) return false;

        if (hasher(actname.m_Buf) < hasher(rhs.actname.m_Buf)) return true;
        if (hasher(actname.m_Buf) > hasher(rhs.actname.m_Buf)) return false;

        if (hasher(PawnName.m_Buf) < hasher(rhs.PawnName.m_Buf)) return true;
        if (hasher(PawnName.m_Buf) > hasher(rhs.PawnName.m_Buf)) return false;

        return Count < rhs.Count;
    }
};

struct Pawn_RollbackData
{
    std::map<FPawnCacheKey, AREDPawn*> pawnCache{};
    AREDPawn* PawnQuickRestoreCache[100]{};
};

extern Pawn_RollbackData pawnRollbackData;

typedef void (*ClearLinkModel_Func)(OBJ_CBase*);
extern ClearLinkModel_Func ClearLinkModel;

typedef void (*SetActorHiddenInGame_Func)(APawn*, bool);
extern SetActorHiddenInGame_Func SetActorHiddenInGame;
extern SetActorHiddenInGame_Func SetActorTickEnabled;

bool Rollback_ProcessCachedPawn(OBJ_CBase* ctx, CXXBYTE<32>* name, bool isCommon, bool effect, bool self = false);
AREDPawn* GetCachedPawn(OBJ_CBase* ctx, CXXBYTE<32>* name);
AREDPawn* GetCachedPawnForSet(OBJ_CBase* ctx);
bool AddLinkPawnToCache(OBJ_CBase* ctx);
bool AddLinkPawnForQuickRestore(OBJ_CBase* ctx);
void QuickRestoreLinkPawns();