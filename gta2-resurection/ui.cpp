#include "pch.h"
#include "ui.h"
#include <stdio.h>
#include "defines.h"

ID3D11DeviceContext* pDeviceContext;
ID3D11RenderTargetView* mainRenderTargetView;
WNDPROC originalWndProc = nullptr;

void initUI(IDXGISwapChain* pSwapchain, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    auto hwnd = FindWindowA("WinMain", "GTA2");
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(pDevice, pDeviceContext);

    ::pDeviceContext = pDeviceContext;

    ImGui::GetIO().ImeWindowHandle = hwnd;
    ID3D11Texture2D* pBackBuffer;

    pSwapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
    pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
    pBackBuffer->Release();
}

bool show_demo_window = true;

void renderUI() {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    ImGui::Render();

    // https://niemand.com.ar/2019/01/01/how-to-hook-directx-11-imgui/
    pDeviceContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

LRESULT CALLBACK _wndProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) {
    //printf("_wndProc\n");
    ImGuiIO& io = ImGui::GetIO();

    POINT mPos;
    GetCursorPos(&mPos);
    ScreenToClient(window, &mPos);
    ImGui::GetIO().MousePos.x = mPos.x;
    ImGui::GetIO().MousePos.y = mPos.y;

    // ImGui_ImplWin32_WndProcHandler(window, msg, wParam, lParam);

    if (io.WantCaptureMouse
        || io.WantCaptureKeyboard
        ) {
        ImGui_ImplWin32_WndProcHandler(window, msg, wParam, lParam);
        return TRUE;
    }
    return CallWindowProc(originalWndProc, window, msg, wParam, lParam);
}
