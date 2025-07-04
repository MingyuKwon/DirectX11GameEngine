#pragma once
#include <KMGDataStructure.h>
#include <KMGComponent.h>
#include <iostream>


class KMGActor {

public:
    KMGActor(std::wstring name);
    virtual ~KMGActor();

    std::atomic<bool> bShouldDrawResourceChange = true;

    std::wstring GetName();
    DirectX::XMMATRIX getWorldMatrix();

    inline DirectX::XMVECTOR GetPosition() const { return transform.position; }
    void SetPosition(float x, float y, float z);

    inline DirectX::XMVECTOR GetRotation() const { return transform.rotation; }
    inline void SetRotation(float pitch, float yaw, float roll) { transform.rotation = DirectX::XMVectorSet(pitch, yaw, roll, 0); }

    inline DirectX::XMVECTOR GetScale() const { return transform.scale; }
    inline void SetScale(float x, float y, float z) { transform.scale = DirectX::XMVectorSet(x, y, z, 0); }

    void Translate(float dx, float dy, float dz);    // 상대이동
    void Rotate(float dpitch, float dyaw, float droll); // 상대회전

    template <typename T>
    inline T* GetComponent(EComponentType type) {
        if (components.count(type) == 0) return nullptr;

        return dynamic_cast<T*>(components[type].get()); 
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

        LightComponent* lightComp = dynamic_cast<LightComponent*>(addComponent);
        if (lightComp)
        {
            Light& light = lightComp->GetLight();
            XMStoreFloat3(&light.position, transform.position);
        }

        addComponent->SetOwner(this);
        components[addComponent->componentType] = std::unique_ptr<KMGComponent>(addComponent);
        
        return true;

    }

private:
    std::unordered_map<EComponentType, std::unique_ptr<KMGComponent>> components;

    std::wstring name;
    KMGTransform transform;
};