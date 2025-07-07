#include <KMGActor.h>
#include <iostream>

using namespace DirectX;

KMGActor::KMGActor(std::wstring name) : name(name)
{
    StaticMeshComponent* staticComponent = new StaticMeshComponent();
    KMGStaticMesh newDefaultMesh = KMGStaticMesh::CreateDefaultSphereMesh(0.5f, XMFLOAT4(1,0,0,1));

    staticComponent->SetMeshData(std::move(newDefaultMesh));

    SetComponent(staticComponent);
}

KMGActor::~KMGActor()
{
}

std::wstring KMGActor::GetName()
{
    return name;
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

void KMGActor::Rotate(float dpitch, float dyaw, float droll) {
    XMVECTOR rot = transform.rotation;
    XMVECTOR delta = XMVectorSet(dpitch, dyaw, droll, 0);
    transform.rotation = XMVectorAdd(rot, delta);
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
            bool hit = meshComp->CheckHitWithRay(localRayOrigin, localRayDir, hitPosLocal);
            if (hit)
            {
                XMVECTOR hitPosWorld = XMVector3Transform(hitPosLocal, getWorldMatrix());

                float distance = XMVectorGetX(XMVector3Length(hitPosWorld - rayOrigin));
                return distance;
            }
        }
    }

    return -1;
}
