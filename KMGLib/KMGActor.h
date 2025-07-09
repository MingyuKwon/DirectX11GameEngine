#pragma once
#include <KMGDataStructure.h>
#include <KMGComponent.h>
#include <iostream>
#include <KMGUtility.h>


class KMGActor {

public:
    KMGActor(int ID, std::wstring name);
    virtual ~KMGActor();

    std::atomic<bool> bShowBoundBox = false;

    inline std::wstring GetName()
    {
        return name;
    }

    inline int GetActorID()
    {
        return actorID;
    }

    inline void SetName(std::wstring inName)
    {
        name = inName;
    }

    DirectX::XMMATRIX getWorldMatrix();

    inline DirectX::XMVECTOR GetPosition() const { return transform.position; }
    void SetPosition(float x, float y, float z);
    void SetPosition(DirectX::XMVECTOR position);

    inline DirectX::XMVECTOR GetRotation() const { return transform.rotation; }
    void SetRotation(float pitch, float yaw, float roll);
    void SetRotation(DirectX::XMVECTOR rotation);

    inline DirectX::XMVECTOR GetScale() const { return transform.scale; }
    inline void SetScale(float x, float y, float z) { transform.scale = DirectX::XMVectorSet(x, y, z, 0); }

    void Translate(float dx, float dy, float dz);    // 상대이동
    void Rotate(float dpitch, float dyaw, float droll); // 상대회전

    inline bool HasComponent(EComponentType type)
    {
        return components.count(type) != 0;
    }

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

    bool SetComponent(KMGComponent* addComponent);

    inline bool HasMesh() {
        return components.count(EComponentType::ECT_STATICMESH) > 0;
    }

    // ray와 만나는 경우, 가장 짧은 거리를 반환하고, 못맞나면 -1을 반한합니다
    float RayTraceHit(DirectX::XMVECTOR rayOrigin, DirectX::XMVECTOR rayDir);

    void UpdateTexture(std::wstring beforeTextureName, std::wstring textureName);
    void UpdateNormalMap(std::wstring beforeTextureName, std::wstring textureName);


private:
    std::unordered_map<EComponentType, std::unique_ptr<KMGComponent>> components;

    std::wstring name;
    KMGTransform transform;

    int actorID = 0;

};