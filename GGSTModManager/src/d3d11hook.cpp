#include "d3d11hook.hpp"

// D3d Related Files
#include <imgui.h>
#include "imgui_impl_dx11.h"
#include <DXGI.h>
#include <d3d11.h>
#include <imgui_impl_win32.h>

#include <mutex>

#include "ModManager.hpp"
#include "safetyhook.hpp"
#include "DynamicOutput/Output.hpp"

#pragma comment(lib, "d3d11.lib")

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static HWND g_hWnd = nullptr;
static HMODULE g_hModule = nullptr;
static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static bool g_isInitialized;

SafetyHookInline shD3D11Present;
SafetyHookInline shD3D11ResizeBuffers;

DWORD_PTR* pSwapChainVTable = nullptr;

RC::GUI::ModManager::ModManager* pak_reloader;
ID3D11RenderTargetView* g_mainRenderTargetView;
HWND window = 0;
WNDPROC oWndProc;

namespace
{
    ImColor g_imgui_bg_color{0.22f, 0.22f, 0.22f, 1.00f};
    ImColor g_imgui_text_color = ImColor{0.95f, 0.95f, 0.95f, 1.00f};
    ImColor g_imgui_text_inactive_color = ImVec4{0.50f, 0.50f, 0.50f, 1.00f};
}

void SetModManager(RC::GUI::ModManager::ModManager* in_pak_reloader)
{
    pak_reloader = in_pak_reloader;
}

D3D11_HOOK_API void ImplHookDX11_Present(ID3D11Device* device, ID3D11DeviceContext* ctx, IDXGISwapChain* swap_chain)
{
    pak_reloader->render();
    g_pd3dContext->OMSetRenderTargets(1, &g_mainRenderTargetView, 0);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void InitImGui()
{
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    (void)io;
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

    style.Colors[ImGuiCol_Text] = g_imgui_text_color;
    style.Colors[ImGuiCol_TextDisabled] = g_imgui_text_inactive_color;
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.03f, 0.03f, 0.03f, 0.94f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg] = g_imgui_bg_color;
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

    io.Fonts->AddFontDefault();

    ImGui_ImplWin32_Init(g_hWnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dContext);
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, __uuidof( pBackBuffer ), reinterpret_cast<void**>(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView)
    {
        g_mainRenderTargetView->Release();
        g_mainRenderTargetView = NULL;
    }
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain)
    {
        g_pSwapChain->Release();
        g_pSwapChain = NULL;
    }
    if (g_pd3dContext)
    {
        g_pd3dContext->Release();
        g_pd3dContext = NULL;
    }
    if (g_pd3dDevice)
    {
        g_pd3dDevice->Release();
        g_pd3dDevice = NULL;
    }
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
        return true;

    return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
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
    CleanupRenderTarget();
    auto ret = shD3D11ResizeBuffers.call<HRESULT>(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
    CreateRenderTarget();
    return ret;
}

HRESULT __stdcall PresentHook(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
    g_pSwapChain = pSwapChain;

    if (!g_isInitialized)
    {
        g_pSwapChain->GetDevice(__uuidof(g_pd3dDevice), reinterpret_cast<void**>(&g_pd3dDevice));
        g_pd3dDevice->GetImmediateContext(&g_pd3dContext);

        DXGI_SWAP_CHAIN_DESC sd;
        g_pSwapChain->GetDesc(&sd);
        window = sd.OutputWindow;
        oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);

        CreateRenderTarget();

        InitImGui();
        g_isInitialized = true;
    }

    ImplHookDX11_Present(g_pd3dDevice, g_pd3dContext, g_pSwapChain);

    return shD3D11Present.call<HRESULT>(g_pSwapChain, SyncInterval, Flags);
}

DWORD __stdcall HookDX11_Init()
{
    D3D_FEATURE_LEVEL levels[] = {D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1};
    D3D_FEATURE_LEVEL obtainedLevel;
    DXGI_SWAP_CHAIN_DESC sd;
    {
        ZeroMemory(&sd, sizeof(sd));
        sd.BufferCount = 1;
        sd.BufferDesc.Width = 100;
        sd.BufferDesc.Height = 100;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        sd.OutputWindow = g_hWnd;
        sd.SampleDesc.Count = 1;
        sd.Windowed = (GetWindowLongPtr(g_hWnd, GWL_STYLE) & WS_POPUP) == 0;
        sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        sd.BufferDesc.Width = 1;
        sd.BufferDesc.Height = 1;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
    }

    HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, levels,
                                               sizeof(levels) / sizeof(D3D_FEATURE_LEVEL), D3D11_SDK_VERSION, &sd,
                                               &g_pSwapChain, &g_pd3dDevice, &obtainedLevel, &g_pd3dContext);
    if (FAILED(hr))
    {
        MessageBox(g_hWnd, L"Failed to create device and swapchain.", L"Fatal Error", MB_ICONERROR);
        return E_FAIL;
    }

    pSwapChainVTable = (DWORD_PTR*)g_pSwapChain;
    pSwapChainVTable = (DWORD_PTR*)pSwapChainVTable[0];

    shD3D11Present = safetyhook::create_inline((void*)pSwapChainVTable[8], &PresentHook);
    shD3D11ResizeBuffers = safetyhook::create_inline((void*)pSwapChainVTable[13], &ResizeBuffersHook);

    return S_OK;
}

D3D11_HOOK_API void ImplHookDX11_Init(HMODULE hModule, void* hwnd)
{
    g_hWnd = (HWND)hwnd;
    g_hModule = hModule;
    HookDX11_Init();
}

void ImplHookDX11_Shutdown()
{
    CleanupRenderTarget();
    if (g_pSwapChain)
    {
        g_pSwapChain->Release();
        g_pSwapChain = NULL;
    }
    if (g_pd3dContext)
    {
        g_pd3dContext->Release();
        g_pd3dContext = NULL;
    }
    if (g_pd3dDevice)
    {
        g_pd3dDevice->Release();
        g_pd3dDevice = NULL;
    }

    shD3D11Present = {};
    shD3D11ResizeBuffers = {};
}
