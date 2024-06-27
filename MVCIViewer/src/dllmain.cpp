#include <Mod/CppUserModBase.hpp>
#include "math_util.hpp"
#include <array>
#include <Unreal/World.hpp>
#include <UnrealDef.hpp>
#include <DynamicOutput/Output.hpp>
#include <SigScanner/SinglePassSigScanner.hpp>

#include "ActionParameter.hpp"
#include "BattleMain.hpp"
#include "sPlayer.hpp"
#include "struct_util.hpp"
#include "sWork.hpp"
#include "Windows.h"

using namespace RC::Unreal;

#define MVCIVIEWER_API __declspec(dllexport)

struct FCanvasUVTri;

struct FVector_Custom
{
	float X, Y, Z;
};

const void *vtable_hook(const void **vtable, const int index, const void *hook) {
	DWORD old_protect;
	VirtualProtect(&vtable[index], sizeof(void *), PAGE_READWRITE, &old_protect);
	const auto *orig = vtable[index];
	vtable[index] = hook;
	VirtualProtect(&vtable[index], sizeof(void *), old_protect, &old_protect);
	return orig;
}

class FTexture** GWhiteTexture;

enum ESimpleElementBlendMode
{
	SE_BLEND_Opaque = 0,
	SE_BLEND_Masked,
	SE_BLEND_Translucent,
	SE_BLEND_Additive,
	SE_BLEND_Modulate,
	SE_BLEND_MaskedDistanceField,
	SE_BLEND_MaskedDistanceFieldShadowed,
	SE_BLEND_TranslucentDistanceField,
	SE_BLEND_TranslucentDistanceFieldShadowed,
	SE_BLEND_AlphaComposite,
	SE_BLEND_AlphaHoldout,
	// Like SE_BLEND_Translucent, but modifies destination alpha
	SE_BLEND_AlphaBlend,
	// Like SE_BLEND_Translucent, but reads from an alpha-only texture
	SE_BLEND_TranslucentAlphaOnly,
	SE_BLEND_TranslucentAlphaOnlyWriteAlpha,

	SE_BLEND_RGBA_MASK_START,
	SE_BLEND_RGBA_MASK_END = SE_BLEND_RGBA_MASK_START + 31, //Using 5bit bit-field for red, green, blue, alpha and desaturation

	SE_BLEND_MAX
};

class FCanvasItem {
protected:
	FCanvasItem() = default;

public:

	// Virtual function wrapper
	void Draw(class FCanvas* InCanvas)
	{
		using Draw_t = void(*)(FCanvasItem*, class FCanvas*);
		((Draw_t)(*(void***)this)[3])(this, InCanvas);
	}

	FIELD(0x14, ESimpleElementBlendMode, BlendMode);
};

class FCanvasTriangleItem : public FCanvasItem {
public:
	FCanvasTriangleItem(
		const FVector2D_Custom& InPointA,
		const FVector2D_Custom& InPointB,
		const FVector2D_Custom& InPointC,
		const FTexture* InTexture);

	~FCanvasTriangleItem()
	{
		TriangleList.~TArray<FCanvasUVTri>();
	}

	FIELD(0x50, Unreal::TArray<FCanvasUVTri>, TriangleList);

private:
	char pad[0x60];
};

static_assert(sizeof(FCanvasTriangleItem) == 0x60);

using FCanvasTriangleItem_ctor_t = void(*)(
	FCanvasTriangleItem*,
	const FVector2D_Custom&,
	const FVector2D_Custom&,
	const FVector2D_Custom&,
	const FTexture*);
FCanvasTriangleItem_ctor_t FCanvasTriangleItem_ctor;

FCanvasTriangleItem::FCanvasTriangleItem(
	const FVector2D_Custom& InPointA,
	const FVector2D_Custom& InPointB,
	const FVector2D_Custom& InPointC,
	const FTexture* InTexture)
{
	FCanvasTriangleItem_ctor(this, InPointA, InPointB, InPointC, InTexture);
}

enum class EBoxType : uint8
{
	Attack,
	Damage,
	Catch,
	Caught,
	Atemi,
	Push,
	Unique,
};

struct drawn_hitbox {
	EBoxType type;

	// Unclipped corners of filled box
	std::array<FVector2D_Custom, 4> corners;

	// Boxes to fill, clipped against other boxes
	std::vector<std::array<FVector2D_Custom, 4>> fill;

	// Outlines
	std::vector<std::array<FVector2D_Custom, 2>> lines;

	drawn_hitbox(const FHitRect16& box, EBoxType type) :
		type(type),
		corners{
			FVector2D_Custom(box.OffsetX - box.SizeX, box.OffsetY - box.SizeY),
			FVector2D_Custom(box.OffsetX + box.SizeX, box.OffsetY - box.SizeY),
			FVector2D_Custom(box.OffsetX + box.SizeX, box.OffsetY + box.SizeY),
			FVector2D_Custom(box.OffsetX - box.SizeX, box.OffsetY + box.SizeY) }
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

			const auto clip_min = FVector2D_Custom(
				max_(box_min.X, other.corners[0].X),
				max_(box_min.Y, other.corners[0].Y));

			const auto clip_max = FVector2D_Custom(
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
					FVector2D_Custom(box_min.X, box_min.Y),
					FVector2D_Custom(clip_min.X, box_min.Y),
					FVector2D_Custom(clip_min.X, box_max.Y),
					FVector2D_Custom(box_min.X, box_max.Y) });
			}

			if (clip_max.X < box_max.X) {
				// Right box
				fill.push_back(std::array{
					FVector2D_Custom(clip_max.X, box_min.Y),
					FVector2D_Custom(box_max.X, box_min.Y),
					FVector2D_Custom(box_max.X, box_max.Y),
					FVector2D_Custom(clip_max.X, box_max.Y) });
			}

			if (clip_min.Y > box_min.Y) {
				// Top box
				fill.push_back(std::array{
					FVector2D_Custom(clip_min.X, box_min.Y),
					FVector2D_Custom(clip_max.X, box_min.Y),
					FVector2D_Custom(clip_max.X, clip_min.Y),
					FVector2D_Custom(clip_min.X, clip_min.Y) });
			}

			if (clip_max.Y < box_max.Y) {
				// Bottom box
				fill.push_back(std::array{
					FVector2D_Custom(clip_min.X, clip_max.Y),
					FVector2D_Custom(clip_max.X, clip_max.Y),
					FVector2D_Custom(clip_max.X, box_max.Y),
					FVector2D_Custom(clip_min.X, box_max.Y) });
			}
		}
	}
};

struct FLinearColor_Custom {
	float R, G, B, A;

	FLinearColor_Custom() : R(0), G(0), B(0), A(0) {}
	FLinearColor_Custom(float R, float G, float B, float A) : R(R), G(G), B(B), A(A) {}
};

struct FCanvasUVTri {
	FVector2D_Custom V0_Pos;
	FVector2D_Custom V0_UV;
	FLinearColor_Custom V0_Color;
	FVector2D_Custom V1_Pos;
	FVector2D_Custom V1_UV;
	FLinearColor_Custom V1_Color;
	FVector2D_Custom V2_Pos;
	FVector2D_Custom V2_UV;
	FLinearColor_Custom V2_Color;
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
	void K2_DrawLine(FVector2D_Custom ScreenPositionA, FVector2D_Custom ScreenPositionB, float Thickness, FLinearColor_Custom RenderColor);
	FVector_Custom K2_Project(const FVector_Custom WorldPosition);

	void K2_DrawTriangle(UTexture* RenderTexture, Unreal::TArray<FCanvasUVTri>* Triangles);

	FIELD(0x270, FCanvas*, Canvas);
};

void UCanvas::K2_DrawLine(FVector2D_Custom ScreenPositionA, FVector2D_Custom ScreenPositionB, float Thickness, FLinearColor_Custom RenderColor)
{
	static auto drawline_name = Unreal::FName(STR("K2_DrawLine"), FNAME_Add);
	static auto fn = GetFunctionByNameInChain(drawline_name);
	struct
	{
		FVector2D_Custom ScreenPositionA;
		FVector2D_Custom ScreenPositionB;
		float Thickness;
		FLinearColor_Custom RenderColor;
	}params;
	params.ScreenPositionA = ScreenPositionA;
	params.ScreenPositionB = ScreenPositionB;
	params.Thickness = Thickness;
	params.RenderColor = RenderColor;
	ProcessEvent(fn, &params);
}

FVector_Custom UCanvas::K2_Project(const FVector_Custom WorldPosition)
{
	static auto project_name = Unreal::FName(STR("K2_Project"), FNAME_Add);
	static auto fn = GetFunctionByNameInChain(project_name);
	struct
	{
		FVector_Custom WorldPosition;
		FVector_Custom ReturnValue;
	}
	params;
	params.WorldPosition = WorldPosition;
	ProcessEvent(fn, &params);
	return params.ReturnValue;
}


void UCanvas::K2_DrawTriangle(UTexture* RenderTexture, Unreal::TArray<FCanvasUVTri>* Triangles)
{
	static auto drawtriangle_name = Unreal::FName(STR("K2_DrawTriangle"), FNAME_Add);
	static auto fn = GetFunctionByNameInChain(drawtriangle_name);
	struct
	{
		UTexture* RenderTexture;
		Unreal::TArray<FCanvasUVTri>* Triangles;
	}params;
	params.RenderTexture = RenderTexture;
	params.Triangles = Triangles;
	ProcessEvent(fn, &params);
}

class AHUD_Custom : public UObject {
public:
	FIELD(0x3D0, UCanvas*, Canvas);
};

void coords_to_screen(UCanvas* canvas, FVector2D_Custom* pos)
{
	FVector_Custom pos3d(pos->X, 0.f, pos->Y);
	
	FVector_Custom proj = canvas->K2_Project(pos3d);
	
	*pos = FVector2D_Custom(proj.X, proj.Y);
}

// Corners must be in CW or CCW order
void fill_rect(
	UCanvas* canvas,
	const std::array<FVector2D_Custom, 4>& corners,
	const FLinearColor_Custom& color)
{
	FCanvasUVTri triangles[2];
	triangles[0].V0_Color = triangles[0].V1_Color = triangles[0].V2_Color = color;
	triangles[1].V0_Color = triangles[1].V1_Color = triangles[1].V2_Color = color;

	triangles[0].V0_Pos = corners[0];
	triangles[0].V1_Pos = corners[1];
	triangles[0].V2_Pos = corners[2];

	triangles[1].V0_Pos = corners[2];
	triangles[1].V1_Pos = corners[3];
	triangles[1].V2_Pos = corners[0];
	
	FCanvasTriangleItem item(
		FVector2D_Custom(0.f, 0.f),
		FVector2D_Custom(0.f, 0.f),
		FVector2D_Custom(0.f, 0.f),
		*GWhiteTexture);

	Unreal::TArray<FCanvasUVTri> List;
	for (auto Triangle : triangles)
		List.Add(Triangle);

	item.TriangleList = List;
	item.BlendMode = SE_BLEND_Translucent;
	item.Draw(canvas->Canvas);
}

// Corners must be in CW or CCW order
void draw_rect(
	UCanvas* canvas,
	const std::array<FVector2D_Custom, 4>& corners,
	const FLinearColor_Custom& color)
{
	//fill_rect(canvas, corners, color);
	for (auto i = 0; i < 4; i++)
		canvas->K2_DrawLine(corners[i], corners[(i + 1) % 4], 4.F, color);
}

void draw_pushbox(UCanvas* canvas, nBattle::cWork* work)
{
	float left;
	if (work->Dir)
		left = (float)-work->Range.L * 1 + (float)work->pos.x / 65536.f;
	else
		left = (float)-work->Range.L * -1 + (float)work->pos.x / 65536.f;
	float top = (float)work->Range.U + (float)work->pos.y / 65536.f;
	float right;
	if (work->Dir)
		right = (float)work->Range.R * 1 + (float)work->pos.x / 65536.f;
	else
		right = (float)work->Range.R * -1 + (float)work->pos.x / 65536.f;
	float bottom = (float)work->Range.D + (float)work->pos.y / 65536.f;
	std::array corners = {
		FVector2D_Custom(left, top),
		FVector2D_Custom(right, top),
		FVector2D_Custom(right, bottom),
		FVector2D_Custom(left, bottom)
	};

	for (auto& pos : corners)
		coords_to_screen(canvas, &pos);
	// Show hollow pushbox when throw invuln

	FLinearColor_Custom color;
	color = FLinearColor_Custom(1.f, 1.f, 0.f, .2f);
	draw_rect(canvas, corners, color);
}

// Transform entity local space to screen space
void transform_hitbox_point(UCanvas* canvas, const nBattle::cWork* work, FVector2D_Custom* pos)
{
	if (!work->Dir)
		pos->X *= -1.f;

	pos->X += (float)work->pos.x / 65536.0;
	pos->Y += (float)work->pos.y / 65536.0;

	coords_to_screen(canvas, pos);
}

void draw_hitbox(UCanvas* canvas, nBattle::cWork* work, const drawn_hitbox& box)
{
	FLinearColor_Custom color;
	if (box.type == EBoxType::Attack)
		color = FLinearColor_Custom(1.f, 0.f, 0.f, .25f);
	else if (box.type == EBoxType::Catch)
		color = FLinearColor_Custom(1.f, 0.f, 1.f, .25f);
	else if (box.type == EBoxType::Atemi)
		color = FLinearColor_Custom(0.f, 0.f, 1.f, .25f);
	else if (box.type == EBoxType::Push)
		color = FLinearColor_Custom(1.f, 1.f, 0.f, .25f);
	else if (box.type == EBoxType::Caught)
		color = FLinearColor_Custom(0.f, 1.f, 1.f, .25f);
	else if (box.type == EBoxType::Damage)
		color = FLinearColor_Custom(0.f, 1.f, 0.f, .25f);
	else
		color = FLinearColor_Custom(1.f, 1.f, 1.f, .25f);
	
	/*for (auto fill : box.fill) {
		for (auto& pos : fill)
			transform_hitbox_point(canvas, work, &pos);

		fill_rect(canvas, fill, color);
	}*/

	for (const auto& line : box.lines) {
		auto start = line[0];
		auto end = line[1];

		transform_hitbox_point(canvas, work, &start);
		transform_hitbox_point(canvas, work, &end);
		
		canvas->K2_DrawLine(start, end, 4.F, color);
	}
}

void draw_hitboxes(UCanvas* canvas, UCharacterAsset* character_asset, nBattle::cWork* work)
{
	if (!work->mpActParam) return;
	
	std::vector<drawn_hitbox> hitboxes;
	
	// Collect hitbox info
	const auto attack_index = work->mpActParam->AttackCollisionListIndex;
	const auto hit_index = work->mpActParam->HitCollisionListIndex;
	const auto body_index = work->mpActParam->BodyCollisionListIndex;
	
	if (attack_index >= 0 && attack_index < character_asset->AttackCollisionIndexList.Num())
	{
		auto& attack_index_list = character_asset->AttackCollisionIndexList[attack_index];

		for (const auto i : attack_index_list.AttackList)
		{
			hitboxes.emplace_back(character_asset->AttackCollisionParamList[i].RectData, EBoxType::Attack);		
		}

		for (const auto i : attack_index_list.CatchList)
		{
			hitboxes.emplace_back(character_asset->CatchCollisionParamList[i].RectData, EBoxType::Catch);		
		}

		for (const auto i : attack_index_list.AtemiList)
		{
			hitboxes.emplace_back(character_asset->AtemiCollisionParamList[i].RectData, EBoxType::Atemi);		
		}
	}

	if (hit_index >= 0 && hit_index < character_asset->DamageCollisionIndexList.Num())
	{	
		auto& hit_index_list = character_asset->DamageCollisionIndexList[hit_index];

		for (const auto i : hit_index_list.LegList)
		{
			hitboxes.emplace_back(character_asset->LegCollisionParamList[i].RectData, EBoxType::Damage);		
		}

		for (const auto i : hit_index_list.BodyList)
		{
			hitboxes.emplace_back(character_asset->BodyCollisionParamList[i].RectData, EBoxType::Damage);		
		}

		for (const auto i : hit_index_list.HeadList)
		{
			hitboxes.emplace_back(character_asset->HeadCollisionParamList[i].RectData, EBoxType::Damage);		
		}

		for (const auto i : hit_index_list.CaughtList)
		{
			hitboxes.emplace_back(character_asset->CaughtCollisionParamList[i].RectData, EBoxType::Caught);		
		}
	}

	if (body_index >= 0 && body_index < character_asset->PushCollisionIndexList.Num())
	{
		auto& body_index_list = character_asset->PushCollisionIndexList[body_index];
	
		for (const auto i : body_index_list.PushList)
		{
			hitboxes.emplace_back(character_asset->PushCollisionParamList[i].RectData, EBoxType::Push);		
		}
	}
	
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
		draw_hitbox(canvas, work, hitboxes[i]);
	}
}

using funcAHUDPostRender_t = void (*)(AHUD_Custom*);
funcAHUDPostRender_t orig_AHUDPostRender;
void hook_AHUDPostRender(AHUD_Custom*);

void hook_AHUDPostRender(AHUD_Custom* hud) {
	orig_AHUDPostRender(hud);

	const auto* battle_main = ABattleMain::Get();
	
	if (battle_main == nullptr)
		return;
	
	if (hud->Canvas == nullptr)
		return;
	
	if (hud->Canvas->Canvas == nullptr)
		return;

	const auto canvas = hud->Canvas;
	
	static bool show_only_me = false;
	if (GetAsyncKeyState(VK_BACK) & 0x01) show_only_me = !show_only_me;

	if (!battle_main->Work) return;
	if (!battle_main->Player) return;
	
	for (auto& work : battle_main->Work->Global_work)
	{
		if (work.status & 2) continue;
		
		const auto character_asset = battle_main->mpCharacterAsset[work.pl_work_no];
		// draw_pushbox(canvas, &work);
		draw_hitboxes(canvas, character_asset, &work);
	}

	for (auto& player : battle_main->Player->mcPlayer)
	{
		if (!player.be_flag) continue;

		const auto character_asset = battle_main->mpCharacterAsset[player.pl_work_no];
		// draw_pushbox(canvas, &player);
		draw_hitboxes(canvas, character_asset, &player);
	}
}


void InitGameState(AGameModeBase* GameMode)
{
	ABattleMain::Reset();
}

/* Mod Definition */
class MVCIViewer : public CppUserModBase 
{
public:
	MVCIViewer() {
		ModName = STR("MVCI Viewer");
		ModVersion = STR("1.0");
		ModDescription = STR("A tool to view collision in MVCI.");
		ModAuthors = STR("WistfulHopes");
		// Do not change this unless you want to target a UE4SS version
		// other than the one you're currently building with somehow.
		// ModIntendedSDKVersion = STR("2.6");
	}

	~MVCIViewer() override {}

	auto on_update() -> void override {
	}

	auto on_unreal_init() -> void override {
		Hook::RegisterInitGameStatePreCallback(InitGameState);

		auto hud = Unreal::UObjectGlobals::StaticFindObject(nullptr, nullptr, STR("/Script/Duck.Default__BattleHUD"));
		if (!hud)
		{
			Output::send(STR("Could not find CDO for BattleHUD!"));
			return;
		}

		orig_AHUDPostRender = (funcAHUDPostRender_t)vtable_hook(((const void***)hud)[0], 196, hook_AHUDPostRender);
	}
};

extern "C" {
	MVCIVIEWER_API CppUserModBase* start_mod() {
	  return new MVCIViewer();
	}

	MVCIVIEWER_API void uninstall_mod(CppUserModBase* mod) {
	  delete mod;
	}
}