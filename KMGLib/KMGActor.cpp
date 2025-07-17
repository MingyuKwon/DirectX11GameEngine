#include <KMGActor.h>
#include <iostream>

using namespace DirectX;

KMGActor::KMGActor(int ID, std::wstring name) : actorID(ID), name(name)
{
    StaticMeshComponent* staticComponent = new StaticMeshComponent();
    KMGStaticMesh newDefaultMesh = KMGStaticMesh::CreateDefaultSphereMesh(0.5f, XMFLOAT4(1,0,0,1));

    staticComponent->SetMeshData(std::move(newDefaultMesh));

    SetComponent(staticComponent);
}

KMGActor::~KMGActor()
{
}

void KMGActor::CopyActorToTarget(KMGActor* targetActor)
{
    // 자 여기서 기존 액터가 가지던거 싹 다 복사 해야 한다

    // 이름, ActorID말고 싹다 복사하자

    targetActor->bVisible = bVisible;
    targetActor->transform = transform;

    for (auto& bucket : components)
    {
        EComponentType compType = bucket.first;
        KMGComponent* pComp = bucket.second.get();

        switch (compType)
        {
        case EComponentType::ECT_LIGHT:
            targetActor->components[compType] = std::make_unique<LightComponent>(*dynamic_cast<LightComponent*>(pComp));
            break;
        case EComponentType::ECT_STATICMESH:
            targetActor->components[compType] = std::make_unique<StaticMeshComponent>(*dynamic_cast<StaticMeshComponent*>(pComp));
            break;
        default:
            continue;
        }

        targetActor->components[compType]->SetOwner(targetActor);
    }
}


DirectX::XMMATRIX KMGActor::getWorldMatrix()
{
    return transform.GetWorldMatrix();
}

void KMGActor::SetPosition(float x, float y, float z)
{
    transform.position = DirectX::XMVectorSet(x, y, z, 1);

    LightComponent* lightComp = GetComponent<LightComponent>(EComponentType::ECT_LIGHT);
    if (lightComp)
    {
        Light& light = lightComp->GetLight();
        XMStoreFloat3(&light.position, transform.position);
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
    XMVECTOR pos = transform.position;
    XMVECTOR delta = XMVectorSet(dx, dy, dz, 0);
    transform.position = XMVectorAdd(pos, delta);

    LightComponent* lightComp = GetComponent<LightComponent>(EComponentType::ECT_LIGHT);
    if (lightComp)
    {
        Light& light = lightComp->GetLight();
        XMStoreFloat3(&light.position, transform.position);
    }
}

DirectX::XMVECTOR KMGActor::GetRotation_E()
{
    if (transform.bEulerCacheDirty) {
        transform.rotation_EulerCache = KMGUtility::QuaternionToEulerXYZ(transform.rotation_Quaternion);
        transform.bEulerCacheDirty = false;
    }
    return transform.rotation_EulerCache;
}

void KMGActor::SetRotation_E(XMVECTOR eulerRadianXYZ)
{
    transform.rotation_Quaternion = KMGUtility::EulerXYZToQuaternion(eulerRadianXYZ);
    transform.rotation_EulerCache = eulerRadianXYZ;
    transform.bEulerCacheDirty = false;

    LightComponent* lightComp = 
        GetComponent<LightComponent>(EComponentType::ECT_LIGHT);
    if (lightComp)
    {
        Light& light = lightComp->GetLight();
        XMStoreFloat3(&light.direction, -transform.GetForwardVector());
    }

}

/// <summary>
/// 이거 기준이 쿼터니언이다
/// 내부적으로는 쿼터니언을 계산을 전부 해야 한다
/// </summary>
/// <param name="rotation"></param>
void KMGActor::SetRotation_Q(DirectX::XMVECTOR rotation)
{
    transform.rotation_Quaternion = XMQuaternionNormalize(rotation);
    transform.bEulerCacheDirty = true;


    LightComponent* lightComp = GetComponent<LightComponent>(EComponentType::ECT_LIGHT);
    if (lightComp)
    {
        Light& light = lightComp->GetLight();
        XMStoreFloat3(&light.direction, -transform.GetForwardVector());
    }
}

/// <summary>
/// 이거 기준이 쿼터니언이다
/// 내부적으로는 쿼터니언을 계산을 전부 해야 한다
/// </summary>
/// <param name="rotation"></param>
void KMGActor::Rotate(DirectX::XMVECTOR worldAxis, float radian) {

    XMMATRIX deltaRotMat = XMMatrixRotationAxis(worldAxis, radian);
    XMMATRIX currentRotMat = XMMatrixRotationQuaternion(transform.rotation_Quaternion);

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

    LightComponent* lightComp = dynamic_cast<LightComponent*>(addComponent);
    if (lightComp)
    {
        Light& light = lightComp->GetLight();
        SetPosition(transform.position);
        SetRotation_Q(GetRotation_Q());

    }

    return true;
}

float KMGActor::RayTraceHit(DirectX::XMVECTOR rayOrigin, DirectX::XMVECTOR rayDir)
{
    if (HasMesh())
    {
        XMMATRIX invWorld = XMMatrixInverse(nullptr, getWorldMatrix());

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
                XMVECTOR hitPosWorld = XMVector3Transform(hitPosLocal, getWorldMatrix());

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
