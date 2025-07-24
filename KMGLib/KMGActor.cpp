#include <KMGActor.h>
#include <iostream>

using namespace DirectX;

KMGActor::KMGActor(int ID, std::wstring name) : actorID(ID), name(name)
{
    StaticMeshComponent* staticComponent = new StaticMeshComponent();
    SetComponent(staticComponent);

    RigidBodyComponent* rigidBodyComponent = new RigidBodyComponent();
    SetComponent(rigidBodyComponent);

}

KMGActor::~KMGActor()
{
}

void KMGActor::CopyActorToTarget(KMGActor* targetActor)
{
    // 자 여기서 기존 액터가 가지던거 싹 다 복사 해야 한다

    // 이름, ActorID말고 싹다 복사하자

    targetActor->bVisible = bVisible;
    targetActor->readTransform = readTransform;

    {
        std::lock_guard<std::mutex> lock(transformWriteLock);
        targetActor->writeTransform = writeTransform;

        targetActor->AABBBox = AABBBox;
    }

    for (auto& bucket : components)
    {
        EComponentType compType = bucket.first;
        KMGComponent* pComp = bucket.second.get();

        switch (compType)
        {
        case EComponentType::ECT_LIGHT:
            targetActor->components[compType] =  std::move(std::make_unique<LightComponent>(*dynamic_cast<LightComponent*>(pComp)));
            break;
        case EComponentType::ECT_STATICMESH:
            targetActor->components[compType] = std::move(std::make_unique<StaticMeshComponent>(*dynamic_cast<StaticMeshComponent*>(pComp)));
            break;
        case EComponentType::ECT_RIGIDBODY:
            targetActor->components[compType] = std::move(std::make_unique<RigidBodyComponent>(*dynamic_cast<RigidBodyComponent*>(pComp)));
            break;

        default:
            continue;
        }

        targetActor->components[compType]->SetOwner(targetActor);
    }
}


DirectX::XMMATRIX KMGActor::getReadWorldMatrix()
{
    return readTransform.GetWorldMatrix();
}

DirectX::XMMATRIX KMGActor::getWriteWorldMatrix()
{
    return writeTransform.GetWorldMatrix();
}

void KMGActor::SetPosition(float x, float y, float z)
{
    {
        std::lock_guard<std::mutex> lock(transformWriteLock);
        writeTransform.position = DirectX::XMVectorSet(x, y, z, 1);
    }

    LightComponent* lightComp = GetComponent<LightComponent>(EComponentType::ECT_LIGHT);
    if (lightComp)
    {
        Light& light = lightComp->GetLight();
        XMStoreFloat3(&light.position, readTransform.position);
    }
}

void KMGActor::SetPosition(DirectX::XMVECTOR position)
{
    float x, y, z;
    x = XMVectorGetX(position);
    y = XMVectorGetY(position);
    z = XMVectorGetZ(position);

    SetPosition(x,y,z);
}

void KMGActor::Translate(float dx, float dy, float dz) {

    XMVECTOR pos;
    XMVECTOR delta;

    {
        std::lock_guard<std::mutex> lock(transformWriteLock);
        pos = writeTransform.position;
        delta = XMVectorSet(dx, dy, dz, 0);
    }

    SetPosition(XMVectorAdd(pos, delta));

    LightComponent* lightComp = GetComponent<LightComponent>(EComponentType::ECT_LIGHT);
    if (lightComp)
    {
        Light& light = lightComp->GetLight();
        XMStoreFloat3(&light.position, readTransform.position);
    }
}

DirectX::XMVECTOR KMGActor::GetRotation_E()
{
    if (readTransform.bEulerCacheDirty) {
        readTransform.rotation_EulerCache = KMGUtility::QuaternionToClosestEulerXYZ(readTransform.rotation_Quaternion, readTransform.rotation_EulerCache);
        readTransform.bEulerCacheDirty = false;
    }
    return readTransform.rotation_EulerCache;
}

void KMGActor::SetRotation_E(XMVECTOR eulerRadianXYZ)
{
    {
        std::lock_guard<std::mutex> lock(transformWriteLock);
        writeTransform.rotation_Quaternion = KMGUtility::EulerXYZToQuaternion(eulerRadianXYZ);
        writeTransform.rotation_EulerCache = eulerRadianXYZ;
        writeTransform.bEulerCacheDirty = false;
    }

    LightComponent* lightComp = 
        GetComponent<LightComponent>(EComponentType::ECT_LIGHT);
    if (lightComp)
    {
        Light& light = lightComp->GetLight();
        XMStoreFloat3(&light.direction, -readTransform.GetForwardVector());
    }

}

/// <summary>
/// 이거 기준이 쿼터니언이다
/// 내부적으로는 쿼터니언을 계산을 전부 해야 한다
/// </summary>
/// <param name="rotation"></param>
void KMGActor::SetRotation_Q(DirectX::XMVECTOR rotation)
{
    {
        std::lock_guard<std::mutex> lock(transformWriteLock);
        writeTransform.rotation_Quaternion = XMQuaternionNormalize(rotation);
        writeTransform.bEulerCacheDirty = true;
    }


    LightComponent* lightComp = GetComponent<LightComponent>(EComponentType::ECT_LIGHT);
    if (lightComp)
    {
        Light& light = lightComp->GetLight();
        XMStoreFloat3(&light.direction, -readTransform.GetForwardVector());
    }
}

void KMGActor::SetScale(float x, float y, float z)
{
    std::lock_guard<std::mutex> lock(transformWriteLock);
    writeTransform.scale = DirectX::XMVectorSet(x, y, z, 0);
}

/// <summary>
/// 이거 기준이 쿼터니언이다
/// 내부적으로는 쿼터니언을 계산을 전부 해야 한다
/// </summary>
/// <param name="rotation"></param>
void KMGActor::Rotate(DirectX::XMVECTOR worldAxis, float radian) {

    XMMATRIX deltaRotMat = XMMatrixRotationAxis(worldAxis, radian);
    XMMATRIX currentRotMat = XMMatrixRotationQuaternion(writeTransform.rotation_Quaternion);

    // 월드 기준 회전 먼저, 현재 회전 나중
    XMMATRIX resultMat = deltaRotMat * currentRotMat;

    XMVECTOR resultQuat = XMQuaternionRotationMatrix(resultMat);

    XMVECTOR right = XMVectorSet(1, 0, 0, 0);   
    XMVECTOR up = XMVectorSet(0, 1, 0, 0);  
    XMVECTOR forward = XMVectorSet(0, 0, 1, 0); 

    XMVECTOR worldRight = XMVector3TransformNormal(right, resultMat);
    XMVECTOR worldUp = XMVector3TransformNormal(up, resultMat);
    XMVECTOR worldForward = XMVector3TransformNormal(forward, resultMat);

    SetRotation_Q(resultQuat);
}


bool KMGActor::SetComponent(KMGComponent* addComponent)
{
    if (addComponent == nullptr) return false;

    if (components.count(addComponent->componentType) > 0)
    {
        std::cout << "There is already Component\n";
        return false;
    }

    addComponent->SetOwner(this);
    components[addComponent->componentType] = std::unique_ptr<KMGComponent>(addComponent);


    StaticMeshComponent* staticComp = dynamic_cast<StaticMeshComponent*>(addComponent);
    if (staticComp)
    {
        KMGStaticMesh newDefaultMesh = KMGStaticMesh::CreateDefaultSphereMesh(0.5f, XMFLOAT4(1, 0, 0, 1));
        staticComp->SetMeshData(std::move(newDefaultMesh));

        LightComponent* local_lightComp = GetComponent<LightComponent>(EComponentType::ECT_LIGHT);
        if (local_lightComp)
        {
            local_lightComp->SetLightColor(local_lightComp->GetLight().color);
        }

    }


    RigidBodyComponent* rigidBodyComponent = dynamic_cast<RigidBodyComponent*>(addComponent);
    if (rigidBodyComponent)
    {

    }

    LightComponent* lightComp = dynamic_cast<LightComponent*>(addComponent);
    if (lightComp)
    {
        Light& light = lightComp->GetLight();
        SetRotation_Q(GetRotation_Q());

    }

    return true;
}

float KMGActor::RayTraceHit(DirectX::XMVECTOR rayOrigin, DirectX::XMVECTOR rayDir)
{
    if (HasMesh())
    {
        XMMATRIX invWorld = XMMatrixInverse(nullptr, getReadWorldMatrix());

        XMVECTOR localRayOrigin = XMVector3Transform(rayOrigin, invWorld);
        XMVECTOR localRayDir = XMVector3TransformNormal(rayDir, invWorld);
        localRayDir = XMVector3Normalize(localRayDir);

        StaticMeshComponent* meshComp = GetComponent<StaticMeshComponent>(EComponentType::ECT_STATICMESH);
        if (meshComp)
        {
            XMVECTOR hitPosLocal;
            float hit = meshComp->CheckHitWithRay(localRayOrigin, localRayDir, hitPosLocal);
            if (hit >= 0)
            {
                XMVECTOR hitPosWorld = XMVector3Transform(hitPosLocal, getReadWorldMatrix());

                float distance = XMVectorGetX(XMVector3Length(hitPosWorld - rayOrigin));
                return distance;
            }
        }
    }

    return -1;
}

void KMGActor::UpdateTexture(std::wstring beforeTextureName, std::wstring textureName)
{
    if (HasMesh())
    {
        StaticMeshComponent* staticMeshComp = GetComponent<StaticMeshComponent>(EComponentType::ECT_STATICMESH);
        if (staticMeshComp)
        {
            std::vector<KMGStaticMesh>* meshes = staticMeshComp->GetMeshes();
            if (meshes)
            {
                for (KMGStaticMesh& mesh : *meshes)
                {
                    if (mesh.textureFilePath == beforeTextureName)
                    {
                        mesh.textureFilePath = textureName;
                        mesh.bShouldMeshChange = true;
                    }
                }
            }
        }
    }
}

void KMGActor::UpdateNormalMap(std::wstring beforeTextureName, std::wstring textureName)
{
    if (HasMesh())
    {
        StaticMeshComponent* staticMeshComp = GetComponent<StaticMeshComponent>(EComponentType::ECT_STATICMESH);
        if (staticMeshComp)
        {
            std::vector<KMGStaticMesh>* meshes = staticMeshComp->GetMeshes();
            if (meshes)
            {
                for (KMGStaticMesh& mesh : *meshes)
                {
                    if (mesh.normalMapFilePath == beforeTextureName)
                    {
                        mesh.normalMapFilePath = textureName;
                        mesh.bShouldMeshChange = true;
                    }
                }
            }
        }
    }
}

void KMGActor::AddForceToActor(DirectX::XMVECTOR force)
{
    auto lamd = [force, this]()
        {
            RigidBodyComponent* rigidBodyComponent = GetComponent<RigidBodyComponent>(EComponentType::ECT_RIGIDBODY);
            if (rigidBodyComponent)
            {
                rigidBodyComponent->AddForce(force);

                std::cout << " rigidBodyComponent->AddForce " << "\n";

            }
        };

    EnQueueKineticCommand(std::move(lamd));
}

void KMGActor::IntegrateVelocity(float deltaTime)
{
    RigidBodyComponent* rigidBodyComponent = GetComponent<RigidBodyComponent>(EComponentType::ECT_RIGIDBODY);
    if (rigidBodyComponent)
    {
        rigidBodyComponent->IntegrateVelocity(deltaTime);
    }
}

void KMGActor::PredictPosition(float deltaTime)
{
    RigidBodyComponent* rigidBodyComponent = GetComponent<RigidBodyComponent>(EComponentType::ECT_RIGIDBODY);
    if (rigidBodyComponent)
    {
        rigidBodyComponent->PredictPosition(deltaTime);
    }
}

void KMGActor::ApplyFinalPosition()
{
    RigidBodyComponent* rigidBodyComponent = GetComponent<RigidBodyComponent>(EComponentType::ECT_RIGIDBODY);
    if (rigidBodyComponent)
    {
        rigidBodyComponent->ApplyFinalPosition();
    }

}

void KMGActor::EnQueueKineticCommand(std::function<void()>&& command)
{
    std::lock_guard<std::mutex> lock(kineticBufferLock);
    kineticCommandQueue_back.push(std::move(command));
}

void KMGActor::ExecuteAllKineticCommand()
{
    {
        // 여기서 백버퍼와 프런트 버퍼를 한번만 바꾸게 한다. 
        // 이렇게 안하고 그냥 버퍼 한개만 썼더니 한개를 잡겠다고 뮤텍스가 계속 막혀서 fps가 떨어진다
        std::lock_guard<std::mutex> lock(kineticBufferLock);
        std::swap(kineticCommandQueue_back, kineticCommandQueue_front);
    }

    while (!kineticCommandQueue_front.empty())
    {
        kineticCommandQueue_front.front()();
        kineticCommandQueue_front.pop();
    }

}
