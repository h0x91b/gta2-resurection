#pragma once
#include <lua.hpp>

#pragma comment(lib, "lua51.lib")

#define LUA_OK 0

// lua examples https://pastebin.com/8f9UH56W

void initLua();
extern lua_State* L;
