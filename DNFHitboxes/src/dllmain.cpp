#include <polyhook2/Detour/x64Detour.hpp>
#include <polyhook2/Exceptions/AVehHook.hpp>
#include <Mod/CppUserModBase.hpp>
#include "sigscan.h"
#include "arcsys.h"
#include "math_util.h"
#include <array>
#include <iostream>
#include <Unreal/World.hpp>
#include <UnrealDef.hpp>
#include <DynamicOutput/Output.hpp>
#include <SigScanner/SinglePassSigScanner.hpp>

#include "BPMacros.hpp"
#include "Windows.h"

#define DNFHITBOXES_API __declspec(dllexport)

const void *vtable_hook(const void **vtable, const int index, const void *hook) {
	DWORD old_protect;
	VirtualProtect(&vtable[index], sizeof(void *), PAGE_READWRITE, &old_protect);
	const auto *orig = vtable[index];
	vtable[index] = hook;
	VirtualProtect(&vtable[index], sizeof(void *), old_protect, &old_protect);
	return orig;
}

struct drawn_hitbox {
	hitbox::box_type type;

	// Unclipped corners of filled box
	std::array<FVector2D, 4> corners;

	// Boxes to fill, clipped against other boxes
	std::vector<std::array<FVector2D, 4>> fill;

	// Outlines
	std::vector<std::array<FVector2D, 2>> lines;

	drawn_hitbox(const hitbox& box) :
		type(box.type),
		corners{
			FVector2D(box.x, box.y),
			FVector2D(box.x + box.w, box.y),
			FVector2D(box.x + box.w, box.y + box.h),
			FVector2D(box.x, box.y + box.h) }
	{
		for (auto i = 0; i < 4; i++)
			lines.push_back(std::array{ corners[i], corners[(i + 1) % 4] });

		fill.push_back(corners);
	}

	// Clip outlines against another hitbox
	void clip_lines(const drawn_hitbox& other)
	{
		auto old_lines = std::move(lines);
		lines.clear();

		for (auto& line : old_lines) {
			float entry_fraction, exit_fraction;
			auto intersected = line_box_intersection(
				other.corners[0], other.corners[2],
				line[0], line[1],
				&entry_fraction, &exit_fraction);

			if (!intersected) {
				lines.push_back(line);
				continue;
			}

			const auto delta = line[1] - line[0];

			if (entry_fraction != 0.f)
				lines.push_back(std::array{ line[0], line[0] + delta * entry_fraction });

			if (exit_fraction != 1.f)
				lines.push_back(std::array{ line[0] + delta * exit_fraction, line[1] });
		}
	}

	// Clip filled rectangle against another hitbox
	void clip_fill(const drawn_hitbox& other)
	{
		auto old_fill = std::move(fill);
		fill.clear();

		for (const auto& box : old_fill) {
			const auto& box_min = box[0];
			const auto& box_max = box[2];

			const auto clip_min = FVector2D(
				max_(box_min.X, other.corners[0].X),
				max_(box_min.Y, other.corners[0].Y));

			const auto clip_max = FVector2D(
				min_(box_max.X, other.corners[2].X),
				min_(box_max.Y, other.corners[2].Y));

			if (clip_min.X > clip_max.X || clip_min.Y > clip_max.Y) {
				// No intersection
				fill.push_back(box);
				continue;
			}

			if (clip_min.X > box_min.X) {
				// Left box
				fill.push_back(std::array{
					FVector2D(box_min.X, box_min.Y),
					FVector2D(clip_min.X, box_min.Y),
					FVector2D(clip_min.X, box_max.Y),
					FVector2D(box_min.X, box_max.Y) });
			}

			if (clip_max.X < box_max.X) {
				// Right box
				fill.push_back(std::array{
					FVector2D(clip_max.X, box_min.Y),
					FVector2D(box_max.X, box_min.Y),
					FVector2D(box_max.X, box_max.Y),
					FVector2D(clip_max.X, box_max.Y) });
			}

			if (clip_min.Y > box_min.Y) {
				// Top box
				fill.push_back(std::array{
					FVector2D(clip_min.X, box_min.Y),
					FVector2D(clip_max.X, box_min.Y),
					FVector2D(clip_max.X, clip_min.Y),
					FVector2D(clip_min.X, clip_min.Y) });
			}

			if (clip_max.Y < box_max.Y) {
				// Bottom box
				fill.push_back(std::array{
					FVector2D(clip_min.X, clip_max.Y),
					FVector2D(clip_max.X, clip_max.Y),
					FVector2D(clip_max.X, box_max.Y),
					FVector2D(clip_min.X, box_max.Y) });
			}
		}
	}
};

struct FLinearColor {
	float R, G, B, A;

	FLinearColor() : R(0), G(0), B(0), A(0) {}
	FLinearColor(float R, float G, float B, float A) : R(R), G(G), B(B), A(A) {}
};

struct FCanvasUVTri {
	FVector2D V0_Pos;
	FVector2D V0_UV;
	FLinearColor V0_Color;
	FVector2D V1_Pos;
	FVector2D V1_UV;
	FLinearColor V1_Color;
	FVector2D V2_Pos;
	FVector2D V2_UV;
	FLinearColor V2_Color;
};

class FCanvas {
public:
	enum ECanvasDrawMode {
		CDM_DeferDrawing,
		CDM_ImmediateDrawing
	};

	FIELD(0xA0, ECanvasDrawMode, DrawMode);

	void Flush_GameThread(bool bForce = false);
};

class UTexture;

class UCanvas : public UObject {
public:
	void K2_DrawLine(FVector2D ScreenPositionA, FVector2D ScreenPositionB, float Thickness, FLinearColor RenderColor);
	FVector K2_Project(const FVector WorldPosition);

	void K2_DrawTriangle(UTexture* RenderTexture, TArray<FCanvasUVTri>* Triangles);

	FIELD(0x260, FCanvas*, Canvas);
};

void UCanvas::K2_DrawLine(FVector2D ScreenPositionA, FVector2D ScreenPositionB, float Thickness, FLinearColor RenderColor)
{
	static auto drawline_name = Unreal::FName(STR("K2_DrawLine"), Unreal::FNAME_Add);
	static auto fn = GetFunctionByNameInChain(drawline_name);
	struct
	{
		FVector2D ScreenPositionA;
		FVector2D ScreenPositionB;
		float Thickness;
		FLinearColor RenderColor;
	}params;
	params.ScreenPositionA = ScreenPositionA;
	params.ScreenPositionB = ScreenPositionB;
	params.Thickness = Thickness;
	params.RenderColor = RenderColor;
	ProcessEvent(fn, &params);
}

FVector UCanvas::K2_Project(const FVector WorldPosition)
{
	static auto project_name = Unreal::FName(STR("K2_Project"), Unreal::FNAME_Add);
	static auto fn = GetFunctionByNameInChain(project_name);
	struct
	{
		FVector WorldPosition;
		FVector ReturnValue;
	}
	params;
	params.WorldPosition = WorldPosition;
	ProcessEvent(fn, &params);
	return params.ReturnValue;
}


void UCanvas::K2_DrawTriangle(UTexture* RenderTexture, TArray<FCanvasUVTri>* Triangles)
{
	static auto drawtriangle_name = Unreal::FName(STR("K2_DrawTriangle"), Unreal::FNAME_Add);
	static auto fn = GetFunctionByNameInChain(drawtriangle_name);
	struct
	{
		UTexture* RenderTexture;
		TArray<FCanvasUVTri>* Triangles;
	}params;
	params.RenderTexture = RenderTexture;
	params.Triangles = Triangles;
	ProcessEvent(fn, &params);
}

class AHUD : public UObject {
public:
	FIELD(0x278, UCanvas*, Canvas);
};

void asw_coords_to_screen(UCanvas* canvas, FVector2D* pos)
{
	pos->X *= asw_engine::COORD_SCALE / 1000.F;
	pos->Y *= asw_engine::COORD_SCALE / 1000.F;

	FVector pos3d(pos->X, 0.f, pos->Y);
	asw_scene::get()->camera_transform(&pos3d, nullptr);

	FVector proj = canvas->K2_Project(pos3d);

	*pos = FVector2D(proj.X, proj.Y);
}

// Corners must be in CW or CCW order
void draw_rect(
	UCanvas* canvas,
	const std::array<FVector2D, 4>& corners,
	const FLinearColor& color)
{
	for (auto i = 0; i < 4; i++)
		canvas->K2_DrawLine(corners[i], corners[(i + 1) % 4], 4.F, color);
}

void draw_pushbox(UCanvas* canvas, const asw_entity* entity)
{
	int left, top, right, bottom;
	entity->get_pushbox(&left, &top, &right, &bottom);
	std::array corners = {
		FVector2D(left, top),
		FVector2D(right, top),
		FVector2D(right, bottom),
		FVector2D(left, bottom)
	};

	for (auto& pos : corners)
		asw_coords_to_screen(canvas, &pos);
	// Show hollow pushbox when throw invuln

	FLinearColor color;
	if (entity->is_throw_invuln())
		color = FLinearColor(1.f, 1.f, 0.f, 0.f);
	else
		color = FLinearColor(1.f, 1.f, 0.f, .2f);
	draw_rect(canvas, corners, color);
}

// Transform entity local space to screen space
void transform_hitbox_point(UCanvas* canvas, const asw_entity* entity, FVector2D* pos, bool is_throw)
{
	if (!is_throw) {
		pos->X *= -entity->scale_x;
		pos->Y *= -entity->scale_y;

		const auto ca = cosf((float)M_PI / 180000.f);
		const auto sa = sinf((float)M_PI / 180000.f);
		*pos = FVector2D(pos->X * ca + pos->Y * -sa, pos->X * sa + pos->Y * ca);

		if (entity->facing == direction::left)
			pos->X *= -1.f;
	}
	else if (entity->opponent != nullptr) {
		// Throws hit on either side, so show it directed towards opponent
		if (entity->get_pos_x() > entity->opponent->get_pos_x())
			pos->X *= -1.f;
	}

	pos->X += entity->get_pos_x();
	pos->Y += entity->get_pos_y();

	asw_coords_to_screen(canvas, pos);
}

void draw_hitbox(UCanvas* canvas, const asw_entity* entity, const drawn_hitbox& box)
{
	FLinearColor color;
	if (box.type == hitbox::box_type::hit)
		color = FLinearColor(1.f, 0.f, 0.f, .25f);
	else if (box.type == hitbox::box_type::grab)
		color = FLinearColor(1.f, 0.f, 1.f, .25f);
	else if (entity->counterhit)
		color = FLinearColor(0.f, 1.f, 1.f, .25f);
	else
		color = FLinearColor(0.f, 1.f, 0.f, .25f);

	const auto is_throw = box.type == hitbox::box_type::grab;

	for (const auto& line : box.lines) {
		auto start = line[0];
		auto end = line[1];
		transform_hitbox_point(canvas, entity, &start, is_throw);
		transform_hitbox_point(canvas, entity, &end, is_throw);
		canvas->K2_DrawLine(start, end, 4.F, color);
	}
}

hitbox calc_throw_box(const asw_entity* entity)
{
	// Create a fake hitbox for throws to be displayed
	hitbox box;
	box.type = hitbox::box_type::grab;

	const auto pushbox_front = entity->pushbox_width() / 2 + entity->pushbox_front_offset;
	box.x = 0.f;
	box.w = (float)(pushbox_front + entity->throw_range);

	if (entity->throw_box_top <= entity->throw_box_bottom) {
		// No throw height, use pushbox height for display
		box.y = 0.f;
		box.h = (float)entity->pushbox_height();
		return box;
	}

	box.y = (float)entity->throw_box_bottom;
	box.h = (float)(entity->throw_box_top - entity->throw_box_bottom);
	return box;
}

void draw_hitboxes(UCanvas* canvas, const asw_entity* entity, bool active)
{
	const auto count = entity->hitbox_count + entity->hurtbox_count;

	std::vector<drawn_hitbox> hitboxes;

	// Collect hitbox info
	for (auto i = 0; i < count; i++) {
		const auto& box = entity->hitboxes[i];

		if (box.type == hitbox::box_type::hit && !active)
			continue;
		//	else if (box.type == hitbox::box_type::hurt && entity->is_strike_invuln())
		//		continue;

		hitboxes.push_back(drawn_hitbox(box));
	}

	// Add throw hitbox if in use
	if (entity->throw_range >= 0 && active)
		hitboxes.push_back(calc_throw_box(entity));

	for (auto i = 0; i < hitboxes.size(); i++) {
		// Clip outlines
		for (auto j = 0; j < hitboxes.size(); j++) {
			if (i != j && hitboxes[i].type == hitboxes[j].type)
				hitboxes[i].clip_lines(hitboxes[j]);
		}
		// Clip fill against every hitbox after, since two boxes
		// shouldn't both be clipped against each other
		for (auto j = i + 1; j < hitboxes.size(); j++) {
			if (hitboxes[i].type == hitboxes[j].type)
				hitboxes[i].clip_fill(hitboxes[j]);
		}
		draw_hitbox(canvas, entity, hitboxes[i]);
	}
}

using funcAHUDPostRender_t = void (*)(AHUD*);
funcAHUDPostRender_t orig_AHUDPostRender;
void hook_AHUDPostRender(AHUD*);

void hook_AHUDPostRender(AHUD* hud) {
	orig_AHUDPostRender(hud);

	const auto* engine = asw_engine::get();
	if (engine == nullptr)
		return;
	
	if (hud->Canvas == nullptr)
		return;
	
	if (hud->Canvas->Canvas == nullptr)
		return;

	const auto canvas = hud->Canvas;
	
	static bool show_only_me = false;
	if (GetAsyncKeyState(VK_BACK) & 0x01) show_only_me = !show_only_me;
		
	// Loop through entities backwards because the player that most
	// recently landed a hit is at index 0
	for (auto entidx = engine->entity_count - 1; entidx >= 0; entidx--) {
		const auto* entity = engine->entities[entidx];

		if (show_only_me && entity->player_index) continue;

		if (entity->is_pushbox_active())
			draw_pushbox(canvas, entity);
			
		const auto active = entity->is_active();
		draw_hitboxes(canvas, entity, active);

		const auto* attached = entity->attached;
		while (attached != nullptr) {
			draw_hitboxes(canvas, attached, active);
			attached = attached->attached;
		}
	}
}

/* Mod Definition */
class DNFHitboxes : public CppUserModBase 
{
public:
	DNFHitboxes() {
		ModName = STR("DNF Hitboxes");
		ModVersion = STR("1.0");
		ModDescription = STR("A tool to display hitboxes.");
		ModAuthors = STR("WistfulHopes");
		// Do not change this unless you want to target a UE4SS version
		// other than the one you're currently building with somehow.
		// ModIntendedSDKVersion = STR("2.6");
	}

	~DNFHitboxes() override {}

	auto on_update() -> void override {
	}

	auto on_unreal_init() -> void override {
		ASWInitFunctions();
		auto hud = UObjectGlobals::StaticFindObject(nullptr, nullptr, STR("/Script/RED.Default__REDHUD_Battle"));
		if (!hud)
		{
			Output::send(STR("Could not find CDO for REDHUD_Battle!"));
			return;
		}

		orig_AHUDPostRender = (funcAHUDPostRender_t)vtable_hook(((const void***)hud)[0], 214, hook_AHUDPostRender);

		SignatureContainer pitch_container{
        	{{"C7 83 74 3B 00 00 00 00 20 40"}},
				[&](const SignatureContainer & self) {
					DWORD Old1;
					PBYTE Patch1Bytes = (PBYTE)"\xC7\x83\x74\x3B\x00\x00\x00\x00\x00\x00";
        
					bool Success1 = VirtualProtect(self.get_match_address(), 0xA, PAGE_READWRITE, &Old1);

					if (Success1)
					{
						memcpy(self.get_match_address(), Patch1Bytes, 0xA);

						VirtualProtect(self.get_match_address(), 0xA, Old1, &Old1);
					}
					return true;
        		},
				[](SignatureContainer & self) {},
		};
		
		std::vector<SignatureContainer> signature_containers;
		signature_containers.push_back(pitch_container);
      
		SinglePassScanner::SignatureContainerMap signature_containers_map;

		signature_containers_map.emplace(ScanTarget::MainExe, signature_containers);

		SinglePassScanner::start_scan(signature_containers_map);
	}
};

extern "C" {
	DNFHITBOXES_API CppUserModBase* start_mod() {
	  return new DNFHitboxes();
	}

	DNFHITBOXES_API void uninstall_mod(CppUserModBase* mod) {
	  delete mod;
	}
}