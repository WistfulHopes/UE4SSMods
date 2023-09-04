#include <polyhook2/Detour/x64Detour.hpp>
#include "sigscan.h"
#include "arcsys.h"

#include <UE4SSProgram.hpp>
#include <UnrealDef.hpp>
#include <Mod/CppUserModBase.hpp>

#define WIN32_LEAN_AND_MEAN
#include "Windows.h"

#define STRIVEFRAMEDATA_API __declspec(dllexport)

class UREDGameCommon : public Unreal::UObject {};
UREDGameCommon* GameCommon;

typedef int(*GetGameMode_Func)(UREDGameCommon*);
GetGameMode_Func GetGameMode;

enum GAME_MODE : int32_t
{
    GAME_MODE_DEBUG_BATTLE = 0x0,
    GAME_MODE_ADVERTISE = 0x1,
    GAME_MODE_MAINTENANCEVS = 0x2,
    GAME_MODE_ARCADE = 0x3,
    GAME_MODE_MOM = 0x4,
    GAME_MODE_SPARRING = 0x5,
    GAME_MODE_VERSUS = 0x6,
    GAME_MODE_VERSUS_PREINSTALL = 0x7,
    GAME_MODE_TRAINING = 0x8,
    GAME_MODE_TOURNAMENT = 0x9,
    GAME_MODE_RANNYU_VERSUS = 0xA,
    GAME_MODE_EVENT = 0xB,
    GAME_MODE_SURVIVAL = 0xC,
    GAME_MODE_STORY = 0xD,
    GAME_MODE_MAINMENU = 0xE,
    GAME_MODE_TUTORIAL = 0xF,
    GAME_MODE_LOBBYTUTORIAL = 0x10,
    GAME_MODE_CHALLENGE = 0x11,
    GAME_MODE_KENTEI = 0x12,
    GAME_MODE_MISSION = 0x13,
    GAME_MODE_GALLERY = 0x14,
    GAME_MODE_LIBRARY = 0x15,
    GAME_MODE_NETWORK = 0x16,
    GAME_MODE_REPLAY = 0x17,
    GAME_MODE_LOBBYSUB = 0x18,
    GAME_MODE_MAINMENU_QUICK_BATTLE = 0x19,
    GAME_MODE_UNDECIDED = 0x1A,
    GAME_MODE_INVALID = 0x1B,
};

bool ShouldUpdateBattle = true;
bool ShouldAdvanceBattle = false;

struct UpdateAdvantage
{
    UpdateAdvantage()
    {
        Text = Unreal::FString(STR(""));
    }

    Unreal::FString Text;
};

std::vector<Unreal::UObject*> mod_actors{};
Unreal::UFunction* bp_function;

bool MatchStartFlag = false;

void(*MatchStart_Orig)(AREDGameState_Battle*);
void MatchStart_New(AREDGameState_Battle* GameState)
{
    MatchStartFlag = true;
    ShouldAdvanceBattle = false;
    ShouldUpdateBattle = true;
    MatchStart_Orig(GameState);
}

int p1_advantage = 0;
int p2_advantage = 0;
int p1_hitstop_prev = 0;
int p2_hitstop_prev = 0;
int p1_act = 0;
int p2_act = 0;

UE4SSProgram* Program;

bool f2_pressed = false;
bool f3_pressed = false;

void(*UpdateBattle_Orig)(AREDGameState_Battle*, float);
void UpdateBattle_New(AREDGameState_Battle* GameState, float DeltaTime) {
	if (!GameCommon)
	{
		GameCommon = reinterpret_cast<UREDGameCommon*>(UObjectGlobals::FindFirstOf(FName(STR("REDGameCommon"))));
		return;
	}
    if (GetGameMode(GameCommon) != static_cast<int>(GAME_MODE_TRAINING))
    {
        UpdateBattle_Orig(GameState, DeltaTime);
        return;
    }
	
	if (GetAsyncKeyState(VK_F2) & 0x8000)
	{
		if (!f2_pressed)
		{
			ShouldUpdateBattle = !ShouldUpdateBattle;
			f2_pressed = true;
		}
	}
	else
	{
		f2_pressed = false;
	}
	if (GetAsyncKeyState(VK_F3) & 0x8000)
	{
		if (!f3_pressed)
		{
			ShouldAdvanceBattle = true;
			f3_pressed = true;
		}
	}
	else
	{
		f3_pressed = false;
	}

    if (ShouldUpdateBattle || ShouldAdvanceBattle)
	{
		UpdateBattle_Orig(GameState, DeltaTime);
		ShouldAdvanceBattle = false;

	    const auto engine = asw_engine::get();
	    if (!engine) return;
	    if ((engine->players[0].entity->action_time == 1 && engine->players[0].entity->hitstop != p1_hitstop_prev - 1)
	        || (engine->players[1].entity->action_time == 1 && engine->players[1].entity->hitstop != p2_hitstop_prev - 1))
	    {
	        if (!engine->players[0].entity->can_act() || !engine->players[1].entity->can_act())
	        {
	            if (!engine->players[1].entity->is_knockdown() || engine->players[1].entity->is_down_bound() || engine->players[1].entity->is_quick_down_1())
	            {
	                p1_advantage = engine->players[0].entity->calc_advantage() + p1_act;
	            }
	            if (!engine->players[0].entity->is_knockdown() || engine->players[0].entity->is_down_bound() || engine->players[0].entity->is_quick_down_1())
	            {
	                p2_advantage = engine->players[1].entity->calc_advantage() + p2_act;
	            }
	        }
	        else
	        {
	            p1_advantage = p1_act - p2_act;
	            p2_advantage = p2_act - p1_act;
	        }
	    }

	    if (engine->players[0].entity->is_stunned() && !engine->players[1].entity->is_stunned())
	        p1_advantage = -p2_advantage;
	    else if (engine->players[1].entity->is_stunned() && !engine->players[0].entity->is_stunned())
	        p2_advantage = -p1_advantage;
	    
	    p1_hitstop_prev = engine->players[0].entity->hitstop;
	    p2_hitstop_prev = engine->players[1].entity->hitstop;
	    if (engine->players[0].entity->can_act() && !engine->players[1].entity->can_act())
	        p1_act++;
	    else p1_act = 0;
	    if (engine->players[1].entity->can_act() && !engine->players[0].entity->can_act())
	        p2_act++;
	    else p2_act = 0;

        if (MatchStartFlag)
        {
            static auto battle_trainingdamage_name = Unreal::FName(STR("Battle_TrainingDamage_C"), Unreal::FNAME_Add);

            UObjectGlobals::FindAllOf(battle_trainingdamage_name, mod_actors);

            if (mod_actors.empty()) return;
            
            bp_function = mod_actors[0]->GetFunctionByNameInChain(STR("UpdateAdvantage"));
            MatchStartFlag = false;
        }
	    
	    UpdateAdvantage params = UpdateAdvantage();
	    auto p1_string = std::to_wstring(p1_advantage);
	    if (p1_advantage > 0)
	    {
	        p1_string.insert(0, STR("+"));
	    }
	    if (p1_advantage > 1500 || p1_advantage < -1500)
	    {
	        p1_string = STR("???");
	    }
	    params.Text = FString(p1_string.c_str());
	    mod_actors[mod_actors.size() - 1]->ProcessEvent(bp_function, &params);

	    auto p2_string = std::to_wstring(p2_advantage);
	    if (p2_advantage > 0)
	    {
	        p2_string.insert(0, STR("+"));
	    }
	    if (p2_advantage > 1500 || p2_advantage < -1500)
	    {
	        p2_string = STR("???");
	    }
	    UpdateAdvantage params2 = UpdateAdvantage();
	    params2.Text = FString(p2_string.c_str());
	    mod_actors[mod_actors.size() - 2]->ProcessEvent(bp_function, &params2);
	}
}

class StriveFrameData : public CppUserModBase
{
public:
	PLH::x64Detour* UpdateBattle_Detour;
	PLH::x64Detour* MatchStart_Detour;

    StriveFrameData() : CppUserModBase()
    {
    	ModName = STR("Strive Frame Advantage");
    	ModVersion = STR("1.0");
    	ModDescription = STR("A tool to display frame advantage.");
    	ModAuthors = STR("WistfulHopes");
    	UpdateBattle_Detour = nullptr;
    	MatchStart_Detour = nullptr;
    	// Do not change this unless you want to target a UE4SS version
    	// other than the one you're currently building with somehow.
    	//ModIntendedSDKVersion = STR("2.6");
    }

    ~StriveFrameData() override
    {
    }

    auto on_update() -> void override
    {
    }

    auto on_unreal_init() -> void override
    {
    	GameCommon = reinterpret_cast<UREDGameCommon*>(UObjectGlobals::FindFirstOf(FName(STR("REDGameCommon"))));

    	Program = &UE4SSProgram::get_program();

	    const uint64_t UpdateBattle_Addr = sigscan::get().scan("\x48\x8B\x97\x88\x0B\x00\x00", "xxxxxxx") - 0x2B;
    	UpdateBattle_Detour = new PLH::x64Detour(
			UpdateBattle_Addr, reinterpret_cast<uint64_t>(&UpdateBattle_New),
			reinterpret_cast<uint64_t*>(&UpdateBattle_Orig));
    	UpdateBattle_Detour->hook();

	    const uint64_t MatchStart_Addr = sigscan::get().scan("\x44\x39\x68\x08\x74\x00\x48\x8B\x18\xEB\x00\x48\x8B\xDF", "xxxxx?xxxx?xxx") - 0x8D;
    	MatchStart_Detour = new PLH::x64Detour(
			MatchStart_Addr, reinterpret_cast<uint64_t>(&MatchStart_New),
			reinterpret_cast<uint64_t*>(&MatchStart_Orig));
    	MatchStart_Detour->hook();

	    const uintptr_t GetGameMode_Addr = sigscan::get().scan("\x0F\xB6\x81\xF0\x02\x00\x00\xC3", "xxxxxxxx");
    	GetGameMode = reinterpret_cast<GetGameMode_Func>(GetGameMode_Addr);
    	
    	ASWInitFunctions();
    	bbscript::BBSInitializeFunctions();
    }
};

extern "C"
{
    STRIVEFRAMEDATA_API CppUserModBase* start_mod()
    {
        return new StriveFrameData();
    }

    STRIVEFRAMEDATA_API void uninstall_mod(CppUserModBase* mod)
    {
        delete mod;
    }
}