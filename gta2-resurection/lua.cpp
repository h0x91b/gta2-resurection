#include "pch.h"
#include "lua.h"
#include <stdio.h>
#include "ui.h"

lua_State* L;

int lPrint(lua_State* L) {
    int narg = lua_gettop(L);
    const size_t S = 4096;
    char buf[S];
    double n;
    sprintf_s(buf, S, "[LUA:%d]", narg);
    OutputDebugStringA(buf);
    for (auto i = 0; i < narg; i++) {
        if (LUA_TNUMBER == lua_type(L, i + 1)) {
            n = lua_tonumber(L, i + 1);
            if (n == (int)n) {
                sprintf_s(buf, S, " 0x%X(%i)", (int)n, (int)n);
                OutputDebugStringA(buf);
            }
            else {
                sprintf_s(buf, S, " %f", n);
                OutputDebugStringA(buf);
            }
        }
        else {
            sprintf_s(buf, S, " %s", lua_tostring(L, i + 1));
            OutputDebugStringA(buf);
        }
    }
    OutputDebugStringA("\n");
    return 0;
};

int lGetSettings(lua_State* L) {
    lua_pushnumber(L, (DWORD)&settings);// отправляем в стек число.
    return 1;
};

void initLua() {
    const size_t S = 4096;
    char buf[S];
    L = lua_open();

    // Load the libraries
    luaL_openlibs(L);

    lua_register(L, "print", lPrint);
    lua_register(L, "getSettings", lGetSettings);
    int x = luaL_dofile(L, "gta2.lua"); // compile & execute file

    if (x != LUA_OK) {
        sprintf_s(buf, S, "Lua error: %s\n", lua_tostring(L, -1));
        OutputDebugStringA(buf);
    }
    else {
        sprintf_s(buf, S, "Lua was ok\n");
        OutputDebugStringA(buf);
    }
}
