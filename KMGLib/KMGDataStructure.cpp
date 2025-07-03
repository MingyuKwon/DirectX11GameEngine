#include <KMGDataStructure.h>
#include <TextureLoader.h>
#include <iostream>

using namespace std;
using namespace DirectX;

KMGStaticMesh KMGStaticMesh::CreateDefaultSphereMesh(float radius) {
    KMGStaticMesh mesh;
    mesh.vertices.clear();
    mesh.indices.clear();

    const int SEGMENTS = 16;
    const int RINGS = 8;
    const float PI = 3.14159265359f;


    for (int y = 0; y <= RINGS; ++y) {
        float v = (float)y / RINGS;
        float theta = v * PI;  // latitude (0 to pi)

        for (int x = 0; x <= SEGMENTS; ++x) {
            float u = (float)x / SEGMENTS;
            float phi = u * 2.0f * PI;  // longitude (0 to 2pi)

            float sinTheta = sinf(theta);
            float cosTheta = cosf(theta);
            float sinPhi = sinf(phi);
            float cosPhi = cosf(phi);

            float px = radius * sinTheta * cosPhi;
            float py = radius * cosTheta;
            float pz = radius * sinTheta * sinPhi;

            KMGVertex vertex;
            vertex.Pos = { px, py, pz };
            vertex.Normal = { px / radius, py / radius, pz / radius }; // normalize
            vertex.Tex = { u, 1.0f - v }; // Flip V for DirectX

            mesh.vertices.push_back(vertex);
        }
    }

    for (int y = 0; y < RINGS; ++y) {
        for (int x = 0; x < SEGMENTS; ++x) {
            int i0 = y * (SEGMENTS + 1) + x;
            int i1 = i0 + SEGMENTS + 1;

            mesh.indices.push_back(i0);
            mesh.indices.push_back(i1);
            mesh.indices.push_back(i0 + 1);

            mesh.indices.push_back(i0 + 1);
            mesh.indices.push_back(i1);
            mesh.indices.push_back(i1 + 1);
        }
    }

    return mesh;
}

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

DrawResource::DrawResource() : name(L"Default")
{

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

    if (pTextureSRV)
    {
        pTextureSRV->Release();
        pTextureSRV = nullptr;

    }

    if (pNormalMapSRV)
    {
        pNormalMapSRV->Release();
        pNormalMapSRV = nullptr;
    }
}

DrawResource::DrawResource(DrawResource&& resource) noexcept
{
    name = resource.name;
    swap(pVertexBuffer, resource.pVertexBuffer);
    swap(pIndexBuffer, resource.pIndexBuffer);
    swap(pCBChangesEveryFrame, resource.pCBChangesEveryFrame);
    swap(pTextureSRV, resource.pTextureSRV);
    swap(pNormalMapSRV, resource.pNormalMapSRV);

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

        if (pTextureSRV)
        {
            pTextureSRV->Release();
            pTextureSRV = nullptr;
        }

        if (pNormalMapSRV)
        {
            pNormalMapSRV->Release();
            pNormalMapSRV = nullptr;
        }

        swap(pVertexBuffer, resource.pVertexBuffer);
        swap(pIndexBuffer, resource.pIndexBuffer);
        swap(pCBChangesEveryFrame, resource.pCBChangesEveryFrame);
        swap(pTextureSRV, resource.pTextureSRV);
        swap(pNormalMapSRV, resource.pNormalMapSRV);

    }

    return *this;
}

void DrawResource::UpdateBuffers(std::vector<KMGVertex> vertices, std::vector<int> indices, std::wstring textureFilePath, std::wstring normalMapFilePath)
{
    if (!g_pMainDevice) return;

    if (!bInitialized)
    {
        bInitialized = true;

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
    // 주어진 경로에서 텍스처 가져와서 만들고 srv까지 만듦
    ////////////////////////////////////////
    if (this->textureFilePath != textureFilePath)
    {
        this->textureFilePath = textureFilePath;

        if (pTextureSRV)
        {
            pTextureSRV->Release();
            pTextureSRV = nullptr;
        }

        HRESULT hr = CreateSrvFromTexture(g_pMainDevice, this->textureFilePath.c_str(), &pTextureSRV);
        if (FAILED(hr))
        {
            wcout << "this->textureFilePath : " << this->textureFilePath << "  Failed \n";
            this->textureFilePath = L"";
        }
        else
        {
            wcout << "this->textureFilePath : " << this->textureFilePath << "  Success \n" ;
        }
    }

    ////////////////////////////////////////
    // 주어진 경로에서 NormalMap 가져와서 만들고 srv까지 만듦
    ////////////////////////////////////////
    if (this->normalMapFilePath != normalMapFilePath)
    {
        this->normalMapFilePath = normalMapFilePath;

        if (pNormalMapSRV)
        {
            pNormalMapSRV->Release();
            pNormalMapSRV = nullptr;
        }

        HRESULT hr = CreateSrvFromTexture(g_pMainDevice, this->normalMapFilePath.c_str(), &pNormalMapSRV);
        if (FAILED(hr))
        {
            wcout << "this->normalMapFilePath : " << this->normalMapFilePath << "  Failed \n";
            this->normalMapFilePath = L"";
        }
        else
        {
            wcout << "this->normalMapFilePath : " << this->normalMapFilePath << "  Success \n";
        }
    }

}

void DrawResource::UpdateWorldMatrix(ID3D11DeviceContext* pMainContext, XMMATRIX WorldMatrix)
{
    if (this->WorldMatrix == WorldMatrix) return;


    this->WorldMatrix = WorldMatrix;

    CBChangeOnActor cb = {};
    cb.mWorld = XMMatrixTranspose(WorldMatrix);

    pMainContext->UpdateSubresource(pCBChangesEveryFrame, 0, nullptr, &cb, 0, 0);

}

