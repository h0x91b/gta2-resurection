#include "pch.h"
#include "lua.h"
#include <stdio.h>
#include "ui.h"

lua_State* L;

int lAddSliderSetting(lua_State* L) {
    int narg = lua_gettop(L);
    const size_t S = 4096;
    char buf[S];

    if (narg != 4 && narg != 6) {
        // error
        lua_pushliteral(L, "This function requires exactly 4 or 6 arguments <Label>, <DefaultValue>, <Min>, <Max>, <Format>, <Flags>");
        return -1;  /* error flag */
    }

    UISlider* val = new UISlider;

    val->label = lua_tostring(L, 1);
    val->v = lua_tonumber(L, 2);
    val->v_min = lua_tonumber(L, 3);
    val->v_max = lua_tonumber(L, 4);

    if (narg == 6) {
        val->format = lua_tostring(L, 5);
        val->flags = lua_tonumber(L, 6);
    }

    UIBools[val->label] = val;
    return 0;
}

int lAddBooleanSetting(lua_State* L) {
    int narg = lua_gettop(L);
    const size_t S = 4096;
    char buf[S];

    if (narg != 2) {
        // error
        lua_pushliteral(L, "This function requires exactly 2 argument <Label>, <DefaultValue>");
        return -1;  /* error flag */
    }

    bool* val = new bool;
    *val = static_cast<bool>(lua_toboolean(L, 2));

    UIBools[lua_tostring(L, 1)] = val;
    return 0;
}

int lPrint(lua_State* L) {
    int narg = lua_gettop(L);
    const size_t S = 4096;
    char buf[S];
    double n;
    lua_Debug info;
    int level = 1;
    lua_getstack(L, level, &info);
    lua_getinfo(L, "nSl", &info);
    sprintf_s(buf, S, 
        "[%s:%d]",
        info.short_src, info.currentline
    );
    
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
        else if (LUA_TTABLE == lua_type(L, i + 1)) {
            sprintf_s(buf, S, " <table>");
            OutputDebugStringA(buf);
        }
        else if (LUA_TBOOLEAN == lua_type(L, i + 1)) {
            bool b = lua_toboolean(L, i + 1);
            sprintf_s(buf, S, " %s", b ? "true" : "false");
            OutputDebugStringA(buf);
        }
        else {
            sprintf_s(buf, S, " %s(%d)", lua_tostring(L, i + 1), lua_type(L, i + 1));
            OutputDebugStringA(buf);
        }
    }
    OutputDebugStringA("\n");
    return 0;
}

int lGetSettings(lua_State* L) {
    lua_pushnumber(L, (DWORD)&settings); // return pointer to struct
    return 1;
}

int lGetSetting(lua_State* L) {
    int narg = lua_gettop(L);
    if (narg != 1) {
        // error
        lua_pushliteral(L, "This function requires exactly 1 argument <Label>");
        return -1;  /* error flag */
    }

    auto k = lua_tostring(L, 1);
    
    if (UIBools.find(k) == UIBools.end()) {
        lua_pushliteral(L, "Setting with such name is not found");
        return -1;  /* error flag */
    }
    auto setting = UIBools[k];

    if (!strcmp(setting.type().name(), "bool *")) {
        lua_pushboolean(L, *(std::any_cast<bool*>(setting)));
        return 1;
    } else if (!strcmp(setting.type().name(), "struct UISlider *")) {
        auto ref = std::any_cast<UISlider*>(setting);
        lua_pushnumber(L, ref->v);
        return 1;
    }

    OutputDebugStringA("Unknown type: ");
    OutputDebugStringA(setting.type().name());
    OutputDebugStringA("\n");
    
    lua_pushliteral(L, "Unknown type of std::any");
    return -1;  /* error flag */
}

int lListMods(lua_State* L) {
    lua_newtable(L);
    WIN32_FIND_DATAA data;
    HANDLE hFind = FindFirstFileA("mod-*.lua", &data);      // DIRECTORY

    if (hFind != INVALID_HANDLE_VALUE) {
        int i = 1;
        do {
            lua_pushstring(L, data.cFileName);
            lua_rawseti(L, -2, i++);
        } while (FindNextFileA(hFind, &data));
        FindClose(hFind);
    }
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
    lua_register(L, "listMods", lListMods);
    lua_register(L, "addBooleanSetting", lAddBooleanSetting);
    lua_register(L, "addSliderSetting", lAddSliderSetting);
    lua_register(L, "getSetting", lGetSetting);
    int x = luaL_dofile(L, "gta2.lua"); // compile & execute file

    if (x != LUA_OK) {
        sprintf_s(buf, S, "Lua error: %s\n", lua_tostring(L, -1));
        OutputDebugStringA(buf);

        lua_Debug info;
        int level = 0;
        while (lua_getstack(L, level, &info)) {
            lua_getinfo(L, "nSl", &info);
            sprintf_s(buf, S, "  [%d] %s:%d -- %s [%s]\n",
                level, info.short_src, info.currentline,
                (info.name ? info.name : "<ROOT>"), info.what);
            ++level;
            OutputDebugStringA(buf);
        }

    }
    else {
        sprintf_s(buf, S, "Lua was ok\n");
        OutputDebugStringA(buf);
    }
}
