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

/// <summary>
/// 이거 기준이 쿼터니언이다
/// 내부적으로는 쿼터니언을 계산을 전부 해야 한다
/// </summary>
/// <param name="rotation"></param>
void KMGActor::SetRotation(DirectX::XMVECTOR rotation)
{
    transform.rotation_Quaternion = XMQuaternionNormalize(rotation);

    LightComponent* lightComp = GetComponent<LightComponent>(EComponentType::ECT_LIGHT);
    if (lightComp)
    {
        Light& light = lightComp->GetLight();
        XMStoreFloat3(&light.direction, transform.GetForwardVector());
    }
}

/// <summary>
/// 이거 기준이 쿼터니언이다
/// 내부적으로는 쿼터니언을 계산을 전부 해야 한다
/// </summary>
/// <param name="rotation"></param>
void KMGActor::Rotate(DirectX::XMVECTOR worldAxis, float radian) {

    XMFLOAT3 coutFloat;
    XMStoreFloat3(&coutFloat, worldAxis);
    //std::cout << "WorldRotate : " << coutFloat.x << " " << coutFloat.y << " " << coutFloat.z << " \n";


    XMMATRIX deltaRotMat = XMMatrixRotationAxis(worldAxis, radian);
    XMMATRIX currentRotMat = XMMatrixRotationQuaternion(transform.rotation_Quaternion);

    // 월드 기준 회전 먼저, 현재 회전 나중
    XMMATRIX resultMat = deltaRotMat * currentRotMat;

    XMVECTOR resultQuat = XMQuaternionRotationMatrix(resultMat);


    // 기준 축 벡터들
    XMVECTOR right = XMVectorSet(1, 0, 0, 0);   // 로컬 X축
    XMVECTOR up = XMVectorSet(0, 1, 0, 0);   // 로컬 Y축
    XMVECTOR forward = XMVectorSet(0, 0, 1, 0); // 로컬 Z축

    // 회전 행렬 적용
    XMVECTOR worldRight = XMVector3TransformNormal(right, resultMat);
    XMVECTOR worldUp = XMVector3TransformNormal(up, resultMat);
    XMVECTOR worldForward = XMVector3TransformNormal(forward, resultMat);

    // 출력
    XMFLOAT3 outVec;

    XMStoreFloat3(&outVec, worldRight);
    std::cout << "[결과] Right  → (" << outVec.x << ", " << outVec.y << ", " << outVec.z << ")\n";

    XMStoreFloat3(&outVec, worldUp);
    std::cout << "[결과] Up     → (" << outVec.x << ", " << outVec.y << ", " << outVec.z << ")\n";

    XMStoreFloat3(&outVec, worldForward);
    std::cout << "[결과] Forward→ (" << outVec.x << ", " << outVec.y << ", " << outVec.z << ")\n";


    SetRotation(resultQuat);
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
        SetRotation(GetRotation());

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
