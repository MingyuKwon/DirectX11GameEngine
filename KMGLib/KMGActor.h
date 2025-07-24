#pragma once
#include <KMGDataStructure.h>
#include <KMGComponent.h>
#include <iostream>
#include <KMGUtility.h>


class KMGActor {

public:
    friend RigidBodyComponent;

    KMGActor(int ID, std::wstring name);
    virtual ~KMGActor();

    std::atomic<bool> bShowBoundBox = false;

    void CopyActorToTarget(KMGActor* targetActor);

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

    inline void SetVisibility(bool bVisible)
    {
        this->bVisible = bVisible;

        if (this->bVisible)
        {
            StaticMeshComponent* staticComponent = GetComponent<StaticMeshComponent>(EComponentType::ECT_STATICMESH);
            std::vector<KMGStaticMesh>* meshes = staticComponent->GetMeshes();

            if (meshes)
            {
                for (KMGStaticMesh& mesh : *meshes)
                {
                    mesh.bShouldMeshChange = true;
                }
            }
        }
        
    }

    inline bool IsVisible()
    {
        return bVisible;
    }

    DirectX::XMMATRIX getReadWorldMatrix();
    DirectX::XMMATRIX getWriteWorldMatrix();

    inline DirectX::XMVECTOR GetPosition() const {
        return readTransform.position; }

    inline DirectX::XMVECTOR GetWriteBufferPosition() const {
        return writeTransform.position;
    }


    inline DirectX::XMVECTOR GetLocalAxis(DirectX::XMVECTOR defaultAxis) {
        defaultAxis = DirectX::XMVector3TransformNormal(defaultAxis, getReadWorldMatrix());
        defaultAxis = DirectX::XMVector3Normalize(defaultAxis);
        return defaultAxis;
    }

    inline DirectX::XMVECTOR GetRotation_Q() const { 
        return readTransform.rotation_Quaternion;
    }

    DirectX::XMVECTOR GetRotation_E();

    inline DirectX::XMVECTOR GetScale() const { return readTransform.scale; }
   
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

    inline DirectX::BoundingBox GetAABBBox()
    {
        std::lock_guard<std::mutex> lock(AABBLock);
        return AABBBox;
    }

    inline void UpdateAABBBox(DirectX::BoundingBox inBox)
    {
        std::lock_guard<std::mutex> lock(AABBLock);
        AABBBox = inBox;
    }

    void AddLocalForceToActor(DirectX::XMVECTOR force);
    void IntegrateVelocity(float deltaTime);
    void PredictPosition(float deltaTime);

    void ApplyFinalPosition();

    inline void SwapTransformBuffer() {
        std::lock_guard<std::mutex> lock(transformWriteLock);
        readTransform = writeTransform;
    }


private:
    std::unordered_map<EComponentType, std::unique_ptr<KMGComponent>> components;

    std::wstring name;
    KMGTransform readTransform;
    KMGTransform writeTransform;

    std::mutex transformWriteLock;

    DirectX::BoundingBox AABBBox;
    std::mutex AABBLock;

    std::queue<std::function<void()>> kineticCommandQueue;
    std::mutex kineticQueueLock;


    bool bVisible = true;

    int actorID = 0;


public:
    void ExecuteAllKineticCommand();

    /// <summary>
    /// Kinematic 등록 함수들
    /// </summary>

    void Translate_Kinematic(float dx, float dy, float dz)
    {
        auto lamd = [dx, dy, dz, this]()
            {
                Translate(dx, dy, dz);
            };

        EnQueueKineticCommand(std::move(lamd));

    }

    void Rotate_Kinematic(DirectX::XMVECTOR worldAxis, float radian) // 상대회전
    {
        auto lamd = [worldAxis, radian, this]()
            {
                Rotate(worldAxis, radian);
            };

        EnQueueKineticCommand(std::move(lamd));

    }

    void SetPosition_Kinematic(float x, float y, float z)
    {
        auto lamd = [x,y,z, this]()
            {
                SetPosition(x,y,z);
            };

        EnQueueKineticCommand(std::move(lamd));
    }
    void SetPosition_Kinematic(DirectX::XMVECTOR position)
    {
        auto lamd = [position, this]()
            {
                SetPosition(position);
            };

        EnQueueKineticCommand(std::move(lamd));

    }

    void SetRotation_E_Kinematic(XMVECTOR eulerRadianXYZ)
    {
        auto lamd = [eulerRadianXYZ, this]()
            {
                SetRotation_E(eulerRadianXYZ);
            };

        EnQueueKineticCommand(std::move(lamd));

    }

    void SetRotation_Q_Kinematic(DirectX::XMVECTOR rotation){
        auto lamd = [rotation, this]()
            {
                SetRotation_Q(rotation);
            };

        EnQueueKineticCommand(std::move(lamd));

    }

    void SetScale_Kinematic(float x, float y, float z) {
        auto lamd = [x,y,z, this]()
            {
                SetScale(x,y,z);
            };

        EnQueueKineticCommand(std::move(lamd));

    }

    /// <summary>
    /// Kinematic 등록 함수들
    /// </summary>

private:
    void EnQueueKineticCommand(std::function<void()>&& command);

    void Translate(float dx, float dy, float dz);    // 상대이동
    void Rotate(DirectX::XMVECTOR worldAxis, float radian); // 상대회전

    void SetPosition(float x, float y, float z);
    void SetPosition(DirectX::XMVECTOR position);

    void SetRotation_E(XMVECTOR eulerRadianXYZ);

    void SetRotation_Q(DirectX::XMVECTOR rotation);

    void SetScale(float x, float y, float z);
    

};