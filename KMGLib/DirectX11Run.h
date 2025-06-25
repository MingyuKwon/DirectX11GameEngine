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


struct CBChangeOnResize
{
    XMMATRIX mProjection;
};

struct CBChangesEveryFrame
{
    XMMATRIX mView;
    XMMATRIX mWorld;
};

//--------------------------------------------------------------------------------------
// 그려야 하는 모델들을 저장하는 구조체
//--------------------------------------------------------------------------------------
struct DrawResource
{
    vector<KMGVertex> vertices;
    vector<int> indices;
    XMMATRIX WorldMatrix = XMMatrixIdentity();

    ID3D11Device* device = nullptr;
    ID3D11Buffer* vertexBuffer = nullptr;
    ID3D11Buffer* indexBuffer = nullptr;

    DrawResource() = default;
    DrawResource(ID3D11Device* device, vector<KMGVertex> vertices, vector<int> indices, XMMATRIX WorldMatrix);
    virtual ~DrawResource();

    DrawResource(const DrawResource&) = delete;
    DrawResource& operator=(const DrawResource&) = delete;

    DrawResource(DrawResource&&) noexcept;
    DrawResource& operator=(DrawResource&&) noexcept;

    void CreateBuffers();

};

class DeferredRenderThread
{
public:
    DeferredRenderThread(ID3D11Device* pd3dDevice);
    virtual ~DeferredRenderThread();

    //////////////////////////////
    /// IMGUI를 위해서 필요한 부분
    /////////////////////////////
    void DrawTexture();
    bool Initialize();

    void GetSRVTexture(ID3D11ShaderResourceView** outSRV);

    void SetScreenSize(int width, int height);
    void ResizeScreen();
    void ClearScreen();

    void SceneWindowRender();
    //////////////////////////////
    /// + 외부에서 명령을 줄 때 필요한 부분
    /////////////////////////////


private:
    //////////////////////////////
    /// IMGUI를 위해서 필요한 부분
    /////////////////////////////
    ID3D11Device* pd3dDevice = nullptr;
    ID3D11DeviceContext* pd3dDeviceContext = nullptr;
    atomic<UINT> screenWidth = 0, screenHeight = 0;

    ID3D11RenderTargetView* pRenderTargetView = nullptr;

    void CleanupRenderTarget();
    void CleanupDeviceD3D();
    void CreateRenderTarget();


    //////////////////////////////
    /// + 게임 씬을 그리기 위해서 필요한 부분
    /////////////////////////////
    ID3D11Buffer* pCBChangeOnResize = nullptr;
    ID3D11Buffer* pCBChangesEveryFrame = nullptr;

    ID3D11Texture2D* pDepthStencil = nullptr;
    ID3D11DepthStencilView* pDepthStencilView = nullptr;

    ID3D11VertexShader* pVertexShader = nullptr;
    ID3D11PixelShader* pPixelShader = nullptr;
    ID3D11InputLayout* pVertexLayout = nullptr;

    XMMATRIX View = XMMatrixIdentity();
    XMMATRIX Projection = XMMatrixIdentity();

    ID3D11RenderTargetView* pSceneRTV = nullptr;
    ID3D11ShaderResourceView* pSceneSRV = nullptr;

    HRESULT CreateConstBuffers();


    //////////////////////////////
    /// + 외부에서 명령을 줄 때 필요한 부분
    /////////////////////////////
    DrawResource* pDrawResource;
};
