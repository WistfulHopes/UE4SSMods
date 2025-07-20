#pragma once

#include "AActor.hpp"
#include "Enums.h"
#include "struct_util.h"
#include "../../SuzieMod/include/Suzie.hpp"

using namespace RC::Unreal;

using CharType = wchar_t;
using StringType = std::basic_string<CharType>;

class AGameStateBase : public AActor
{
    DECLARE_EXTERNAL_OBJECT_CLASS(AGameStateBase, RED)

private:
    char _padding[0x278];
};

class AREDGameState : public AGameStateBase
{
    DECLARE_EXTERNAL_OBJECT_CLASS(AREDGameState, RED)

private:
    char _padding[0xAE8 - 0x278];
};
struct FDecideInfo
{
    int CharaID;
    int ColorID;
    int CostumeID;
    int ScriptID;
    int StageID;
    int BGMID;
    int SpFlag;
    int SkillSetIndex;
};

struct FSideInfo
{
    SIDE_ID SideID;
    PAD_ID ControlPad;
    FDecideInfo DecideInfo;
    bool bSelectCPUChara;
    int32 Page;
};

struct FDecideInfoHistory
{
    FDecideInfo CharaHistory1P;
    FDecideInfo CharaHistory2P;
    bool IsValid;
    SIDE_ID MainSide;
};

struct FCharaSelectPlayerParam
{
    int32 Cursor;
    FVector CursorPos;
    int32 RandomCounter;
    bool bRandomSelect;
    int32 PrevCursor;
    int32 SelectColor;
    int32 SelectChara;
    bool bSelectCPU;

    static void InitializeStruct();
    
    static inline FDynamicScriptStruct Struct;
};

class AREDGameState_CharaSelectRE : public AREDGameState
{
    DECLARE_EXTERNAL_OBJECT_CLASS(AREDGameState_CharaSelectRE, REDExtend)
    
public:
    FCharaSelectPlayerParam PlayerParam[2];
    bool bInitialized;

    static void execInitialize(UObject* Context, FFrame* Stack, void* Z_Param__Result);
    void Initialize();
    
    static void AREDGameState_CharaSelectRE_Ctor(const FObjectInitializer* ObjectInitializer);
    static void InitializeClass();

    static FDynamicClass Class;
};