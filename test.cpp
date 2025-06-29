// draw_cube_main.cpp - Renders a red cube using DirectX11 with a Win32 window

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <d3dcompiler.h>

#include <windows.h>
#pragma comment(lib, "d3d11.lib")
using namespace DirectX;

HRESULT CompileShader(const WCHAR* vertexShaderName, const WCHAR* pixelShaderName);
HRESULT CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);

// Structures
struct KMGVertex {
    XMFLOAT3 Pos;
    XMFLOAT3 Normal;
    XMFLOAT4 Color;
    XMFLOAT2 Tex;
};

struct CBChangeOnActor {
    XMMATRIX View;
    XMMATRIX World;
};

struct CBChangeOnResize {
    XMMATRIX Projection;
};

// Globals
HWND gHwnd = nullptr;
ID3D11Device* gDevice = nullptr;
ID3D11DeviceContext* gContext = nullptr;
IDXGISwapChain* gSwapChain = nullptr;
ID3D11RenderTargetView* gRTV = nullptr;
ID3D11DepthStencilView* gDSV = nullptr;
ID3D11Buffer* gVB = nullptr, * gIB = nullptr, * gCBFrame = nullptr, * gCBResize = nullptr;
ID3D11InputLayout* gInputLayout = nullptr;
ID3D11VertexShader* gVS = nullptr;
ID3D11PixelShader* gPS = nullptr;

std::vector<KMGVertex> vertices;
std::vector<int> indices;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void CreateCubeData();
void InitD3D(HWND hwnd);
void CleanD3D();
void DrawScene();

HWND CreateAppWindow(HINSTANCE hInstance, int width, int height) {
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"CubeWindowClass";
    RegisterClass(&wc);

    RECT rc = { 0, 0, width, height };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    HWND hwnd = CreateWindow(wc.lpszClassName, L"Red Cube - DirectX11", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top,
        nullptr, nullptr, hInstance, nullptr);
    return hwnd;
}

int main() {
    HINSTANCE hInstance = GetModuleHandle(nullptr);
    HWND hwnd = CreateAppWindow(hInstance, 1280, 720);
    gHwnd = hwnd;
    ShowWindow(hwnd, SW_SHOW);

    InitD3D(hwnd);
    CreateCubeData();

    MSG msg = {};
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            DrawScene();
        }
    }

    CleanD3D();
    return 0;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void CreateCubeData() {
    vertices = {
        { {-1,1,-1},{},{1,0,0,1},{1,0} }, { {1,1,-1},{},{1,0,0,1},{0,0} },
        { {1,1,1},{},{1,0,0,1},{0,1} }, { {-1,1,1},{},{1,0,0,1},{1,1} },
        { {-1,-1,-1},{},{1,0,0,1},{0,0} }, { {1,-1,-1},{},{1,0,0,1},{1,0} },
        { {1,-1,1},{},{1,0,0,1},{1,1} }, { {-1,-1,1},{},{1,0,0,1},{0,1} },
        { {-1,-1,1},{},{1,0,0,1},{0,1} }, { {-1,-1,-1},{},{1,0,0,1},{1,1} },
        { {-1,1,-1},{},{1,0,0,1},{1,0} }, { {-1,1,1},{},{1,0,0,1},{0,0} },
        { {1,-1,1},{},{1,0,0,1},{1,1} }, { {1,-1,-1},{},{1,0,0,1},{0,1} },
        { {1,1,-1},{},{1,0,0,1},{0,0} }, { {1,1,1},{},{1,0,0,1},{1,0} },
        { {-1,-1,-1},{},{1,0,0,1},{0,1} }, { {1,-1,-1},{},{1,0,0,1},{1,1} },
        { {1,1,-1},{},{1,0,0,1},{1,0} }, { {-1,1,-1},{},{1,0,0,1},{0,0} },
        { {-1,-1,1},{},{1,0,0,1},{1,1} }, { {1,-1,1},{},{1,0,0,1},{0,1} },
        { {1,1,1},{},{1,0,0,1},{0,0} }, { {-1,1,1},{},{1,0,0,1},{1,0} },
    };
    indices = {
        3,1,0, 2,1,3, 6,4,5, 7,4,6, 11,9,8, 10,9,11,
        14,12,13, 15,12,14, 19,17,16, 18,17,19, 22,20,21, 23,20,22
    };

    D3D11_BUFFER_DESC vbDesc = { sizeof(KMGVertex) * (UINT)vertices.size(), D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER };
    D3D11_SUBRESOURCE_DATA vbData = { vertices.data() };
    gDevice->CreateBuffer(&vbDesc, &vbData, &gVB);

    D3D11_BUFFER_DESC ibDesc = { sizeof(int) * (UINT)indices.size(), D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER };
    D3D11_SUBRESOURCE_DATA ibData = { indices.data() };
    gDevice->CreateBuffer(&ibDesc, &ibData, &gIB);
}

void InitD3D(HWND hwnd) {
    DXGI_SWAP_CHAIN_DESC scd = {};
    scd.BufferCount = 1;
    scd.BufferDesc.Width = 1280;
    scd.BufferDesc.Height = 720;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = hwnd;
    scd.SampleDesc.Count = 1;
    scd.Windowed = TRUE;

    D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0,
        D3D11_SDK_VERSION, &scd, &gSwapChain, &gDevice, nullptr, &gContext);

    ID3D11Texture2D* backBuffer = nullptr;
    gSwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
    gDevice->CreateRenderTargetView(backBuffer, nullptr, &gRTV);
    backBuffer->Release();

    D3D11_TEXTURE2D_DESC depthDesc = {};
    depthDesc.Width = 1280;
    depthDesc.Height = 720;
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    ID3D11Texture2D* depthBuffer = nullptr;
    gDevice->CreateTexture2D(&depthDesc, nullptr, &depthBuffer);
    gDevice->CreateDepthStencilView(depthBuffer, nullptr, &gDSV);
    depthBuffer->Release();

    // Create constant buffers
    D3D11_BUFFER_DESC cbDesc = { sizeof(CBChangeOnActor), D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER };
    gDevice->CreateBuffer(&cbDesc, nullptr, &gCBFrame);
    cbDesc.ByteWidth = sizeof(CBChangeOnResize);
    gDevice->CreateBuffer(&cbDesc, nullptr, &gCBResize);

    CompileShader(L"KMGLib\\VertexShader.hlsli", L"KMGLib\\PixelShader.hlsli");
}

void DrawScene() {
    XMVECTOR Eye = XMVectorSet(0, 2, -6, 0);
    XMVECTOR At = XMVectorSet(0, 0, 0, 0);
    XMVECTOR Up = XMVectorSet(0, 1, 0, 0);
    XMMATRIX View = XMMatrixLookAtLH(Eye, At, Up);
    XMMATRIX World = XMMatrixIdentity();
    XMMATRIX Projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, 1280.0f / 720.0f, 0.1f, 100.0f);

    CBChangeOnActor cbFrame = { XMMatrixTranspose(View), XMMatrixTranspose(World) };
    gContext->UpdateSubresource(gCBFrame, 0, nullptr, &cbFrame, 0, 0);
    CBChangeOnResize cbResize = { XMMatrixTranspose(Projection) };
    gContext->UpdateSubresource(gCBResize, 0, nullptr, &cbResize, 0, 0);

    float clearColor[4] = { 0, 0, 1, 1 };
    gContext->ClearRenderTargetView(gRTV, clearColor);
    gContext->ClearDepthStencilView(gDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
    gContext->OMSetRenderTargets(1, &gRTV, gDSV);

    D3D11_VIEWPORT vp = { 0, 0, 1280, 720, 0.0f, 1.0f };
    gContext->RSSetViewports(1, &vp);

    UINT stride = sizeof(KMGVertex);
    UINT offset = 0;
    gContext->IASetVertexBuffers(0, 1, &gVB, &stride, &offset);
    gContext->IASetIndexBuffer(gIB, DXGI_FORMAT_R32_UINT, 0);
    gContext->IASetInputLayout(gInputLayout);
    gContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    gContext->VSSetShader(gVS, nullptr, 0);
    gContext->VSSetConstantBuffers(0, 1, &gCBFrame);
    gContext->VSSetConstantBuffers(1, 1, &gCBResize);
    gContext->PSSetShader(gPS, nullptr, 0);

    gContext->DrawIndexed((UINT)indices.size(), 0, 0);
    gSwapChain->Present(1, 0);
}

void CleanD3D() {
    if (gVB) gVB->Release();
    if (gIB) gIB->Release();
    if (gCBFrame) gCBFrame->Release();
    if (gCBResize) gCBResize->Release();
    if (gInputLayout) gInputLayout->Release();
    if (gVS) gVS->Release();
    if (gPS) gPS->Release();
    if (gRTV) gRTV->Release();
    if (gDSV) gDSV->Release();
    if (gSwapChain) gSwapChain->Release();
    if (gContext) gContext->Release();
    if (gDevice) gDevice->Release();
}

// Tutorial에서 가져온 코드
//--------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DCompile
//
// With VS 11, we could load up prebuilt .cso files instead...
//--------------------------------------------------------------------------------------

HRESULT CompileShader(const WCHAR* vertexShaderName, const WCHAR* pixelShaderName)
{
    ID3DBlob* pVSBlob = nullptr;
    HRESULT hr = CompileShaderFromFile(vertexShaderName, "VS", "vs_4_0", &pVSBlob);
    if (FAILED(hr))
    {
        MessageBox(nullptr,
            L"The Vertex Shader file cannot be compiled.  Please run this executable from the directory that contains the Shader file.", L"Error", MB_OK);
        return hr;
    }

    hr = gDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &gVS);
    if (FAILED(hr))
    {
        pVSBlob->Release();
        return hr;
    }

    const D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = ARRAYSIZE(layout);

    hr = gDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
        pVSBlob->GetBufferSize(), &gInputLayout);
    pVSBlob->Release();
    if (FAILED(hr)) return hr;

    ID3DBlob* pPSBlob = nullptr;
    hr = CompileShaderFromFile(pixelShaderName, "PS", "ps_4_0", &pPSBlob);
    if (FAILED(hr))
    {
        MessageBox(nullptr,
            L"The Pixel Shader file cannot be compiled.  Please run this executable from the directory that contains the Shader file.", L"Error", MB_OK);
        return hr;
    }

    hr = gDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &gPS);
    pPSBlob->Release();
    if (FAILED(hr)) return hr;

    return hr;
}