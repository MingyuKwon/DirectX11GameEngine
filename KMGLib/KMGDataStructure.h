#pragma once
#include <directxmath.h>
#include <vector>
#include <string>
#include <queue>
#include <unordered_map>
#include <atomic>
#include <mutex>
#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxcolors.h>

#include <EngineData.h>

extern ID3D11Device* g_pMainDevice;

struct KMGVertex
{
    DirectX::XMFLOAT3 Pos;
    DirectX::XMFLOAT3 Normal;
    DirectX::XMFLOAT4 Color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    DirectX::XMFLOAT2 Tex;
};

struct KMGMesh {
    std::vector<KMGVertex> vertices;
    std::vector<int> indices;
    std::wstring textureFilePath = DEFAULT_TEXTURE_FILEPATH;
};

//--------------------------------------------------------------------------------------
// 액터의 Transform을 저장하는 구조체
//--------------------------------------------------------------------------------------
struct KMGTransform {
    
    DirectX::XMMATRIX GetWorldMatrix() const {
        
        using namespace DirectX;
        XMMATRIX S = XMMatrixScalingFromVector(scale);
        XMMATRIX R = XMMatrixRotationRollPitchYawFromVector(rotation);
        XMMATRIX T = XMMatrixTranslationFromVector(position);

        return S * R * T;
    }

    DirectX::XMVECTOR position = DirectX::XMVectorSet(0, 0, 0, 1);
    DirectX::XMVECTOR rotation = DirectX::XMVectorSet(0, 0, 0, 0); // 이건 진짜 roatatrion이 아니라 yaw, pitch, roll 저장용의 벡터이다
    DirectX::XMVECTOR scale = DirectX::XMVectorSet(1, 1, 1, 0);
};

struct CBChangeOnResize
{
    DirectX::XMMATRIX mProjection;
};

struct CBChangeOnPlayer
{
    DirectX::XMMATRIX mView;
};

struct CBChangeOnActor
{
    DirectX::XMMATRIX mWorld;
};

//--------------------------------------------------------------------------------------
// 그려야 하는 모델들을 저장하는 구조체
//--------------------------------------------------------------------------------------
struct DrawResource
{
    bool bInitialized = false;

    std::wstring name;
    std::wstring textureFilePath;

    ID3D11Buffer* pVertexBuffer = nullptr;
    ID3D11Buffer* pIndexBuffer = nullptr;
    ID3D11Buffer* pCBChangesEveryFrame = nullptr;

    ID3D11ShaderResourceView* pTextureSRV = nullptr;

    int indexCount = 0;


    DrawResource(std::wstring name);
    virtual ~DrawResource();

    DrawResource(const DrawResource&) = delete;
    DrawResource& operator=(const DrawResource&) = delete;

    DrawResource(DrawResource&&) noexcept;
    DrawResource& operator=(DrawResource&&) noexcept;

    void UpdateBuffers(std::vector<KMGVertex> vertices, std::vector<int> indices, std::wstring textureFilePath);
    void UpdateWorldMatrix(ID3D11DeviceContext* pMainContext, DirectX::XMMATRIX WorldMatrix);

private:
    DirectX::XMMATRIX WorldMatrix = DirectX::XMMATRIX(
        0.f, 0.f, 0.f, 0.f,
        0.f, 0.f, 0.f, 0.f,
        0.f, 0.f, 0.f, 0.f,
        0.f, 0.f, 0.f, 0.f
    );
};