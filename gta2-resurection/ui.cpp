#include "pch.h"
#include "ui.h"
#include <stdio.h>
#include "defines.h"
#include "gta-helper.h"

ID3D11DeviceContext* pDeviceContext;
ID3D11RenderTargetView* mainRenderTargetView;
WNDPROC fnWndProc = (WNDPROC)0x004d0a00;

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

UISettings settings = {TRUE};

void renderUI() {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
    ImGui::Begin("Settings", &settings.open);
        ImGui::Checkbox("do_show_cycles", &settings.do_show_cycles);
        ImGui::Checkbox("do_show_physics", &settings.do_show_physics);
        ImGui::Checkbox("do_show_ids", &settings.do_show_ids);
        ImGui::Checkbox("do_free_shopping", &settings.do_free_shopping);
        ImGui::Checkbox("Fast cars", &settings.fast_cars);
        ImGui::Checkbox("Flamethrower", &settings.flamethrower);
        ImGui::Checkbox("Set Cop level", &settings.set_cop_level);
        ImGui::SliderInt("Cop level", &settings.copLevel, 0, 6);
    ImGui::End();

    ImGui::Render();

    // https://niemand.com.ar/2019/01/01/how-to-hook-directx-11-imgui/
    pDeviceContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

LRESULT CALLBACK _wndProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) {
    //printf("_wndProc 0x%x\n", msg);
    ImGuiIO& io = ImGui::GetIO();
     
    POINT mPos;
    GetCursorPos(&mPos);
    ScreenToClient(window, &mPos);
    ImGui::GetIO().MousePos.x = mPos.x;
    ImGui::GetIO().MousePos.y = mPos.y;

    if (io.WantCaptureMouse
        || io.WantCaptureKeyboard
        ) {
        ImGui_ImplWin32_WndProcHandler(window, msg, wParam, lParam);
        return TRUE;
    }
    return fnWndProc(window, msg, wParam, lParam);
}
