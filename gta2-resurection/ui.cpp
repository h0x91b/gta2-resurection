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
std::unordered_map<std::string, std::any> UIElements;

void renderUI() {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
    ImGui::Begin("Settings", &settings.open);
        for (auto& [key, value] : UIElements) {
            if (!strcmp(value.type().name(), "bool *")) {
                auto ref = std::any_cast<bool*>(value);
                ImGui::Checkbox(key.c_str(), ref);
            } else if (!strcmp(value.type().name(), "struct UISlider *")) {
                auto ref = std::any_cast<UISlider*>(value);
                ImGui::SliderInt(ref->label, &ref->v, ref->v_min, ref->v_max, ref->format, ref->flags);
            } else {
                OutputDebugStringA("Unknown type: ");
                OutputDebugStringA(value.type().name());
                OutputDebugStringA("\n");
            }
        }
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
