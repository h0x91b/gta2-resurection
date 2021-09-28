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

    UIElements.push_back(std::make_pair< std::string, std::any>(val->label, val));
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

    UIElements.push_back(std::make_pair< std::string, std::any>(lua_tostring(L, 1), val));
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
    log("[%s:%d]",
        info.short_src, info.currentline
    );
    
    OutputDebugStringA(buf);
    for (auto i = 0; i < narg; i++) {
        if (LUA_TNUMBER == lua_type(L, i + 1)) {
            n = lua_tonumber(L, i + 1);
            if (n == (int)n) {
                sprintf_s(buf, S, " 0x%X(%i)", (int)n, (int)n);
                log(" 0x%X(%i)", (int)n, (int)n);
                OutputDebugStringA(buf);
            }
            else {
                sprintf_s(buf, S, " %f", n);
                log(" %f", n);
                OutputDebugStringA(buf);
            }
        }
        else if (LUA_TTABLE == lua_type(L, i + 1)) {
            sprintf_s(buf, S, " <table>");
            log(" <table>");
            OutputDebugStringA(buf);
        }
        else if (LUA_TBOOLEAN == lua_type(L, i + 1)) {
            bool b = lua_toboolean(L, i + 1);
            sprintf_s(buf, S, " %s", b ? "true" : "false");
            log(" %s", b ? "true" : "false");
            OutputDebugStringA(buf);
        }
        else if (LUA_TSTRING == lua_type(L, i + 1)) {
            sprintf_s(buf, S, " %s", lua_tostring(L, i + 1));
            log(" %s", lua_tostring(L, i + 1));
            OutputDebugStringA(buf);
        }
        else {
            sprintf_s(buf, S, " %s(%d)", lua_tostring(L, i + 1), lua_type(L, i + 1));
            log(" %s(%d)", lua_tostring(L, i + 1), lua_type(L, i + 1));
            OutputDebugStringA(buf);
        }
    }
    OutputDebugStringA("\n");
    log("\n");
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

    auto it = std::find_if(
        UIElements.begin(), 
        UIElements.end(),
        [&k](const std::pair<std::string, std::any>& element) { 
            return element.first == k; 
        }
    );
    
    if (it == UIElements.end()) {
        lua_pushliteral(L, "Setting with such name is not found");
        return -1;  /* error flag */
    }
    auto setting = it->second;

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
    HANDLE hFind = FindFirstFileA("scripts\\mod-*.lua", &data);      // DIRECTORY

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

int lGetCursorPos(lua_State* L) {
    POINT p;
    GetCursorPos(&p);
    
    RECT wrect;
    auto hwnd = FindWindowA("WinMain", "GTA2");
    GetWindowRect(hwnd, &wrect);
    
    double x, y;

    // https://stackoverflow.com/questions/56549853/windows-getting-a-window-title-bars-height/56620749
    RECT crect;
    GetClientRect(hwnd, &crect);
    POINT lefttop = { crect.left, crect.top }; // Practicaly both are 0
    ClientToScreen(hwnd, &lefttop);
    POINT rightbottom = { crect.right, crect.bottom };
    ClientToScreen(hwnd, &rightbottom);

    int left_border = lefttop.x - wrect.left; // Windows 10: includes transparent part
    int right_border = wrect.right - rightbottom.x; // As above
    int bottom_border = wrect.bottom - rightbottom.y; // As above
    int top_border_with_title_bar = lefttop.y - wrect.top; // There is no transparent part

    auto width = wrect.right - wrect.left - left_border - right_border;
    auto height = wrect.bottom - wrect.top - top_border_with_title_bar - bottom_border;

    x = (p.x - wrect.left - left_border) / (double)width;
    y = (p.y - wrect.top - top_border_with_title_bar) / (double)(height);

    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    lua_pushnumber(L, width);
    lua_pushnumber(L, height);

    return 4;
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
    lua_register(L, "getCursorPos", lGetCursorPos);
    lua_register(L, "addBooleanSetting", lAddBooleanSetting);
    lua_register(L, "addSliderSetting", lAddSliderSetting);
    lua_register(L, "getSetting", lGetSetting);
    int x = luaL_dofile(L, "scripts\\gta2.lua"); // compile & execute file

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

void LeftMouse(bool isDown) {
    lua_pushboolean(L, isDown);
    lua_setglobal(L, "IsLeftMouseDown");
}

void RightMouse(bool isDown) {
    lua_pushboolean(L, isDown);
    lua_setglobal(L, "IsRightMouseDown");
}

void restartLua() {
    lua_close(L);
    for (auto& [key, value] : UIElements) {
        if (!strcmp(value.type().name(), "bool *")) {
            auto ref = std::any_cast<bool*>(value);
            delete ref;
        }
        else if (!strcmp(value.type().name(), "struct UISlider *")) {
            auto ref = std::any_cast<UISlider*>(value);
            delete ref;
        }
        else {
            OutputDebugStringA("Unknown type: ");
            OutputDebugStringA(value.type().name());
            OutputDebugStringA("\n");
        }
    }
    UIElements.clear();
    initLua();
}
