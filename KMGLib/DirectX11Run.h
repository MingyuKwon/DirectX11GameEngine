//--------------------------------------------------------------------------------------
// 이 코드의 기본 골자는 Microsoft에서 제공하는 DirectX11 Tutorial에서 따왔습니다.
// https://github.com/microsoft/DirectX-SDK-Samples/tree/main/C%2B%2B/Direct3D11/Tutorials
//--------------------------------------------------------------------------------------

#pragma once
#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxcolors.h>
#include "DDSTextureLoader.h"
#include <atomic>
#include <mutex>

#include <KMGActor.h>

using namespace DirectX;
using namespace std;

//--------------------------------------------------------------------------------------
// GPU에 넘겨줄 구조체
//--------------------------------------------------------------------------------------


struct CBNeverChanges
{
    XMMATRIX mView;
};

struct CBChangeOnResize
{
    XMMATRIX mProjection;
};

struct CBChangesEveryFrame
{
    XMMATRIX mWorld;
};

//--------------------------------------------------------------------------------------
// 그려야 하는 모델들을 저장하는 구조체
//--------------------------------------------------------------------------------------
struct DrawResource
{
    vector<KMGVertex> vertices;
    vector<int> indices;

    ID3D11Device* device = nullptr;
    ID3D11Buffer* vertexBuffer = nullptr;
    ID3D11Buffer* indexBuffer = nullptr;

    DrawResource() = default;
    DrawResource(ID3D11Device* device, vector<KMGVertex> vertices, vector<int> indices);
    virtual ~DrawResource();

    DrawResource(const DrawResource&) = delete;
    DrawResource& operator=(const DrawResource&) = delete;

    DrawResource(DrawResource&&) noexcept;
    DrawResource& operator=(DrawResource&&) noexcept;

    void CreateBuffers();

};

class D3D11Machine
{
public:
    D3D11Machine(HWND hWnd);
    virtual ~D3D11Machine();

    //////////////////////////////
    /// IMGUI를 위해서 필요한 부분
    /////////////////////////////
    HRESULT TryPresent(); // 메인 스레드에서 일어나야 함
    HRESULT SetDrawReady();
    void GetD3DDeviceContext(ID3D11Device** outDevice, ID3D11DeviceContext** outContext);

    void SetScreenSize(int width, int height);
    void ResizeScreen();
    void ClearScreen();

    //////////////////////////////
    /// + 외부에서 명령을 줄 때 필요한 부분
    /////////////////////////////
    HRESULT AddActor(const KMGActor& actor);
    HRESULT deleteActor(wstring name);


private:
    //////////////////////////////
    /// IMGUI를 위해서 필요한 부분
    /////////////////////////////
    ID3D11Device* pd3dDevice = nullptr;
    ID3D11DeviceContext* pd3dDeviceContext = nullptr;
    IDXGISwapChain* pSwapChain = nullptr;
    atomic<UINT> screenWidth = 0, screenHeight = 0;
    ID3D11RenderTargetView* pRenderTargetView = nullptr;

    bool CreateDeviceD3D(HWND hWnd);
    void CleanupRenderTarget();
    void CleanupDeviceD3D();
    void CreateRenderTarget();


    //////////////////////////////
    /// + 게임 씬을 그리기 위해서 필요한 부분
    /////////////////////////////
    ID3D11Buffer* pVertexBuffer = nullptr;
    ID3D11Buffer* pIndexBuffer = nullptr;
    ID3D11Buffer* pCBNeverChanges = nullptr;
    ID3D11Buffer* pCBChangeOnResize = nullptr;
    ID3D11Buffer* pCBChangesEveryFrame = nullptr;

    //////////////////////////////
    /// + 외부에서 명령을 줄 때 필요한 부분
    /////////////////////////////
    unordered_map<wstring, DrawResource> drawResources;
    atomic<bool> bCanDrawUI = false;

};

class DirectX11Wrapper
{
public:
    DirectX11Wrapper(HWND viewWindow, int width, int height);
    virtual ~DirectX11Wrapper();

    void SceneWindowRender(); // 씬에다가 백버퍼에 그림 그리기
    HRESULT TryPresent(); // 메인 스레드에서 일어나야 함
    HRESULT SetDrawReady();

    void GetD3DDeviceContext(ID3D11Device** outDevice, ID3D11DeviceContext** outContext);

protected:
    HRESULT InitDirectX11(int width, int height); // 기본적인 전역 변수 값 할당
    HRESULT Init_Device_Context(); // 가장 기본인 Device, context를 생성

    HRESULT Init_RTV_DSV_Viewport(int width, int height); // 렌더 타깃을 만들어주는 함수

    HRESULT CompileShader(const WCHAR* vertexShaderName, const WCHAR* pixelShaderName); 
    HRESULT CreateConstBuffers();

    void CleanupDevice(); // 전역 변수 값 전부 초기화

private:
    atomic<bool> bCanDrawUI = false;

    D3D_DRIVER_TYPE                     g_driverType = D3D_DRIVER_TYPE_NULL;
    D3D_FEATURE_LEVEL                   g_featureLevel = D3D_FEATURE_LEVEL_11_0;

    ID3D11Device* pd3dDevice = nullptr;
    ID3D11DeviceContext* pImmediateContext = nullptr;

    IDXGISwapChain* pSwapChain = nullptr;
    ID3D11RenderTargetView* pRenderTargetView = nullptr;

    ID3D11Texture2D* pDepthStencil = nullptr;
    ID3D11DepthStencilView* pDepthStencilView = nullptr;

    ID3D11VertexShader* pVertexShader = nullptr;
    ID3D11PixelShader* pPixelShader = nullptr;
    ID3D11InputLayout* pVertexLayout = nullptr;

    ID3D11Buffer* pVertexBuffer = nullptr;
    ID3D11Buffer* pIndexBuffer = nullptr;
    ID3D11Buffer* pCBNeverChanges = nullptr;
    ID3D11Buffer* pCBChangeOnResize = nullptr;
    ID3D11Buffer* pCBChangesEveryFrame = nullptr;

    ID3D11ShaderResourceView* pTextureRV = nullptr;
    ID3D11SamplerState* pSamplerLinear = nullptr;

    XMMATRIX World = XMMatrixIdentity();
    XMMATRIX View = XMMatrixIdentity();
    XMMATRIX Projection = XMMatrixIdentity();

    HWND mainWindow;
    HWND sceneWindow;

    unordered_map<wstring, DrawResource> drawResources;

};