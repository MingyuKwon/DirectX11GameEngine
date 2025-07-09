#include "KMGScene.h"
#include "KMGUtility.h"
#include "UseAssimp.h"

using namespace DirectX;

KMGScene::KMGScene()
{
    CreateAxis();
}

void KMGScene::CreateAxis()
{
    // 먼저 AxisActor 만들기
    axisActor = std::make_unique<KMGActor>(-1, L"System_Axis");
    axisActor->SetScale(0.1f, 0.1f, 0.1f);

    LoadModelToActor("D:\\DirectX11GameEngine\\Resource\\XYZ axis.obj", *axisActor, nullptr);
    
    StaticMeshComponent* staticComp = axisActor->GetComponent<StaticMeshComponent>(EComponentType::ECT_STATICMESH);

    DirectX::XMFLOAT4 axisColors[3] =
    {
        DirectX::XMFLOAT4(0, 0, 1, 1.0f),
        DirectX::XMFLOAT4(0, 1, 0, 1.0f),
        DirectX::XMFLOAT4(1, 0, 0, 1.0f),

    };

    if (staticComp)
    {
        std::vector<KMGStaticMesh>* meshes = staticComp->GetMeshes();
        for (int i=0; i<3; i++)
        {
            std::vector<KMGVertex>& axisMesh = (*meshes)[i+1].vertices;
            for (KMGVertex& vertex : axisMesh)
            {
                vertex.Color = axisColors[i];
            }
        }
    }


}

KMGActor* KMGScene::CreateActor(std::wstring name)
{
    std::lock_guard<std::mutex> lock(actorMapLock);

    if (actors.count(name) != 0) return nullptr;

    // 이 경우에는 그냥 이름 안주고 만들라고 하는거다
    if (DEFAULT_ACTOR_NAME == name)
    {
        name += std::to_wstring(ActorCreateCount);
    }

    auto actor = std::make_unique<KMGActor>(ActorCreateCount, name);
    KMGActor* ptr = actor.get();

    actors[name] = std::move(actor);
    actorNames.emplace(name);

    ActorCreateCount++;

    return ptr;
}

void KMGScene::EraseActor(const std::wstring& name)
{
    std::lock_guard<std::mutex> lock(actorMapLock);

    if (actors.count(name) == 0) return;

    if (focusActor && focusActor->GetName() == name)
    {
        focusActor = nullptr;
    }
    actors.erase(name);
    actorNames.erase(name);

}

void KMGScene::Tick(float deltaTime)
{
    std::vector<std::wstring> shouldErase;

    for (auto& bucket : debugLifeTime)
    {
        bucket.second -= deltaTime;
        if (bucket.second <= 0)
        {
            shouldErase.emplace_back(bucket.first);
        }
    }

    for (std::wstring name : shouldErase)
    {
        debugLifeTime.erase(name);
    }
}

KMGActor* KMGScene::GetActor(const std::wstring& name)
{
    std::lock_guard<std::mutex> lock(actorMapLock);

    if (actors.count(name) != 0) return actors[name].get();
    return nullptr;
}

void KMGScene::RenameActor(std::wstring beforeName, std::wstring aftername)
{
    std::lock_guard<std::mutex> lock(actorMapLock);

    if (actors.count(beforeName) == 0) return;
    if (actors.count(aftername) != 0) return;

    actors[aftername] = std::move(actors[beforeName]);
    actors.erase(beforeName);

    actorNames.emplace(aftername);
    actorNames.erase(beforeName);

    actors[aftername]->SetName(aftername);

}

const std::unordered_map<std::wstring, std::unique_ptr<KMGActor>>& KMGScene::getAllActors()
{
    std::lock_guard<std::mutex> lock(actorMapLock);

    return actors;
}

void KMGScene::CheckHoverAxis(DirectX::XMVECTOR rayDir)
{
    DirectX::XMVECTOR cameraPosition = currentCamera.GetCameraPosition();

    StaticMeshComponent* staicComp = axisActor->GetComponent<StaticMeshComponent>(EComponentType::ECT_STATICMESH);
    if (staicComp)
    {
        XMMATRIX invWorld = XMMatrixInverse(nullptr, axisActor->getWorldMatrix());

        XMVECTOR localRayOrigin = XMVector3Transform(cameraPosition, invWorld);
        XMVECTOR localRayDir = XMVector3TransformNormal(rayDir, invWorld);
        localRayDir = XMVector3Normalize(localRayDir);

        staicComp->AxisOnly_CheckHoverAxis(localRayOrigin, localRayDir, hoverMode);

        switch (hoverMode)
        {
        case EHoverMode::EHM_NONE:
            break;
        case EHoverMode::EHM_X:
            std::cout << "EHoverMode::EHM_X" << "\n";
            break;
        case EHoverMode::EHM_Y:
            std::cout << "EHoverMode::EHM_Y" << "\n";

            break;
        case EHoverMode::EHM_Z:
            std::cout << "EHoverMode::EHM_Z" << "\n";

            break;
        case EHoverMode::EHM_MAX:
            break;
        default:
            break;
        }
    }

}
