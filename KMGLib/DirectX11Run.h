//--------------------------------------------------------------------------------------
// 이 코드의 기본 골자는 Microsoft에서 제공하는 DirectX11 Tutorial에서 따왔습니다.
// https://github.com/microsoft/DirectX-SDK-Samples/tree/main/C%2B%2B/Direct3D11/Tutorials
//--------------------------------------------------------------------------------------

#pragma once
#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "DDSTextureLoader.h"

#include <vector>

using namespace DirectX;
using namespace std;

//--------------------------------------------------------------------------------------
// GPU에 넘겨줄 구조체
//--------------------------------------------------------------------------------------
struct KMGVertex
{
    XMFLOAT3 Pos;
    XMFLOAT3 Normal;
    XMFLOAT4 Color;
    XMFLOAT2 Tex;
};

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

    ID3D11Device* device;
    ID3D11Buffer* vertexBuffer = nullptr;
    ID3D11Buffer* indexBuffer = nullptr;

    DrawResource(ID3D11Device* device, vector<KMGVertex> vertices, vector<int> indices);
    virtual ~DrawResource();

    DrawResource(const DrawResource&) = delete;
    DrawResource& operator=(const DrawResource&) = delete;

    void CreateBuffers();

};


//--------------------------------------------------------------------------------------
// 렌더링에 사용할 전역 변수
//--------------------------------------------------------------------------------------
class DirectX11Wrapper
{
public:
    DirectX11Wrapper();
    virtual ~DirectX11Wrapper();

    void ResizeViewtarget(int width, int height); // 렌더링할 부분이 바뀌면 호출할 함수
    void Render(); // 그림 그리기

protected:
    HRESULT InitDirectX11(); // 기본적인 전역 변수 값 할당
    HRESULT Init_Device_Context(); // 가장 기본인 Device, context를 생성
    HRESULT Init_RTV_DSV_Viewport(int width, int height); // 가장 기본인 Device, context를 생성
    HRESULT CompileShader(const WCHAR* vertexShaderName, const WCHAR* pixelShaderName); 

    void CleanupDevice(); // 전역 변수 값 전부 초기화

private:
    D3D_DRIVER_TYPE                     g_driverType = D3D_DRIVER_TYPE_NULL;
    D3D_FEATURE_LEVEL                   g_featureLevel = D3D_FEATURE_LEVEL_11_0;

    ID3D11Device* g_pd3dDevice = nullptr;
    ID3D11DeviceContext* g_pImmediateContext = nullptr;

    IDXGISwapChain* g_pSwapChain = nullptr;
    ID3D11RenderTargetView* g_pRenderTargetView = nullptr;

    ID3D11Texture2D* g_pDepthStencil = nullptr;
    ID3D11DepthStencilView* g_pDepthStencilView = nullptr;

    ID3D11VertexShader* g_pVertexShader = nullptr;
    ID3D11PixelShader* g_pPixelShader = nullptr;
    ID3D11InputLayout* g_pVertexLayout = nullptr;

    ID3D11Buffer* g_pVertexBuffer = nullptr;
    ID3D11Buffer* g_pIndexBuffer = nullptr;
    ID3D11Buffer* g_pCBNeverChanges = nullptr;
    ID3D11Buffer* g_pCBChangeOnResize = nullptr;
    ID3D11Buffer* g_pCBChangesEveryFrame = nullptr;

    ID3D11ShaderResourceView* g_pTextureRV = nullptr;
    ID3D11SamplerState* g_pSamplerLinear = nullptr;

    XMMATRIX g_World = XMMatrixIdentity();
    XMMATRIX g_View = XMMatrixIdentity();
    XMMATRIX g_Projection = XMMatrixIdentity();

    vector<DrawResource> drawResources;

};