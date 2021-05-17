#pragma once

#define cast(type, x) (type*)*(DWORD*)x
#define trace printf("%s:%i %s\n", __FILE__, __LINE__, __FUNCTION__)

#define thisCallHook(fnName, addr, thisType, retType, ...) \
    typedef retType (__fastcall fnName)(thisType _this, DWORD _edx, __VA_ARGS__); \
    fnName* real##fnName = (fnName*)addr; \
    retType __fastcall _##fnName(thisType _this, DWORD _edx, __VA_ARGS__)

#define Attach(fnName) DetourAttach(&(PVOID&)real##fnName, _##fnName);
#define Dettach(fnName) DetourDetach(&(PVOID&)real##fnName, _##fnName);
