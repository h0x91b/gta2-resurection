// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <stdio.h>
#include <stdlib.h>
#include <lua.hpp>

#pragma comment(lib, "lua51.lib")

#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1)     /* set alignment to 1 byte boundary */
#include "gta2.h"
#pragma pack(pop)   /* restore original alignment from stack */

#pragma region asserts
static_assert(sizeof(Ped) == 0x290, "Wrong size of Car struct");
static_assert(offsetof(struct Ped, health) == 0x216, "Wrong offset of health in Unit struct");
#pragma endregion

#define LUA_OK 0

// lua examples https://pastebin.com/8f9UH56W
lua_State* L;

#pragma region gtaFuncs
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

BOOL DetourFunc(const DWORD originalFn, DWORD hookFn, size_t copyBytes = 5) {
	DWORD OldProtection = { 0 };

	char* trampoline = new char[copyBytes + 5];

    BOOL success = VirtualProtectEx(GetCurrentProcess(), (LPVOID)trampoline, copyBytes + 5, PAGE_EXECUTE_READWRITE, &OldProtection);
    if (!success) {
        DWORD error = GetLastError();
        printf("Last error: %d\n", error);
        return 0;
    }

	oAddMoney = (AddMoney*)trampoline;
    
	memcpy(trampoline, (void*)originalFn, copyBytes);
	trampoline += copyBytes;
	*trampoline = 0xE9;

	DWORD offset = (((DWORD)originalFn + copyBytes) - ((DWORD)trampoline + 5)); //Offset math.
	*(DWORD*)((LPBYTE)trampoline + 1) = offset;

	success = VirtualProtectEx(GetCurrentProcess(), (LPVOID)originalFn, copyBytes, PAGE_EXECUTE_READWRITE, &OldProtection);
	if (!success) {
		DWORD error = GetLastError();
		printf("Last error: %d\n", error);
		return 0;
	}

	*(BYTE*)((LPBYTE)originalFn) = 0xE9; //JMP FAR
	offset = (((DWORD)hookFn) - ((DWORD)originalFn + 5)); //Offset math.
	*(DWORD*)((LPBYTE)originalFn + 1) = offset;

	return 1;
}

struct SDetour {
    void* originalFn = nullptr;
    void* tramplineInFn = nullptr; // from originalFn to our StandardDetourFn
    void* tramplineOutFn = nullptr;
    int hookFn = 0; // index in lua ref registry
    size_t opcodeBytes = 5;
    size_t numArgs = 0;
};

void __declspec(naked) StandardDetourFn() {
    static SDetour* s = nullptr;
    __asm {
        pop s
    }
    printf("StandardDetourFn args=%d hookFn=%d\n", s->numArgs, s->hookFn);
    static int b = s->numArgs * 4;
    // call lua

    lua_rawgeti(L, LUA_REGISTRYINDEX, s->hookFn);
    static DWORD *_esp;
    static DWORD arg;

    for (int i = 0; i < s->numArgs; i++) {
        __asm {
            mov _esp, esp
        }
        arg = *(_esp + i + 1);
        lua_pushnumber(L, arg);
    }

    static int x = lua_pcall(L, s->numArgs, 0, 0);

    if (x != LUA_OK) {
        printf("Lua error: %s\n", lua_tostring(L, -1));
    }

    static DWORD tramplineOutFn = (DWORD)s->tramplineOutFn;
    __asm {
        //add esp, b
        mov eax, 0
        jmp tramplineOutFn
    }
}

DWORD relativeAddr(DWORD from, DWORD to) {
    return to - from - 5;
}

int lDetourAttach(lua_State* L) {
    int narg = lua_gettop(L);
    if (narg != 4) {
        printf("Wrong args count, must be 4\n");
        return 0;
    }

    auto s = (SDetour*)malloc(sizeof(SDetour));
    
    auto originalFn = (void*)(DWORD)lua_tonumber(L, 2);

    auto opcodeBytes = (size_t)lua_tonumber(L, 3);
    auto numArgs = (size_t)lua_tonumber(L, 4);

    lua_pop(L, 3);
    if (!lua_isfunction(L, 1)) {
        printf("Second argument must be a function\n");
        return 0;
    }
    //auto hookFn = (void*)(DWORD)lua_tonumber(L, -1);
    auto hookFn = luaL_ref(L, LUA_REGISTRYINDEX);
    // lua_rawgeti(L, LUA_REGISTRYINDEX, ref);

    char* tramplineInFn = (char*)malloc(5 + 5);
    *(BYTE*)((DWORD)tramplineInFn + 0) = 0x68; // PUSH
    *(DWORD*)((DWORD)tramplineInFn + 1) = (DWORD)s; // Addr of struct
    *(BYTE*)((DWORD)tramplineInFn + 5) = 0xE9; // JMP relative
    *(DWORD*)((DWORD)tramplineInFn + 6) = relativeAddr((DWORD)tramplineInFn + 5, (DWORD)StandardDetourFn);

    DWORD OldProtection;
    // allow execution of tramplineInFn
    BOOL success = VirtualProtectEx(GetCurrentProcess(), (LPVOID)tramplineInFn, 5 + 5, PAGE_EXECUTE_READWRITE, &OldProtection);
    if (!success) {
        DWORD error = GetLastError();
        printf("Last error: %d\n", error);
        return 0;
    }

    char* tramplineOutFn = (char*)malloc(5 + opcodeBytes);
    success = VirtualProtectEx(GetCurrentProcess(), (LPVOID)tramplineOutFn, 5 + 5, PAGE_EXECUTE_READWRITE, &OldProtection);
    if (!success) {
        DWORD error = GetLastError();
        printf("Last error: %d\n", error);
        return 0;
    }
    memcpy(tramplineOutFn, originalFn, opcodeBytes);
    *(BYTE*)((DWORD)tramplineOutFn + opcodeBytes) = 0xE9; // JMP relative
    *(DWORD*)((DWORD)tramplineOutFn + opcodeBytes + 1) = relativeAddr((DWORD)tramplineOutFn + opcodeBytes, (DWORD)originalFn + 5);

    // allow write to original fn
    success = VirtualProtectEx(GetCurrentProcess(), (LPVOID)originalFn, opcodeBytes, PAGE_EXECUTE_READWRITE, &OldProtection);
    if (!success) {
        DWORD error = GetLastError();
        printf("Last error: %d\n", error);
        return 0;
    }

    *(BYTE*)((DWORD)originalFn + 0) = 0xE9; // JMP relative
    *(DWORD*)((DWORD)originalFn + 1) = relativeAddr((DWORD)originalFn, (DWORD)tramplineInFn);
    for (int i = 5; i < opcodeBytes; i++) {
        *(BYTE*)((DWORD)originalFn + i) = 0x90; // NOP
    }

    s->originalFn = originalFn;
    s->tramplineInFn = tramplineInFn;
    s->tramplineOutFn = tramplineOutFn;
    s->hookFn = hookFn;
    s->opcodeBytes = opcodeBytes;
    s->numArgs = numArgs;

    printf("lDetourAttach(0x%08X, %i, %i, %i)\n", originalFn, hookFn, opcodeBytes, numArgs);

    lua_pushinteger(L, (DWORD)s);
    return 1;
}

int lDetourDettach(lua_State* L) {

    return 0;
}

int lSetPedHealthById(lua_State* L) {
    int narg = lua_gettop(L);
    if (narg != 2) {
        printf("Wrong args count, must be 2\n");
        return 1;
    }

    auto id = (int)lua_tonumber(L, 1);
    auto health = (short)lua_tonumber(L, 2);
    auto ped = fnGetPedByID(id);
    if (!ped) {
        printf("Ped doesnt exists\n");
        return 1;
    }
    ped->health = health;
    return 0;
}

int lGetPedById(lua_State* L) {
    int narg = lua_gettop(L);
    if (narg != 1) {
        printf("Wrong args count, must be 2\n");
        return 1;
    }
    auto id = (int)lua_tonumber(L, 1);
    printf("lGetPedById(%i)\n", id);
    auto ped = fnGetPedByID(id);
    if (!ped) {
        printf("Ped doesnt exists\n");
        lua_pushinteger(L, 0);
        return 1;
    }
    lua_pushinteger(L, (DWORD)ped);
    return 1;
}

int lPrint(lua_State* L) {
    int narg = lua_gettop(L);
    printf("[LUA:%d]", narg);
    for (auto i = 0; i < narg; i++) {
        printf(" %s", lua_tostring(L, i + 1));
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

    lua_register(L, "print", lPrint);
    lua_register(L, "SetPedHealthById", lSetPedHealthById);
    lua_register(L, "GetPedById", lGetPedById);
    lua_register(L, "DetourAttach", lDetourAttach);

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

	//DetourFunc(0x0044b2c0, (DWORD)addMoney, 7u);
	//printf("Detour complete\n");

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

    OutputDebugString(L"Dettach and shutdown everything\r\n");
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

