#include "pch.h"
#include "ui.h"
#include <stdio.h>
#include "defines.h"
#include "gta-helper.h"
#include "lua.h"

#pragma comment(lib, "discord_game_sdk.dll.lib")

discord::Core* core{};

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

	auto result = discord::Core::Create(898242996468871178, DiscordCreateFlags_Default, &core);
	discord::Activity activity{};
	activity.SetState("In main menu");
	//activity.SetDetails("Fruit Loops");
	core->ActivityManager().UpdateActivity(activity, [](discord::Result result) {

		});
}

bool show_demo_window = false;
bool show_mods_window = true;

UISettings settings = {0};

std::vector<std::pair<std::string, std::any>> UIElements;

void renderUI() {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

#ifdef _DEBUG
    ImGui::ShowDemoWindow(&show_demo_window);
#endif

    ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
    ImGui::Begin("Settings", &show_mods_window, ImGuiWindowFlags_AlwaysAutoResize);

    // Red reload button
    {
        ImGui::PushID(1);
        ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0 / 7.0f, 0.6f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0 / 7.0f, 0.7f, 0.7f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0 / 7.0f, 0.8f, 0.8f));
        if (ImGui::Button("Restart LUA")) {
            OutputDebugStringA("Restart LUA\n");
            restartLua();
        }
        ImGui::PopStyleColor(3);
        ImGui::PopID();
        /*
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Reload all LUA scripts from HD");
        }
        */
        ImGui::SameLine();
        ImGui::TextWrapped("Reload all LUA scripts from Hard Disk");
    }

    if (ImGui::BeginTabBar("Mods", 0))
    {
        if (ImGui::BeginTabItem("General"))
        {
            for (auto& [key, value] : UIElements) {
                if (!strcmp(value.type().name(), "bool *")) {
                    auto ref = std::any_cast<bool*>(value);
                    ImGui::Checkbox(key.c_str(), ref);
                }
                else if (!strcmp(value.type().name(), "struct UISlider *")) {
                    auto ref = std::any_cast<UISlider*>(value);
                    ImGui::SliderInt(ref->label, &ref->v, ref->v_min, ref->v_max, ref->format, ref->flags);
                }
                else {
                    OutputDebugStringA("Unknown type: ");
                    OutputDebugStringA(value.type().name());
                    OutputDebugStringA("\n");
                }
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Logs"))
        {
            ImGui::BeginChild("Log", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

            for (auto i = _log.rbegin(); i != _log.rend(); i++) {
                ImGui::TextUnformatted(i->c_str());
            }

            ImGui::PopStyleVar();
            ImGui::EndChild();
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
    
    ImGui::End();

    ImGui::Render();

    // https://niemand.com.ar/2019/01/01/how-to-hook-directx-11-imgui/
    pDeviceContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    ShowCursor(1);
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
    } else if (msg == WM_LBUTTONDOWN) {
        LeftMouse(true);
    } else if (msg == WM_LBUTTONUP) {
        LeftMouse(false);
    } else if (msg == WM_RBUTTONDOWN) {
        RightMouse(true);
    } else if (msg == WM_RBUTTONUP) {
        RightMouse(false);
    }

    core->RunCallbacks();
    return fnWndProc(window, msg, wParam, lParam);
}

std::vector<std::string> _log;

void log(const char* fmt, ...) {
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    auto len = vsnprintf(buf, 1024, fmt, args);
    va_end(args);
    _log.push_back(buf);
    OutputDebugStringA(buf);

    if (_log.size() > 100) {
        _log.erase(_log.begin());
    }
}