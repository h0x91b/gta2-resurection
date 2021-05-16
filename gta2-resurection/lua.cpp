#include "pch.h"
#include "lua.h"
#include <stdio.h>

lua_State* L;

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
    int x = luaL_dofile(L, "gta2.lua");// Загружает и запускает заданный файл. файл в которым все происходит.

    if (x != LUA_OK) {
        printf("Lua error: %s\n", lua_tostring(L, -1));
    }
    else {
        printf("Lua was ok\n");
    }
}
