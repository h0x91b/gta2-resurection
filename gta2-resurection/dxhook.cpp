#include "pch.h"
#include <stdio.h>
#include "dxhook.h"
#include "ui.h"
#include "defines.h"

ID3D11Device* pDevice = nullptr;
IDXGISwapChain* pSwapchain = nullptr;
ID3D11DeviceContext* pContext = nullptr;
ID3D11RenderTargetView* pRenderTargetView = nullptr;
ID3D11VertexShader* pVertexShader = nullptr;
ID3D11InputLayout* pVertexLayout = nullptr;
ID3D11PixelShader* pPixelShader = nullptr;
ID3D11Buffer* pVertexBuffer = nullptr;
ID3D11Buffer* pIndexBuffer = nullptr;
ID3D11Buffer* pConstantBuffer = nullptr;

void* ogPresent;					// Pointer to the original Present function
fnPresent ogPresentTramp;			// Function pointer that calls the Present stub in our trampoline
void* pTrampoline = nullptr;		// Pointer to jmp instruction in our trampoline that leads to hkPresent
char ogBytes[PRESENT_STUB_SIZE];	// Buffer to store original bytes from Present

bool firstRun = true;

HRESULT __stdcall hkPresent(IDXGISwapChain* pThis, UINT SyncInterval, UINT Flags)
{
    pSwapchain = pThis;

    if (!pDevice)
    {
        HRESULT hr = pSwapchain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice);
    }
    if (firstRun) {
        firstRun = false;

        pDevice->GetImmediateContext(&pContext);
        initUI(pSwapchain, pDevice, pContext);
    }

    //printf("frame\n");
    //
    // render our shit
    //
    renderUI();

    return ogPresentTramp(pThis, SyncInterval, Flags);
}

bool HookD3D()
{
    trace;
    // Create a dummy device, get swapchain vmt, hook present.
    D3D_FEATURE_LEVEL featLevel;
    DXGI_SWAP_CHAIN_DESC sd{ 0 };
    sd.BufferCount = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.Height = 800;
    sd.BufferDesc.Width = 600;
    sd.BufferDesc.RefreshRate = { 60, 1 };
    sd.OutputWindow = GetForegroundWindow();
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    trace;
    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr, 
        D3D_DRIVER_TYPE_HARDWARE, //D3D_DRIVER_TYPE_REFERENCE, 
        nullptr, 
        0, 
        nullptr, 
        0, 
        D3D11_SDK_VERSION, 
        &sd, 
        &pSwapchain, 
        &pDevice, 
        &featLevel, 
        nullptr
    );
    printf("D3D11CreateDeviceAndSwapChain return HR %08X\n", hr);
    if (FAILED(hr)) {
        trace;
        return false;
    }
    trace;
    // Get swapchain vmt
    void** pVMT = *(void***)pSwapchain;

    trace;
    // Get Present's address out of vmt
    ogPresent = (fnPresent)(pVMT[VMT_PRESENT]);

    // got what we need, we can release device and swapchain now
    // we'll be stealing the game's.
    safe_release(pSwapchain);
    safe_release(pDevice);
    trace;
    // Create a code cave to trampoline to our hook
    // We'll try to do this close to present to make sure we'll be able to use a 5 byte jmp (important for x64)
    void* pLoc = (void*)((uintptr_t)ogPresent - 0x2000);
    void* pTrampLoc = nullptr;
    trace;
    while (!pTrampLoc)
    {
        trace;
        pTrampLoc = VirtualAlloc(pLoc, 1, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
        pLoc = (void*)((uintptr_t)pLoc + 0x200);
    }
    trace;
    ogPresentTramp = (fnPresent)pTrampLoc;

    // write original bytes to trampoline
    // write jmp to hook
    memcpy(ogBytes, ogPresent, PRESENT_STUB_SIZE);
    memcpy(pTrampLoc, ogBytes, PRESENT_STUB_SIZE);

    pTrampLoc = (void*)((uintptr_t)pTrampLoc + PRESENT_STUB_SIZE);
    trace;
    // write the jmp back into present
    *(char*)pTrampLoc = (char)0xE9;
    pTrampLoc = (void*)((uintptr_t)pTrampLoc + 1);
    uintptr_t ogPresRet = (uintptr_t)ogPresent + 5;
    *(int*)pTrampLoc = (int)(ogPresRet - (uintptr_t)pTrampLoc - 4);

    trace;
    // write the jmp to our hook
    pTrampoline = pTrampLoc = (void*)((uintptr_t)pTrampLoc + 4);
    // if x86, normal 0xE9 jmp
    *(char*)pTrampLoc = (char)0xE9;
    pTrampLoc = (void*)((uintptr_t)pTrampLoc + 1);
    *(int*)pTrampLoc = (uintptr_t)hkPresent - (uintptr_t)pTrampLoc - 4;

    trace;
    // hook present, place a normal mid-function at the beginning of the Present function
    return Hook(ogPresent, pTrampoline, PRESENT_STUB_SIZE);
}

bool Hook(void* pSrc, void* pDst, size_t size)
{
    trace;
    DWORD dwOld;
    uintptr_t src = (uintptr_t)pSrc;
    uintptr_t dst = (uintptr_t)pDst;

    if (!VirtualProtect(pSrc, size, PAGE_EXECUTE_READWRITE, &dwOld))
        return false;

    *(char*)src = (char)0xE9;
    *(int*)(src + 1) = (int)(dst - src - 5);

    VirtualProtect(pSrc, size, dwOld, &dwOld);
    return true;
}