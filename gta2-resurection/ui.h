#pragma once

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern WNDPROC originalWndProc;

void initUI(IDXGISwapChain*, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
void renderUI();
LRESULT CALLBACK _wndProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam);
