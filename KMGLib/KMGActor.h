#pragma once
#include <KMGDataStructure.h>
#include <KMGComponent.h>
#include <iostream>


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

    inline const std::vector<KMGStaticMesh>* GetMeshes()
    {
        if (meshes.size() == 0) return nullptr;

        return &meshes;
    }

    inline void SetMeshData(std::vector<KMGStaticMesh>&& inMeshes)
    { 
        meshes = std::move(inMeshes);
        bShouldDrawResourceChange = true;
    }

    inline KMGComponent* GetComponent(EComponentType type) {
        if (components.count(type) == 0) return nullptr;

        return components[type].get();
    }

    inline bool RemoveComponent(EComponentType type) {
        if (components.count(type) == 0) return false;
        components.erase(type);

        return true;
    }

    inline bool SetComponent(KMGComponent* addComponent) {

        if (addComponent == nullptr) return false;

        if (components.count(addComponent->componentType) > 0)
        {
            std::cout << "There is already Component\n";
            return false;
        }

        components[addComponent->componentType] = std::unique_ptr<KMGComponent>(addComponent) ;
        return true;

    }

private:
    std::unordered_map<EComponentType, std::unique_ptr<KMGComponent>> components;

    std::wstring name;
    std::vector<KMGStaticMesh> meshes;  
    KMGTransform transform;
};