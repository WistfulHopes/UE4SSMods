#include <polyhook2/Detour/x64Detour.hpp>
#include "sigscan.h"
#include "arcsys.h"

#include <UE4SSProgram.hpp>
#include <UnrealDef.hpp>
#include <AGameModeBase.hpp>
#include <Mod/CppUserModBase.hpp>

#define WIN32_LEAN_AND_MEAN
#include "Windows.h"

#define STRIVEFRAMEDATA_API __declspec(dllexport)

class UREDGameCommon : public Unreal::UObject {};
UREDGameCommon* GameCommon;

typedef int(*GetGameMode_Func)(UREDGameCommon*);
GetGameMode_Func GetGameMode;

typedef bool(*IsPauseRED_Func)();
IsPauseRED_Func IsPauseRED;

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

enum class ESlateVisibility {
	Visible = 0,
	Collapsed = 1,
	Hidden = 2,
	HitTestInvisible = 3,
	SelfHitTestInvisible = 4,
	ESlateVisibility_MAX = 5,
};

bool ShouldUpdateBattle = true;
bool ShouldAdvanceBattle = false;
bool ShowFrameBar = false;

struct UpdateAdvantage
{
	UpdateAdvantage()
	{
		Text = Unreal::FString(STR(""));
	}

	Unreal::FString Text;
};

struct SetFrames
{
	TArray<int> P1Frames = {};
	TArray<int> P2Frames = {};
};

struct SetVisibility
{
	SetVisibility()
	{
		InVisibility = ESlateVisibility::Visible;
	}

	ESlateVisibility InVisibility;
};

std::vector<Unreal::UObject*> training_damage{};
std::vector<Unreal::UObject*> training_frames{};
Unreal::UFunction* updateadvantage_function;
Unreal::UFunction* setframes_function;
Unreal::UFunction* setvisibility_function;

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
TArray<int> p1_frames = {};
TArray<int> p2_frames = {};

UE4SSProgram* Program;

bool f2_pressed = false;
bool f3_pressed = false;
bool f4_pressed = false;

void SetFrameBarVisibility(bool Visible) {
	SetVisibility Params = SetVisibility();
	if (Visible) 
		Params.InVisibility = ESlateVisibility::Visible;
	else
		Params.InVisibility = ESlateVisibility::Collapsed;
	training_frames[training_frames.size() - 1]->ProcessEvent(setvisibility_function, &Params);
}

void(*UpdateBattle_Orig)(AREDGameState_Battle*, float);
void UpdateBattle_New(AREDGameState_Battle* GameState, float DeltaTime) {
	if (MatchStartFlag)
	{
		p1_frames.Empty();
		p2_frames.Empty();

		static auto battle_trainingdamage_name = Unreal::FName(STR("Battle_TrainingDamage_C"), Unreal::FNAME_Add);
		UObjectGlobals::FindAllOf(battle_trainingdamage_name, training_damage);
		updateadvantage_function = training_damage[0]->GetFunctionByNameInChain(STR("UpdateAdvantage"));

		static auto battle_trainingframes_name = Unreal::FName(STR("Battle_TrainingFrameData_C"), Unreal::FNAME_Add);
		UObjectGlobals::FindAllOf(battle_trainingframes_name, training_frames);
		setframes_function = training_frames[0]->GetFunctionByNameInChain(STR("SetFrames"));
		setvisibility_function = training_frames[0]->GetFunctionByNameInChain(FName(STR("SetVisibility")));

		MatchStartFlag = false;
	}
	if (!GameCommon)
	{
		GameCommon = reinterpret_cast<UREDGameCommon*>(UObjectGlobals::FindFirstOf(FName(STR("REDGameCommon"))));
		return;
	}
    if (GetGameMode(GameCommon) != static_cast<int>(GAME_MODE_TRAINING))
    {
		SetFrameBarVisibility(false);
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
	if (GetAsyncKeyState(VK_F4) & 0x8000)
	{
		if (!f4_pressed)
		{
			ShowFrameBar = !ShowFrameBar;
			SetFrameBarVisibility(ShowFrameBar);
			f4_pressed = true;
		}
	}
	else
	{
		f4_pressed = false;
	}


    if (ShouldUpdateBattle || ShouldAdvanceBattle)
	{
		UpdateBattle_Orig(GameState, DeltaTime);
		ShouldAdvanceBattle = false;

		if (IsPauseRED()) return;

	    const auto engine = asw_engine::get();
	    if (!engine) return;

		if (engine->players[0].entity->can_act() && !engine->players[1].entity->can_act()) p1_advantage++;
	    else if (engine->players[1].entity->can_act() && !engine->players[0].entity->can_act()) p2_advantage++;
		else if (!engine->players[1].entity->can_act() && !engine->players[0].entity->can_act())
		{
			p1_advantage = 0;
			p2_advantage = 0;
		}
		else
		{
			if (p1_advantage == 0) p1_advantage = -p2_advantage;
			else if (p2_advantage == 0) p2_advantage = -p1_advantage;
		}
    	
    	if (training_damage.empty()) return;
    	if (training_frames.empty()) return;

		if (!engine->players[0].entity->can_act() || !engine->players[1].entity->can_act())
		{
			if (p1_frames.Num() > 63 || p2_frames.Num() > 63)
			{
				p1_frames.Empty();
				p2_frames.Empty();
			}

			if (engine->super_freeze_self_timer > 0) return;

			if (engine->players[0].entity->hitstop > 1 && engine->players[0].entity->is_active()) return;
			if (engine->players[1].entity->hitstop > 1 && engine->players[1].entity->is_active()) return;

			int p1_frame;
			int p2_frame;

			p1_frame = -1;
			if (engine->players[0].entity->is_startup()) p1_frame = 0;
			if (engine->players[0].entity->is_active()) p1_frame = 1;
			if (engine->players[0].entity->is_recovery()) p1_frame = 2;
			if (engine->players[0].entity->is_stunned()) p1_frame = 3;

			p2_frame = -1;
			if (engine->players[1].entity->is_startup()) p2_frame = 0;
			if (engine->players[1].entity->is_active()) p2_frame = 1;
			if (engine->players[1].entity->is_recovery()) p2_frame = 2;
			if (engine->players[1].entity->is_stunned()) p2_frame = 3;

			p1_frames.Add(p1_frame);
			p2_frames.Add(p2_frame);

			SetFrames setframes_params = SetFrames();
			setframes_params.P1Frames = p1_frames;
			setframes_params.P2Frames = p2_frames;
			training_frames[training_frames.size() - 1]->ProcessEvent(setframes_function, &setframes_params);
		}
    	
	    UpdateAdvantage updateadvantage_paramsp1 = UpdateAdvantage();
	    auto p1_string = std::to_wstring(p1_advantage);
	    if (p1_advantage > 0)
	    {
	        p1_string.insert(0, STR("+"));
	    }
	    updateadvantage_paramsp1.Text = FString(p1_string.c_str());
	    training_damage[training_damage.size() - 1]->ProcessEvent(updateadvantage_function, &updateadvantage_paramsp1);

	    auto p2_string = std::to_wstring(p2_advantage);
	    if (p2_advantage > 0)
	    {
	        p2_string.insert(0, STR("+"));
	    }
	    UpdateAdvantage updateadvantage_paramsp2 = UpdateAdvantage();
	    updateadvantage_paramsp2.Text = FString(p2_string.c_str());
	    training_damage[training_damage.size() - 2]->ProcessEvent(updateadvantage_function, &updateadvantage_paramsp2);
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

	    const uint64_t UpdateBattle_Addr = sigscan::get().scan("\x40\x53\x57\x41\x54\x41\x55\x48\x81\xEC\x88\x00\x00\x00", "xxxxxxxxxxxxxx");
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

		const uintptr_t IsPauseRED_Addr = sigscan::get().scan("\x48\x8B\x88\x38\x02\x00\x00\xEB\x00\xF6\x05\x00\x00\x00\x00\x00", "xxxxxxxx?xx?????") - 0x10;
		IsPauseRED = reinterpret_cast<IsPauseRED_Func>(*reinterpret_cast<int*>(IsPauseRED_Addr + 0x1) + IsPauseRED_Addr + 0x5);

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