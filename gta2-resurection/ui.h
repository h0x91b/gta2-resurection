#pragma once

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#include <map>
#include <unordered_map>
#include <string>
#include <any>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern WNDPROC fnWndProc;

void initUI(IDXGISwapChain*, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
void renderUI();
LRESULT CALLBACK _wndProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam);
void log(const char* fmt, ...);

extern std::vector<std::pair<std::string, std::any>> UIElements;
extern std::vector<std::string> _log;

struct UISettings {
    bool open;
    bool do_show_cycles; 
    bool do_show_physics; 
    bool do_show_ids;
    bool do_free_shopping;
    bool fast_cars;
    bool flamethrower;
    bool set_cop_level;
    int copLevel;
};
extern UISettings settings;

struct UISlider {
    const char* label;
    int v;
    int v_min;
    int v_max;
    const char* format = "%d";
    ImGuiSliderFlags flags = 0;
};
