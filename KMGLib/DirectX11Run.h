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
    XMMATRIX worldMatrix = XMMatrixIdentity();

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

void RenderThread(
    vector<ID3D11CommandList*>& DX11CommandLists, mutex& dx11CommandMutex,
    ID3D11Device* pMainDevice,
    ID3D11VertexShader* pVertexShader,
    ID3D11PixelShader* pPixelShader,
    ID3D11InputLayout* pVertexLayout,
    ID3D11RenderTargetView* pRTV, ID3D11DepthStencilView* pDSV,
    ID3D11Buffer* pCBChangeOnResize, ID3D11Buffer* pCBChangesEveryFrame,
    ID3D11Buffer* pVertexBuffer, ID3D11Buffer* pIdexBuffer,
    int drawIndexCount,
    const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix,
    int textureWidth, int textureHeight);