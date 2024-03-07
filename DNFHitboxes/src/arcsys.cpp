#include "arcsys.h"

#include <UnrealDef.hpp>
#include <Unreal/Hooks.hpp>
#include "sigscan.h"

RC::UClass* Class = nullptr;
AREDGameState_Battle* GameState = nullptr;

using asw_scene_camera_transform_t = void(*)(const asw_scene*, FVector*, FVector*, FVector*);
asw_scene_camera_transform_t asw_scene_camera_transform;

using asw_entity_is_active_t = bool(*)(const asw_entity*, int);
asw_entity_is_active_t asw_entity_is_active;

using asw_entity_is_pushbox_active_t = bool(*)(const asw_entity*);
asw_entity_is_pushbox_active_t asw_entity_is_pushbox_active;

using asw_entity_get_pos_x_t = int(*)(const asw_entity*);
asw_entity_get_pos_x_t asw_entity_get_pos_x;

using asw_entity_get_pos_y_t = int(*)(const asw_entity*);
asw_entity_get_pos_y_t asw_entity_get_pos_y;

using asw_entity_pushbox_width_t = int(*)(const asw_entity*);
asw_entity_pushbox_width_t asw_entity_pushbox_width;

using asw_entity_pushbox_height_t = int(*)(const asw_entity*);
asw_entity_pushbox_height_t asw_entity_pushbox_height;

using asw_entity_pushbox_bottom_t = int(*)(const asw_entity*);
asw_entity_pushbox_bottom_t asw_entity_pushbox_bottom;

using asw_entity_get_pushbox_t = void(*)(const asw_entity*, int*, int*, int*, int*, int*, int*);
asw_entity_get_pushbox_t asw_entity_get_pushbox;

void InitGameState(RC::Unreal::AGameModeBase* GameMode)
{
	GameState = nullptr;
}

void ASWInitFunctions()
{
	RC::Unreal::Hook::RegisterInitGameStatePreCallback(InitGameState);
	
    asw_scene_camera_transform = (asw_scene_camera_transform_t)(
		sigscan::get().scan("\x4D\x85\xC0\x74\x15\xF2\x41\x0F", "xxxxxxxx") - 0x56);
    asw_entity_is_active = (asw_entity_is_active_t)(
		sigscan::get().scan("\xC1\xE8\x09\xA8\x01\x0F\x85\x00\x00\x00\x00\x84\xD2", "xxxxxxx????xx") - 0x11);
    asw_entity_is_pushbox_active = (asw_entity_is_pushbox_active_t)(
		sigscan::get().scan("\xC1\xEA\x11\xF6\xC2\x01\x75\x00", "xxxxxxx?") - 0x37);
    asw_entity_get_pos_x = (asw_entity_get_pos_x_t)(
		sigscan::get().scan("\x03\x83\x88\x03\x00\x00\x48\x83\xC4\x20", "xxxxxxxxxx") - 0x3F);
    asw_entity_get_pos_y = (asw_entity_get_pos_y_t)(
		sigscan::get().scan("\x03\x83\x8C\x03\x00\x00\x48\x83\xC4\x20\x5B\xC3\x8B\x83\x8C\x03\x00\x00", "xxxxxxxxxxxxxxxxxx") - 0x50);
    asw_entity_pushbox_width = (asw_entity_pushbox_width_t)sigscan::get().scan("\x40\x53\x48\x83\xEC\x20\x8B\x81\x88\x05\x00\x00", "xxxxxxxxxxxx");
    asw_entity_pushbox_height = (asw_entity_pushbox_height_t)sigscan::get().scan("\x40\x53\x48\x83\xEC\x20\x8B\x81\x8C\x05\x00\x00", "xxxxxxxxxxxx");
    asw_entity_pushbox_bottom = (asw_entity_pushbox_bottom_t)sigscan::get().scan("\x40\x53\x48\x83\xEC\x20\x8B\x81\x90\x05\x00\x00", "xxxxxxxxxxxx");
    asw_entity_get_pushbox = (asw_entity_get_pushbox_t)(
		sigscan::get().scan("\x48\x8B\x45\x58\x48\x85\xC0\x74\x00\x44\x89\x20", "xxxxxxxx?xxx") - 0x33);
}

AREDGameState_Battle* GetGameState()
{
	if (Class == nullptr)
		Class = RC::UObjectGlobals::StaticFindObject<RC::UClass*>(nullptr, nullptr, STR("/Script/RED.REDGameState_Battle"));
	
	if (!IsValidUObject(GameState))
	{
		static auto game_state_name = RC::Unreal::FName(STR("REDGameState_Battle"), RC::Unreal::FNAME_Add);
		GameState = (AREDGameState_Battle*)RC::Unreal::UObjectGlobals::FindFirstOf(game_state_name);
	}

	if (GameState == nullptr) return nullptr;
	if (!GameState->IsA(Class)) return nullptr;

	return GameState;
}

bool IsValidUObject(RC::Unreal::UObject* Object) {
	//return Object && !Object->IsUnreachable(); 
	return Object && RC::Unreal::UObjectArray::IsValid(Object->GetObjectItem(), false);
}

asw_engine *asw_engine::get()
{
    if (Class == nullptr)
    	Class = RC::UObjectGlobals::StaticFindObject<RC::UClass*>(nullptr, nullptr, STR("/Script/RED.REDGameState_Battle"));
	
	if (!IsValidUObject(GameState))
	{
		static auto game_state_name = RC::Unreal::FName(STR("REDGameState_Battle"), RC::Unreal::FNAME_Add);
		GameState = (AREDGameState_Battle*)RC::Unreal::UObjectGlobals::FindFirstOf(game_state_name);
	}

    if (GameState == nullptr) return nullptr;
    if (!GameState->IsA(Class)) return nullptr;

    return GameState->Engine;
}

asw_scene *asw_scene::get()
{
    if (Class == nullptr)
        Class = RC::UObjectGlobals::StaticFindObject<RC::UClass*>(nullptr, nullptr, STR("/Script/RED.REDGameState_Battle"));

	if (!IsValidUObject(GameState))
	{
		static auto game_state_name = RC::Unreal::FName(STR("REDGameState_Battle"), RC::Unreal::FNAME_Add);
		GameState = (AREDGameState_Battle*)RC::Unreal::UObjectGlobals::FindFirstOf(game_state_name);
	}

    if (GameState == nullptr) return nullptr;
    if (!GameState->IsA(Class)) return nullptr;

    return GameState->Scene;
}

void asw_scene::camera_transform(FVector *delta, FVector *position, FVector *angle) const
{
	asw_scene_camera_transform(this, delta, position, angle);
}

void asw_scene::camera_transform(FVector *position, FVector *angle) const
{
	FVector delta;
	asw_scene_camera_transform(this, &delta, position, angle);
}

bool asw_entity::is_active() const
{
	// Otherwise returns false during COUNTER
	// return cinematic_counter || asw_entity_is_active(this, !is_player);
	return asw_entity_is_active(this, ( is_player != 0 && (is_active_player_bit & 0x8000000) != 0 ));
}

bool asw_entity::is_pushbox_active() const
{
	return asw_entity_is_pushbox_active(this);
}

bool asw_entity::is_strike_invuln() const
{
	return strike_invuln || wakeup;
}

bool asw_entity::is_throw_invuln() const
{
	return throw_invuln || wakeup;// || backdash_invuln > 0;
}

int asw_entity::get_pos_x() const
{
	return asw_entity_get_pos_x(this);
}

int asw_entity::get_pos_y() const
{
	return asw_entity_get_pos_y(this);
}

int asw_entity::pushbox_width() const
{
	return asw_entity_pushbox_width(this);
}

int asw_entity::pushbox_height() const
{
	return asw_entity_pushbox_height(this);
}

int asw_entity::pushbox_bottom() const
{
	return asw_entity_pushbox_bottom(this);
}

void asw_entity::get_pushbox(int *left, int *top, int *right, int *bottom) const
{
	// last 2 param are X,Y position
	asw_entity_get_pushbox(this, left, top, right, bottom, nullptr, nullptr );
}
