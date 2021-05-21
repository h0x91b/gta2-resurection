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

thisCallHook(GameTick, 0x0045c1f0, Game*, void) {
    realGameTick(_this, _edx);

    static DWORD lastTicks = GetTickCount();
    DWORD currentTicks = GetTickCount();
    lua_getglobal(L, "gameTick");// получаем из lua функцию gameTick.
    lua_pushnumber(L, (float)(currentTicks - lastTicks) / 1000);// отправляем в стек число.
    auto x = lua_pcall(L, 1, 0, 0);// вызов функции, передаем 2 параметра, возвращаем 1.

    if (x != LUA_OK) {
        printf("Lua error: %s\n", lua_tostring(L, -1));
    }

    lastTicks = currentTicks;
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

    //AllocConsole();
    //FILE* fDummy;
    //freopen_s(&fDummy, "CONOUT$", "w", stdout);
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
    DetourTransactionCommit();

    printf("hwnd is found %X\n", hwnd);
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

