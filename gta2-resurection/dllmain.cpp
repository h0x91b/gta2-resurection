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
#pragma endregion

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

void lGameTick() {
    static DWORD lastTicks = GetTickCount();
    DWORD currentTicks = GetTickCount();
    lua_getglobal(L, "gameTick");// получаем из lua функцию gameTick.
    lua_pushnumber(L, (float)(currentTicks-lastTicks)/1000);// отправляем в стек число.
    auto x = lua_pcall(L, 1, 0, 0);// вызов функции, передаем 2 параметра, возвращаем 1.

    if (x != LUA_OK) {
        printf("Lua error: %s\n", lua_tostring(L, -1));
    }

    lastTicks = currentTicks;
}

void initLua() {
    L = lua_open();

    // Load the libraries
    luaL_openlibs(L);

    lua_register(L, "pprint", lPrint);
    int x = luaL_dofile(L, "gta2.lua");// Загружает и запускает заданный файл. файл в которым все происходит.

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

    DetourRestoreAfterWith();

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    // DetourAttach(&(PVOID&)TrueSleep, TimedSleep);
    DetourTransactionCommit();
	printf("Detour complete\n");

    initLua();

    while (!GetAsyncKeyState(VK_F2)) {
        // our code
        lGameTick();
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

