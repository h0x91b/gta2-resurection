// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <stdio.h>
#include <stdlib.h>
#include <lua.hpp>
#include <detours.h>

#pragma comment(lib, "lua51.lib")
#pragma comment(lib, "detours.lib")

#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1)     /* set alignment to 1 byte boundary */
#include "gta2.h"
#pragma pack(pop)   /* restore original alignment from stack */

#pragma region asserts
static_assert(sizeof(Ped) == 0x290, "Wrong size of Car struct");
static_assert(offsetof(struct Ped, health) == 0x216, "Wrong offset of health in Ped struct");
#pragma endregion

#define LUA_OK 0

#define cast(type, x) (type*)*(DWORD*)x

// lua examples https://pastebin.com/8f9UH56W
lua_State* L;

#pragma region gta
Game* game = cast(Game, 0x005eb4fc);

typedef Ped* (__stdcall GetPedById)(int);
static GetPedById* fnGetPedByID = (GetPedById*)0x0043ae10;

// 0045c1f0 - void __fastcall GameTick(Game *game)
#pragma endregion

#define thisCallHook(fnName, addr, thisType, retType, ...) \
    typedef retType (__fastcall fnName)(thisType _this, DWORD _edx, __VA_ARGS__); \
    fnName* real##fnName = (fnName*)addr; \
    retType __fastcall _##fnName(thisType _this, DWORD _edx, __VA_ARGS__)

#define Attach(fnName) DetourAttach(&(PVOID&)real##fnName, _##fnName);
#define Dettach(fnName) DetourDetach(&(PVOID&)real##fnName, _##fnName);

thisCallHook(GameTick, 0x0045c1f0, Game*, void) {
    realGameTick(_this, _edx);

    static DWORD lastTicks = GetTickCount();
    DWORD currentTicks = GetTickCount();
    lua_getglobal(L, "gameTick");// �������� �� lua ������� gameTick.
    lua_pushnumber(L, (float)(currentTicks - lastTicks) / 1000);// ���������� � ���� �����.
    auto x = lua_pcall(L, 1, 0, 0);// ����� �������, �������� 2 ���������, ���������� 1.

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

int lPrint(lua_State* L) {
    int narg = lua_gettop(L);
    double n;
    printf("[LUA:%d]", narg);
    for (auto i = 0; i < narg; i++) {
        if (LUA_TNUMBER == lua_type(L, i + 1)) {
            n = lua_tonumber(L, i + 1);
            if (n == (int)n) {
                printf(" 0x%X(%i)", (int)n, (int)n);
            }
            else {
                printf(" %f", n);
            }
        }
        else {
            printf(" %s", lua_tostring(L, i + 1));
        }
    }
    printf("\n");
    return 0;
};

void initLua() {
    L = lua_open();

    // Load the libraries
    luaL_openlibs(L);

    lua_register(L, "print", lPrint);
    int x = luaL_dofile(L, "gta2.lua");// ��������� � ��������� �������� ����. ���� � ������� ��� ����������.

    if (x != LUA_OK) {
        printf("Lua error: %s\n", lua_tostring(L, -1));
    }
    else {
        printf("Lua was ok\n");
    }
}

DWORD WINAPI MainThread(HMODULE hModule) {
    OutputDebugString(L"MainThread");

    AllocConsole();
    FILE* fDummy;
    freopen_s(&fDummy, "CONOUT$", "w", stdout);
    printf("Console window activated, press HOME to activate detour\n");

    while (!GetAsyncKeyState(VK_HOME)) {
        Sleep(50);
    }

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

    hwnd = FindWindowA("ConsoleWindowClass", 0);

    GetWindowRect(hwnd, &rect);
    width = rect.right - rect.left;
    height = rect.bottom - rect.top;
    MoveWindow(hwnd,
        screenX - width,
        screenY - height,
        width,
        height,
        true
    );
    //GetNextWindow()

    DetourRestoreAfterWith();

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    // DetourAttach(&(PVOID&)TrueSleep, TimedSleep);
    Attach(GameTick);
    DetourTransactionCommit();
	printf("Detour complete\n");

    initLua();

    while (!GetAsyncKeyState(VK_F2)) {
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
    Dettach(GameTick);
    DetourTransactionCommit();
    printf("Dettach and shutdown everything\n");

    // Close lua
    lua_close(L);

    FreeLibraryAndExitThread(hModule, 0);
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

