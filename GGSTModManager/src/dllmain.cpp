#include <Mod/CppUserModBase.hpp>
#include <UE4SSProgram.hpp>
#include "ModManager.hpp"
#include <d3d11.h>
#include <dxgi.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include "kiero.h"
#include "Roboto.hpp"

typedef HRESULT(__stdcall* Present) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
typedef HRESULT(__stdcall* ResizeBuffers) (
    IDXGISwapChain* pSwapChain,
    UINT BufferCount,
    UINT Width,
    UINT Height,
    DXGI_FORMAT NewFormat,
    UINT SwapChainFlags);
typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);
typedef uintptr_t PTR;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Present PresentOrig;
ResizeBuffers ResizeBuffersOrig;
HWND window = NULL;
WNDPROC oWndProc;
ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* mainRenderTargetView;

void InitImGui()
{
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;

    ImGuiStyle& style = ImGui::GetStyle();
    io.IniFilename = nullptr;
    io.LogFilename = nullptr;

    style.WindowPadding = ImVec2(8, 8);
    style.FramePadding = ImVec2(12, 5);
    style.CellPadding = ImVec2(3, 3);
    style.ItemSpacing = ImVec2(8, 4);
    style.ItemInnerSpacing = ImVec2(4, 4);
    style.TouchExtraPadding = ImVec2(0, 0);
    style.IndentSpacing = 21;
    style.ScrollbarSize = 14;
    style.GrabMinSize = 12;

    style.WindowBorderSize = 0;
    style.ChildBorderSize = 1;
    style.PopupBorderSize = 0;
    style.FrameBorderSize = 0;
    style.TabBorderSize = 0;

    style.WindowRounding = 5;
    style.ChildRounding = 0;
    style.FrameRounding = 3;
    style.PopupRounding = 0;
    style.ScrollbarRounding = 9;
    style.GrabRounding = 3;
    style.LogSliderDeadzone = 4;
    style.TabRounding = 4;

    style.WindowTitleAlign = ImVec2(0.50f, 0.50f);
    style.WindowMenuButtonPosition = ImGuiDir_Left;
    style.ColorButtonPosition = ImGuiDir_Right;
    style.ButtonTextAlign = ImVec2(0.50f, 0.50f);
    style.SelectableTextAlign = ImVec2(0.00f, 0.00f);

    style.DisplaySafeAreaPadding = ImVec2(3, 3);

    style.Colors[ImGuiCol_Text] = ImColor{0.95f, 0.95f, 0.95f, 1.00f};
    style.Colors[ImGuiCol_TextDisabled] = ImVec4{0.50f, 0.50f, 0.50f, 1.00f};;
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.03f, 0.03f, 0.03f, 0.94f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4{0.22f, 0.22f, 0.22f, 1.00f};
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.52f, 0.20f, 0.41f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.66f, 0.21f, 0.58f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.55f, 0.22f, 0.45f, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.63f, 0.24f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.51f, 0.23f, 0.42f, 1.00f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.59f, 0.22f, 0.48f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.63f, 0.24f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    style.Colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.92f, 0.24f, 0.84f, 0.20f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.92f, 0.24f, 0.84f, 0.67f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.92f, 0.24f, 0.84f, 0.95f);
    style.Colors[ImGuiCol_Tab] = ImVec4(0.51f, 0.23f, 0.42f, 1.00f);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.59f, 0.22f, 0.48f, 1.00f);
    style.Colors[ImGuiCol_TabActive] = ImVec4(0.63f, 0.24f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
    style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.65f, 0.24f, 0.57f, 0.38f);
    style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

    /*io.Fonts->Clear();

    float base_font_size = 14 * UE4SSProgram::settings_manager.Debug.DebugGUIFontScaling;

    // Increase font atlas size (if needed for many characters)
    io.Fonts->TexDesiredWidth = 2048; // Increase the atlas size to allow more glyphs to fit

    // Load base font (Latin characters)
    ImFontConfig font_cfg;
    font_cfg.FontDataOwnedByAtlas = false; // if true it will try to free memory and fail
    io.Fonts->AddFontFromMemoryTTF(Roboto, sizeof(Roboto), base_font_size, &font_cfg, io.Fonts->GetGlyphRangesDefault());
    font_cfg.FontDataOwnedByAtlas = false;
    
    // Build font atlas
    io.Fonts->Build();*/

    ImGui_ImplWin32_Init(window);
    ImGui_ImplDX11_Init(pDevice, pContext);
}

HRESULT __stdcall ResizeBuffersHook(
    IDXGISwapChain* pSwapChain,
    UINT BufferCount,
    UINT Width,
    UINT Height,
    DXGI_FORMAT NewFormat,
    UINT SwapChainFlags
)
{
    if (!pContext) return ResizeBuffersOrig(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
    pContext->OMSetRenderTargets(0, 0, 0);
    if (mainRenderTargetView)
    {
        mainRenderTargetView->Release();
        mainRenderTargetView = NULL;
    }
    auto ret = ResizeBuffersOrig(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
    ID3D11Texture2D* pBackBuffer;
    pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)& pBackBuffer);
    pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
    pBackBuffer->Release();
    pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
    
    D3D11_VIEWPORT vp;
    vp.Width = Width;
    vp.Height = Height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    pContext->RSSetViewports( 1, &vp );
    
    return ret;
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
        return true;

    return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

class GGSTModManager : public CppUserModBase
{
    static inline GUI::ModManager::ModManager m_pak_reloader{};
    std::thread d3d11_thread;
    static inline bool init = false;

public:
    GGSTModManager()
    {
        ModName = STR("GGST Mod Manager");
        ModVersion = STR("1.0");
        ModDescription = STR("In-game mod manager for Guilty Gear -Strive-");
        ModAuthors = STR("WistfulHopes");
    }

    static HRESULT __stdcall PresentHook(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
    {
        if (!init)
        {
            if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)& pDevice)))
            {
                pDevice->GetImmediateContext(&pContext);
                DXGI_SWAP_CHAIN_DESC sd;
                pSwapChain->GetDesc(&sd);
                window = sd.OutputWindow;
                ID3D11Texture2D* pBackBuffer;
                pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)& pBackBuffer);
                pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
                pBackBuffer->Release();
                oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
                InitImGui();
                init = true;
            }

            else
                return PresentOrig(pSwapChain, SyncInterval, Flags);
        }

        m_pak_reloader.render();
        
        pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        return PresentOrig(pSwapChain, SyncInterval, Flags);
    }
    
    auto init_d3d11()
    {
        bool init_hook = false;
        do
        {
            if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
            {
                kiero::bind(8, reinterpret_cast<void**>(&PresentOrig), PresentHook);
                kiero::bind(13, reinterpret_cast<void**>(&ResizeBuffersOrig), ResizeBuffersHook);
                init_hook = true;
            }
        } while (!init_hook);
        return true;
    }
    
    auto on_unreal_init() -> void override
    {
        m_pak_reloader.init();
        d3d11_thread = std::thread(&GGSTModManager::init_d3d11, this);

        register_keydown_event(Input::M, {Input::ModifierKey::CONTROL}, [&]
        {
            m_pak_reloader.toggle_open();
        });
    }
    
    ~GGSTModManager() override
    {
        kiero::shutdown();
    }
};

#define GGSTMODMANAGER_API __declspec(dllexport)
extern "C"
{
    GGSTMODMANAGER_API CppUserModBase* start_mod()
    {
        return new GGSTModManager();
    }

    GGSTMODMANAGER_API void uninstall_mod(CppUserModBase* mod)
    {
        delete mod;
    }
}

