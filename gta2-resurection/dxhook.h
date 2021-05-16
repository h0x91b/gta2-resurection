#pragma once
#include <d3d11.h>

#pragma comment(lib, "d3d11.lib")

// https://github.com/guided-hacking/GH_D3D11_Hook/tree/master/GH_D3D11_Hook
#define safe_release(p) if (p) { p->Release(); p = nullptr; } 
#include "D3D_VMT_Indices.h"
#define VMT_PRESENT (UINT)IDXGISwapChainVMT::Present
#define PRESENT_STUB_SIZE 5

bool Hook(void* pSrc, void* pDst, size_t size);
HRESULT __stdcall hkPresent(IDXGISwapChain * pThis, UINT SyncInterval, UINT Flags);
using fnPresent = HRESULT(__stdcall*)(IDXGISwapChain* pThis, UINT SyncInterval, UINT Flags);

bool HookD3D();
