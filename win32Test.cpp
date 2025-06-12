#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <iostream>

using namespace DirectX;
void DrawScreen(float clearColor[4]);


HWND g_hWnd = nullptr;

// 사용자에 의해 바뀔 수 있는 값들
UINT g_width = 1280;
UINT g_height = 720;
UINT g_wPosX = 650;
UINT g_wPosY = 300;
float defaultClearColor[4] = { 0.3f, 0.3f, 0.3f, 1.f };
float resizeClearColor[4] = { 0.1f, 0.3f, 0.1f, 1.f };

// Direct3D 전역 변수
ID3D11Device* g_device = nullptr;
ID3D11DeviceContext* g_context = nullptr;
IDXGISwapChain* g_swapChain = nullptr;
ID3D11RenderTargetView* g_renderTargetView = nullptr;
ID3D11Texture2D* g_depthStencilBuffer = nullptr;
ID3D11DepthStencilView* g_depthStencilView = nullptr;

// 윈도우 프로시저
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    switch (msg)
    {
    case WM_DESTROY :
        PostQuitMessage(0);
        return 0;
    case WM_SIZE :
        DrawScreen(resizeClearColor);
    }

    
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

// Direct3D 초기화 함수
bool InitDirect3D(HWND hWnd) {
    // 1. Device, Context, SwapChain 생성
    DXGI_SWAP_CHAIN_DESC swapDesc = {};
    swapDesc.BufferCount = 1;
    swapDesc.BufferDesc.Width = g_width;
    swapDesc.BufferDesc.Height = g_height;
    swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapDesc.OutputWindow = hWnd;
    swapDesc.SampleDesc.Count = 1;
    swapDesc.SampleDesc.Quality = 0;
    swapDesc.Windowed = TRUE;
    swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    D3D_FEATURE_LEVEL featureLevel;
    if (FAILED(D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0,
        D3D11_SDK_VERSION, &swapDesc, &g_swapChain, &g_device, &featureLevel, &g_context))) {
        return false;
    }

    // 2. 백버퍼 -> RenderTargetView 생성
    ID3D11Texture2D* backBuffer = nullptr;
    g_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
    g_device->CreateRenderTargetView(backBuffer, nullptr, &g_renderTargetView);
    backBuffer->Release();

    // 3. DepthStencilBuffer + View 생성
    D3D11_TEXTURE2D_DESC depthDesc = {};
    depthDesc.Width = g_width;
    depthDesc.Height = g_height;
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    g_device->CreateTexture2D(&depthDesc, nullptr, &g_depthStencilBuffer);
    g_device->CreateDepthStencilView(g_depthStencilBuffer, nullptr, &g_depthStencilView);

    // 4. OM(출력 병합기)에 바인딩
    g_context->OMSetRenderTargets(1, &g_renderTargetView, g_depthStencilView);

    // 5. Viewport 설정
    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = static_cast<float>(g_width);
    viewport.Height = static_cast<float>(g_height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    g_context->RSSetViewports(1, &viewport);

    return true;
}

// 윈도우 생성 및 메시지 루프
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    // 1. 윈도우 클래스 등록
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"MyD3DWindow";
    RegisterClass(&wc);

    // 2. 윈도우 생성
    g_hWnd = CreateWindowEx(0, wc.lpszClassName, L"D3D11 Window", WS_OVERLAPPEDWINDOW,
        g_wPosX, g_wPosY, g_width, g_height, nullptr, nullptr, hInstance, nullptr);
    ShowWindow(g_hWnd, nCmdShow);

    // 3. Direct3D 초기화
    if (!InitDirect3D(g_hWnd)) {
        MessageBox(nullptr, L"Direct3D 초기화 실패", L"Error", MB_OK);
        return 1;
    }

    // 4. 메시지 루프
    MSG msg = {};
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            DrawScreen(defaultClearColor);
        }
    }

    // 리소스 해제
    if (g_context) g_context->ClearState();
    if (g_depthStencilView) g_depthStencilView->Release();
    if (g_depthStencilBuffer) g_depthStencilBuffer->Release();
    if (g_renderTargetView) g_renderTargetView->Release();
    if (g_swapChain) g_swapChain->Release();
    if (g_context) g_context->Release();
    if (g_device) g_device->Release();

    return 0;
}

void DrawScreen(float clearColor[4])
{
    if (!g_context) return;

    g_context->ClearRenderTargetView(g_renderTargetView, clearColor);
    g_context->ClearDepthStencilView(g_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    // Draw
    g_swapChain->Present(1, 0);
}
