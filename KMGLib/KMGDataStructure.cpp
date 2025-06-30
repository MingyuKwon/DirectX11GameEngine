#include <KMGDataStructure.h>
#include <iostream>

using namespace std;
using namespace DirectX;

inline bool operator==(const XMMATRIX& lhs, const XMMATRIX& rhs)
{
    const float epsilon = 1e-5f;

    for (int i = 0; i < 4; ++i)
    {
        if (!XMVector4NearEqual(lhs.r[i], rhs.r[i], XMVectorReplicate(epsilon)))
            return false;
    }

    return true;
}

inline bool operator!=(const XMMATRIX& lhs, const XMMATRIX& rhs)
{
    return !(lhs == rhs);
}

DrawResource::DrawResource(std::wstring name) : name(name)
{
}

DrawResource::~DrawResource()
{
    if (pVertexBuffer)
    {
        pVertexBuffer->Release();
        pVertexBuffer = nullptr;
    }

    if (pIndexBuffer)
    {
        pIndexBuffer->Release();
        pIndexBuffer = nullptr;
    }

    if (pCBChangesEveryFrame)
    {
        pCBChangesEveryFrame->Release();
        pCBChangesEveryFrame = nullptr;
    }
}

DrawResource::DrawResource(DrawResource&& resource) noexcept
{
    name = resource.name;
    swap(pVertexBuffer, resource.pVertexBuffer);
    swap(pIndexBuffer, resource.pIndexBuffer);
    swap(pCBChangesEveryFrame, resource.pCBChangesEveryFrame);
}

DrawResource& DrawResource::operator=(DrawResource&& resource) noexcept
{
    if (this != &resource)
    {
        if (pVertexBuffer)
        {
            pVertexBuffer->Release();
            pVertexBuffer = nullptr;
        }

        if (pIndexBuffer)
        {
            pIndexBuffer->Release();
            pIndexBuffer = nullptr;
        }

        if (pCBChangesEveryFrame)
        {
            pCBChangesEveryFrame->Release();
            pCBChangesEveryFrame = nullptr;
        }

        swap(pVertexBuffer, resource.pVertexBuffer);
        swap(pIndexBuffer, resource.pIndexBuffer);
        swap(pCBChangesEveryFrame, resource.pCBChangesEveryFrame);

    }

    return *this;
}

void DrawResource::UpdateBuffers(std::vector<KMGVertex> vertices, std::vector<int> indices)
{
    if (!g_pMainDevice) return;

    if (pVertexBuffer)
    {
        pVertexBuffer->Release();
        pVertexBuffer = nullptr;
    }

    if (pIndexBuffer)
    {
        pIndexBuffer->Release();
        pIndexBuffer = nullptr;
    }

    ////////////////////////////////////////
    // 버텍스 버퍼 생성
    ////////////////////////////////////////
    D3D11_BUFFER_DESC vbd = {};
    vbd.Usage = D3D11_USAGE_DEFAULT;
    vbd.ByteWidth = sizeof(KMGVertex) * (UINT)vertices.size();
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vinitData = {};
    vinitData.pSysMem = vertices.data();

    g_pMainDevice->CreateBuffer(&vbd, &vinitData, &pVertexBuffer);

    ////////////////////////////////////////
    // 인덱스 버퍼 생성
    ////////////////////////////////////////
    D3D11_BUFFER_DESC ibd = {};
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.ByteWidth = sizeof(UINT) * (UINT)indices.size();
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA iinitData = {};
    iinitData.pSysMem = indices.data();

    g_pMainDevice->CreateBuffer(&ibd, &iinitData, &pIndexBuffer);

    indexCount = indices.size();


    ////////////////////////////////////////
    // 매 프레임 바뀌는 버퍼 생성
    ////////////////////////////////////////
    D3D11_BUFFER_DESC cbd = {};
    cbd.Usage = D3D11_USAGE_DEFAULT;
    cbd.ByteWidth = sizeof(CBChangeOnActor);
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.CPUAccessFlags = 0;

    g_pMainDevice->CreateBuffer(&cbd, nullptr, &pCBChangesEveryFrame);
}

void DrawResource::UpdateWorldMatrix(ID3D11DeviceContext* pMainContext, XMMATRIX WorldMatrix)
{
    if (this->WorldMatrix == WorldMatrix) return;

    this->WorldMatrix = WorldMatrix;

    CBChangeOnActor cb = {};
    cb.mWorld = XMMatrixTranspose(WorldMatrix);

    pMainContext->UpdateSubresource(pCBChangesEveryFrame, 0, nullptr, &cb, 0, 0);

}

