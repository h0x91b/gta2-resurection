// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <stdio.h>
#include <stdlib.h>
#include <detours.h>
#include "defines.h"
#include "gta-helper.h"
#include "dxhook.h"
#include "lua.h"
#include "ui.h"

#pragma comment(lib, "detours.lib")

void luaError() {
    const size_t S = 4096;
    char buf[S];
    sprintf_s(buf, S, "Lua error: %s\n", lua_tostring(L, -1));

    lua_Debug info;
    int level = 0;
    while (lua_getstack(L, level, &info)) {
        lua_getinfo(L, "nSl", &info);
        sprintf_s(buf, S, "  [%d] %s:%d -- %s [%s]\n",
            level, info.short_src, info.currentline,
            (info.name ? info.name : "<unknown>"), info.what);
        ++level;
    }
    OutputDebugStringA(buf);
}

thisCallHook(GameTick, 0x0045c1f0, Game*, void) {
    const size_t S = 4096;
    char buf[S];

    static DWORD lastTicks = GetTickCount();
    DWORD currentTicks = GetTickCount();
    lua_getglobal(L, "gameTickPre");
    lua_pushnumber(L, (float)(currentTicks - lastTicks) / 1000);
    auto x = lua_pcall(L, 1, 0, 0);

    if (x != LUA_OK) {
        luaError();
    }

    realGameTick(_this, _edx);

    lua_getglobal(L, "gameTick");
    lua_pushnumber(L, (float)(currentTicks - lastTicks) / 1000);
    x = lua_pcall(L, 1, 0, 0);

    if (x != LUA_OK) {
        luaError();
    }

    lastTicks = currentTicks;
}

thisCallHook(SetKeyState, 0x004a4930, Player_S4*, void, uint keys) {
    const size_t S = 4096;
    char buf[S];
    sprintf_s(buf, S, "SetKeyState(%08X)\n", keys);
    OutputDebugStringA(buf);

    lua_getglobal(L, "setKeyState");

    lua_pushnumber(L, keys);
    auto x = lua_pcall(L, 1, 1, 0);

    if (x != LUA_OK) {
        luaError();
    }
    else {
        keys = lua_tonumber(L, -1);
        sprintf_s(buf, S, "Modified SetKeyState(%08X)\n", keys);
        OutputDebugStringA(buf);
    }

    realSetKeyState(_this, _edx, keys);
}

// 0x0044b2c0 - 7 bytes
typedef void (__fastcall AddMoney)(void *_this, DWORD edx, int money);
AddMoney* oAddMoney = 0;
void __fastcall addMoney(void* _this, DWORD _edx, int money) {
    printf("addMoney %i!\n", money);

    oAddMoney(_this, _edx, money * 100);
}

DWORD WINAPI MainThread(HMODULE hModule) {
    OutputDebugString(L"MainThread");

    AllocConsole();
    FILE* fDummy;
    freopen_s(&fDummy, "CONOUT$", "w", stdout);
    printf("Console window activated, press HOME to activate detour\n");

    Sleep(1000);

    //while (!GetAsyncKeyState(VK_HOME)) {
    //    Sleep(50);
    //}

    // move windows
    RECT rect;
    auto hwnd = FindWindowA("WinMain", "GTA2");
    GetWindowRect(hwnd, &rect);
    auto screenX = GetSystemMetrics(SM_CXSCREEN);
    auto screenY = GetSystemMetrics(SM_CYSCREEN);
    auto width = rect.right - rect.left;
    auto height = rect.bottom - rect.top;
    MoveWindow(hwnd,
        screenX - width,
        0,
        width,
        height,
        true
    );
	
    printf("DetourTransactionCommit done\n");
    Sleep(100);
    HookD3D();
    printf("HookD3D done\n");
    Sleep(100);
    initLua();
    printf("initLua done\n");
    Sleep(100);

    DetourRestoreAfterWith();

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    // DetourAttach(&(PVOID&)TrueSleep, TimedSleep);
    DetourAttach(&(PVOID&)fnWndProc, _wndProc);
    Attach(GameTick);
    Attach(SetKeyState);
    DetourTransactionCommit();

    //printf("hwnd is found %X\n", hwnd);
    //originalWndProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)_wndProc);
    printf("SetWindowLongPtr done\n");

    int frames = 0;
    while (true) {
        // our code
        if (GetAsyncKeyState(VK_F1)) {
            lua_close(L);
            initLua();
            Sleep(1000);
        }
        Sleep(100);
    }

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    //DetourDetach(&(PVOID&)TrueSleep, TimedSleep);
    DetourDetach(&(PVOID&)fnWndProc, _wndProc);
    Dettach(GameTick);
    Dettach(SetKeyState);
    DetourTransactionCommit();
   
    printf("Dettach and shutdown everything\n");
    

    printf("lua_close\n");
    lua_close(L);
    
    printf("FreeConsole\n");
    //fclose(fDummy); 
    //FreeConsole();
    
    FreeLibraryAndExitThread(hModule, 0);
    ExitProcess(0);
    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)MainThread, hModule, 0, 0));
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

