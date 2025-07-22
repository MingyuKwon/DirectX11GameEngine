#include <KMGDataStructure.h>
#include <TextureLoader.h>
#include <iostream>
#include <UseAssimp.h>
#include <QueueCommand.h>

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

        swap(pVertexBuffer, resource.pVertexBuffer);
        swap(pIndexBuffer, resource.pIndexBuffer);
        swap(pCBChangesEveryFrame, resource.pCBChangesEveryFrame);
        swap(pTextureSRV, resource.pTextureSRV);
        swap(pNormalMapSRV, resource.pNormalMapSRV);

    }

    return *this;
}

void DrawResource::UpdateBuffers(std::vector<KMGVertex> vertices, std::vector<int> indices)
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
}

void DrawResource::UpdateActorCB(ID3D11DeviceContext* pMainContext, XMMATRIX WorldMatrix)
{
    if (this->WorldMatrix == WorldMatrix) return;


    this->WorldMatrix = WorldMatrix;

    CBChangeOnActor cb = {};
    cb.mWorld = XMMatrixTranspose(WorldMatrix);

    pMainContext->UpdateSubresource(pCBChangesEveryFrame, 0, nullptr, &cb, 0, 0);

}

void MeshLoader::PushMeshRequest(std::wstring actorName, std::string fileName)
{
    std::lock_guard<std::mutex> lock(meshRequestLock);
    meshRequest[actorName] = fileName;

}

void MeshLoader::MakeMeshOnRequest()
{
    std::lock_guard<std::mutex> lock(meshRequestLock);

    for (auto& bucket : meshRequest)
    {
        std::wstring actorName = bucket.first;
        std::string fileName = bucket.second;


        std::vector<KMGStaticMesh> meshes = LoadModelToActor(fileName);

  
        KMGCommand::UpdateStaticMesh(actorName, fileName, std::move(meshes));
    }
    
    meshRequest.clear();
}




//////////////////////////////////////////////////////////////
/// 아래 코드들은 직접 만들지 않고 외부에서 가져온 코드입니다
//////////////////////////////////////////////////////////////

KMGStaticMesh KMGStaticMesh::CreateDefaultSphereMesh(float radius, DirectX::XMFLOAT4 lightColor) {
    KMGStaticMesh mesh;
    mesh.vertices.clear();
    mesh.indices.clear();

    radius /= 2;

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
            vertex.Color = lightColor;

            mesh.vertices.push_back(vertex);
        }
    }

    for (int y = 0; y < RINGS; ++y) {
        for (int x = 0; x < SEGMENTS; ++x) {
            int i0 = y * (SEGMENTS + 1) + x;
            int i1 = i0 + SEGMENTS + 1;

            mesh.indices.push_back(i0);
            mesh.indices.push_back(i0 + 1);
            mesh.indices.push_back(i1);

            mesh.indices.push_back(i0 + 1);
            mesh.indices.push_back(i1 + 1);
            mesh.indices.push_back(i1);
        }
    }

    return mesh;
}

KMGStaticMesh KMGStaticMesh::CreateDefaultBoxPlaneMesh(float width, float depth, float thickness, DirectX::XMFLOAT4 color)
{
    KMGStaticMesh mesh;
    mesh.vertices.clear();
    mesh.indices.clear();

    float hw = width / 2.0f;
    float hd = depth / 2.0f;
    float hh = thickness / 2.0f;

    struct Face {
        XMFLOAT3 normal;
        XMFLOAT3 v0, v1, v2, v3;
    };

    std::vector<Face> faces = {
        // Bottom (-Y)
        {{0,-1,0}, {-hw,-hh,-hd}, {-hw,-hh, hd}, { hw,-hh, hd}, { hw,-hh,-hd}},
        // Top (+Y)
        {{0, 1,0}, {-hw, hh,-hd}, {-hw, hh, hd}, { hw, hh, hd}, { hw, hh,-hd}},
        // Front (+Z)
        {{0, 0,1}, {-hw,-hh, hd}, {-hw, hh, hd}, { hw, hh, hd}, { hw,-hh, hd}},
        // Back (-Z)
        {{0, 0,-1}, {-hw,-hh,-hd}, {-hw, hh,-hd}, { hw, hh,-hd}, { hw,-hh,-hd}},
        // Left (-X)
        {{-1,0,0}, {-hw,-hh,-hd}, {-hw, hh,-hd}, {-hw, hh, hd}, {-hw,-hh, hd}},
        // Right (+X)
        {{1, 0,0}, { hw,-hh,-hd}, { hw, hh,-hd}, { hw, hh, hd}, { hw,-hh, hd}},
    };

    int index = 0;
    for (const auto& face : faces) {
        // 각 면마다 4개 정점 추가
        KMGVertex v[4];
        for (int i = 0; i < 4; ++i) {
            v[i].Color = color;
            v[i].Normal = face.normal;
            v[i].Tex = {
                (i == 0 || i == 3) ? 0.0f : 1.0f,
                (i == 0 || i == 1) ? 0.0f : 1.0f
            };
        }

        v[0].Pos = face.v0;
        v[1].Pos = face.v1;
        v[2].Pos = face.v2;
        v[3].Pos = face.v3;

        mesh.vertices.push_back(v[0]);
        mesh.vertices.push_back(v[1]);
        mesh.vertices.push_back(v[2]);
        mesh.vertices.push_back(v[3]);

        // 두 개의 삼각형 인덱스 추가
        mesh.indices.push_back(index + 0);
        mesh.indices.push_back(index + 1);
        mesh.indices.push_back(index + 2);

        mesh.indices.push_back(index + 0);
        mesh.indices.push_back(index + 2);
        mesh.indices.push_back(index + 3);

        index += 4;
    }

    return mesh;
}
