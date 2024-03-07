#pragma once

#include <Unreal/AActor.hpp>
#include "struct_util.h"

void ASWInitFunctions();

bool IsValidUObject(RC::Unreal::UObject* Object);

struct FVector
{
	float X, Y, Z;
};

class AGameState : public RC::Unreal::AActor {};

class UWorld : public RC::Unreal::UObject {
public:
    FIELD(0x130, AGameState*, GameState);
};

class AREDGameState_Battle : public AGameState {
public:
    FIELD(0xB10, class asw_engine*, Engine);
	FIELD(0xB18, class asw_scene*, Scene);
};

AREDGameState_Battle* GetGameState();

class player_block {
	char pad[0x280];
public:
	FIELD(0x10, class asw_entity*, entity);
};

static_assert(sizeof(player_block) == 0x280);

// Used by the shared GG/BB/DBFZ engine code
class asw_engine {
public:
	static constexpr auto COORD_SCALE = .43f; // 458f

	static asw_engine *get();

	ARRAY_FIELD(0x0, player_block[2], players);
	FIELD(0xAE0, int, entity_count);
	ARRAY_FIELD(0x1170, class asw_entity*[110], entities);
};


class asw_scene {
public:
	static asw_scene *get();

	// "delta" is the difference between input and output position
	// position gets written in place
	// position/angle can be null
	void camera_transform(FVector *delta, FVector *position, FVector *angle) const;
	void camera_transform(FVector *position, FVector *angle) const;
};

class hitbox {
public:
	enum class box_type : int {
		hurt = 0,
		hit = 1,
		grab = 2 // Not used by the game
	};

	box_type type;
	float x, y;
	float w, h;
};

enum class direction : int {
	right = 0,
	left = 1
};

class asw_entity {
public:
	//FIELD(0x28, int, is_active_value);
	FIELD(0x15, bool, is_player);
	FIELD(0x50, unsigned int, player_index);
	FIELD(0x90, hitbox*, hitboxes);
	FIELD(0x134, int, hurtbox_count);
	FIELD(0x138, int, hitbox_count);
	
	//   _____    ____    _    _   _   _   _______   ______   _____  
	//  / ____|  / __ \  | |  | | | \ | | |__   __| |  ____| |  __ \ 
	// | |      | |  | | | |  | | |  \| |    | |    | |__    | |__) |
	// | |      | |  | | | |  | | | . ` |    | |    |  __|   |  _  / 
	// | |____  | |__| | | |__| | | |\  |    | |    | |____  | | \ \ 
	//  \_____|  \____/   \____/  |_| \_|    |_|    |______| |_|  \_\ 
	BIT_FIELD(0x198, 0x4000000, cinematic_counter);
	FIELD(0x1B8, int, state_frames); //outdated*bad offset
	FIELD(0x2A8, asw_entity*, opponent); //outdated*bad offset
//	FIELD(0x2BC, asw_entity*, parent); //outdated*bad offset
	FIELD(0x318, asw_entity*, attached); //outdated*bad offset

	BIT_FIELD(0x364, 1, airborne); 
	BIT_FIELD(0x364, 0x8000000, counterhit); 
	
	BIT_FIELD(0x378, 16, strike_invuln);
	BIT_FIELD(0x378, 32, throw_invuln);
	BIT_FIELD(0x378, 64, wakeup);
	
	FIELD(0x384, direction, facing);

	FIELD(0x388, int, pos_x);
	FIELD(0x38C, int, pos_y);
	FIELD(0x390, int, pos_z);
	FIELD(0x3A0, int, angle_x);
	FIELD(0x3A4, int, angle_y);
	FIELD(0x3A8, int, angle_z);
	FIELD(0x3BC, int, scale_x);
	FIELD(0x3C0, int, scale_y);
	FIELD(0x3C4, int, scale_z);

	FIELD(0x59C, int, pushbox_front_offset);

	FIELD(0x868, int, throw_box_top); // 0x860
	FIELD(0x86C, int, throw_box_bottom); // 0x868
	FIELD(0x870, int, throw_range); // 0x86C


	FIELD( 0x3E48, unsigned int, is_active_player_bit );
	//FIELD(0x828, int, backdash_invuln); //850

	bool is_active() const;
	bool is_pushbox_active() const;
	bool is_strike_invuln() const;
	bool is_throw_invuln() const;
	int get_pos_x() const;
	int get_pos_y() const;
	int pushbox_width() const;
	int pushbox_height() const;
	int pushbox_bottom() const;
	void get_pushbox(int *left, int *top, int *right, int *bottom) const;
};
