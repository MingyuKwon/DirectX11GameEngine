#pragma once
#include <KMGDataStructure.h>


class KMGActor {

public:
    KMGActor(std::wstring name);

    std::atomic<bool> bShouldDrawResourceChange = true;

    std::wstring GetName();
    DirectX::XMMATRIX getWorldMatrix();

    inline DirectX::XMVECTOR GetPosition() const { return transform.position; }
    inline void SetPosition(float x, float y, float z) { transform.position = DirectX::XMVectorSet(x, y, z, 1); }

    inline DirectX::XMVECTOR GetRotation() const { return transform.rotation; }
    inline void SetRotation(float pitch, float yaw, float roll) { transform.rotation = DirectX::XMVectorSet(pitch, yaw, roll, 0); }

    inline DirectX::XMVECTOR GetScale() const { return transform.scale; }
    inline void SetScale(float x, float y, float z) { transform.scale = DirectX::XMVectorSet(x, y, z, 0); }

    void Translate(float dx, float dy, float dz);    // 상대이동
    void Rotate(float dpitch, float dyaw, float droll); // 상대회전

    inline const std::vector<KMGMesh>& GetMeshes()
    {
        return meshes;
    }

    inline void SetMeshData(std::vector<KMGMesh>&& inMeshes)
    { 
        meshes = std::move(inMeshes);
        bShouldDrawResourceChange = true;
    }

private:
    std::wstring name;
    std::vector<KMGMesh> meshes;  
    KMGTransform transform;
};