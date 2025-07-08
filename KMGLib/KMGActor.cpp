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

void KMGActor::SetRotation(DirectX::XMVECTOR rotation)
{
    float x, y, z;
    x = XMVectorGetX(rotation);
    y = XMVectorGetY(rotation);
    z = XMVectorGetZ(rotation);

    SetRotation(x, y, z);

}

void KMGActor::SetRotation(float pitch, float yaw, float roll)
{
    pitch = KMGUtility::WrapAngleRad(pitch);
    yaw = KMGUtility::WrapAngleRad(yaw);
    roll = KMGUtility::WrapAngleRad(roll);

    transform.rotation = DirectX::XMVectorSet(pitch, yaw, roll, 0);

    LightComponent* lightComp = GetComponent<LightComponent>(EComponentType::ECT_LIGHT);
    if (lightComp)
    {
        Light& light = lightComp->GetLight();
        XMStoreFloat3(&light.direction, transform.GetForwardVector());
    }
}


void KMGActor::Rotate(float dpitch, float dyaw, float droll) {
    XMVECTOR rot = transform.rotation;
    XMVECTOR delta = XMVectorSet(dpitch, dyaw, droll, 0);

    SetRotation(XMVectorAdd(rot, delta));
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
        SetRotation(transform.rotation);

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
                        std::wcout << "UpdateTexture " << beforeTextureName << " " << textureName << " \n";

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
                        std::wcout << "UpdateNormalMap " << beforeTextureName << " " << textureName << " \n";

                        mesh.normalMapFilePath = textureName;
                        mesh.bShouldMeshChange = true;
                    }
                }
            }
        }
    }
}
