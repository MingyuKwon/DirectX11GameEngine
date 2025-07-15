#pragma once
#include <directxmath.h>
#include <vector>
#include <string>
#include <queue>
#include <unordered_map>
#include <unordered_set>

#include <atomic>
#include <mutex>
#include <memory>
#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxcolors.h>
#include <iostream>
#include <EngineData.h>

using namespace DirectX;

extern ID3D11Device* g_pMainDevice;

enum class ESceneMode
{
    ESM_NONE,

    ESM_EDIT,
    ESM_PLAY,

    ESM_MAX,
};

enum class ESceneEditMode
{
    ESEM_NONE,

    ESEM_SELECT,
    ESEM_MOVE,
    ESEM_ROTATE,
    ESEM_SCALE,

    ESEM_MAX,
};

enum class EHoverMode
{
    EHM_NONE,

    EHM_X,
    EHM_Y,
    EHM_Z,

    EHM_MAX,
};

struct KMGVertex
{
    DirectX::XMFLOAT3 Pos;
    DirectX::XMFLOAT3 Normal;
    DirectX::XMFLOAT4 Color = DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
    DirectX::XMFLOAT2 Tex;
    DirectX::XMFLOAT3 Tangent;
    DirectX::XMFLOAT3 Binormal;
};

struct KMGStaticMesh {

    static KMGStaticMesh CreateDefaultSphereMesh(float radius = 1.0f, DirectX::XMFLOAT4 lightColor = DirectX::XMFLOAT4(1,1,0,1));

    std::vector<KMGVertex> vertices;
    std::vector<int> indices;
    std::wstring textureFilePath = DEFAULT_TEXTURE_FILEPATH;
    std::wstring normalMapFilePath = DEFAULT_NORMAL_FILEPATH;

    bool bShouldMeshChange = true;
};

struct KMGDebugMesh {

    std::vector<KMGVertex> vertices;
    std::vector<int> indices;

    bool bShouldDebugChange = true;

};

struct alignas(16) Light
{
    int type = 1; // 0 = directional, 1 = point, 2 = spot
    float range = 100;
    float intensity = 1;
    float lightPad_1;

    DirectX::XMFLOAT3 position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
    float lightPad_2;

    DirectX::XMFLOAT3 direction = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
    float lightPad_3;

    DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(1.0f, 1.0f, 0.9f, 1.0f);

    void setToDefault()
    {
        position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
        direction = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
        range = 100;
        intensity = 1;
        color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    }
};


struct alignas(16) CBLightArray
{
    Light lights[MAX_LIGHTS];
    int lightCount = 0;
    float padding[3] = { 0, 0, 0 };

    void clear()
    {
        lightCount = 0;
        for (int i=0; i< MAX_LIGHTS; i++)
        {
            lights[i].setToDefault();
        }
    }

    bool AddLight(Light light)
    {
        if (lightCount == MAX_LIGHTS) return false;

        lights[lightCount] = light;
        ++lightCount;

        return true;

    }
};

//--------------------------------------------------------------------------------------
// 액터의 Transform을 저장하는 구조체
//--------------------------------------------------------------------------------------
struct KMGTransform {
    
    DirectX::XMMATRIX GetWorldMatrix() const {
        
        XMMATRIX S = XMMatrixScalingFromVector(scale);
        XMMATRIX R = XMMatrixRotationQuaternion(rotation_Quaternion);
        XMMATRIX T = XMMatrixTranslationFromVector(position);

        return S * R * T;
    }

    inline DirectX::XMVECTOR GetForwardVector()
    {
        DirectX::XMVECTOR forwardVector = DirectX::XMVectorSet(0, 0, 1, 0);

        DirectX::XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(rotation_Quaternion);

        return DirectX::XMVector3TransformNormal(forwardVector, rotationMatrix);
    }

    DirectX::XMVECTOR position = DirectX::XMVectorSet(0, 0, 0, 1);
    DirectX::XMVECTOR rotation_Quaternion = DirectX::XMQuaternionIdentity();
    DirectX::XMVECTOR rotation_EulerCache = DirectX::XMVectorSet(0, 0, 0, 0);
    bool bEulerCacheDirty = true; // 쿼터니언이 바뀌면 true


    DirectX::XMVECTOR scale = DirectX::XMVectorSet(1, 1, 1, 0);
};

struct alignas(16) CBChangeOnResize
{
    DirectX::XMMATRIX mProjection;
};

struct alignas(16) CBChangeOnPlayer
{
    DirectX::XMMATRIX mView;
};

struct alignas(16) CBChangeOnActor
{
    DirectX::XMMATRIX mWorld;
};

struct MeshLoader
{
    void PushMeshRequest(std::wstring actorName, std::string fileName);
   
    void MakeMeshOnRequest();

    // 요청하는 액터 이름, 메시 이름
    std::unordered_map<std::wstring, std::string> meshRequest;
    std::mutex meshRequestLock;

};

//--------------------------------------------------------------------------------------
// 그려야 하는 모델들을 저장하는 구조체
//--------------------------------------------------------------------------------------
struct DrawResource
{
    bool bVisible = true;

    bool bInitialized = false;
    bool bDebug = false;
    bool bLightEffected = true;

    std::wstring name;

    ID3D11Buffer* pVertexBuffer = nullptr;
    ID3D11Buffer* pIndexBuffer = nullptr;
    ID3D11Buffer* pCBChangesEveryFrame = nullptr;

    ID3D11ShaderResourceView* pTextureSRV = nullptr;
    ID3D11ShaderResourceView* pNormalMapSRV = nullptr;

    int indexCount = 0;

    DrawResource();
    DrawResource(std::wstring name);
    virtual ~DrawResource();

    DrawResource(const DrawResource&) = delete;
    DrawResource& operator=(const DrawResource&) = delete;

    DrawResource(DrawResource&&) noexcept;
    DrawResource& operator=(DrawResource&&) noexcept;

    void UpdateBuffers(std::vector<KMGVertex> vertices, std::vector<int> indices);
    void UpdateActorCB(ID3D11DeviceContext* pMainContext, DirectX::XMMATRIX WorldMatrix);

private:
    DirectX::XMMATRIX WorldMatrix = DirectX::XMMATRIX(
        0.f, 0.f, 0.f, 0.f,
        0.f, 0.f, 0.f, 0.f,
        0.f, 0.f, 0.f, 0.f,
        0.f, 0.f, 0.f, 0.f
    );
};