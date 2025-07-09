#include "KMGScene.h"
#include "KMGUtility.h"
#include "UseAssimp.h"
#include "QueueCommand.h"

using namespace DirectX;

KMGScene::KMGScene()
{
    CreateAxis();
}

void KMGScene::ColorHoverAxis()
{
    static EHoverMode prevHoverMode = EHoverMode::EHM_MAX;

    if (prevHoverMode != hoverMode)
    {
        StaticMeshComponent* staticComp = axisActor->GetComponent<StaticMeshComponent>(EComponentType::ECT_STATICMESH);

        DirectX::XMFLOAT4 hoverAxisColors[3] =
        {
            DirectX::XMFLOAT4(0, 0, 1, 1.0f),
            DirectX::XMFLOAT4(0, 1, 0, 1.0f),
            DirectX::XMFLOAT4(1, 0, 0, 1.0f),

        };

        DirectX::XMFLOAT4 axisColors[3] =
        {
            DirectX::XMFLOAT4(0.6f, 0.6f, 0.9f, 1.0f),  
            DirectX::XMFLOAT4(0.6f, 0.9f, 0.6f, 1.0f),  
            DirectX::XMFLOAT4(0.9f, 0.6f, 0.6f, 1.0f),  
        };


        EHoverMode axisModes[3] =
        {
            EHoverMode::EHM_Y,
            EHoverMode::EHM_Z,
            EHoverMode::EHM_X,
        };

        if (staticComp)
        {
            std::vector<KMGStaticMesh>* meshes = staticComp->GetMeshes();
            for (int i = 0; i < 3; i++)
            {
                (*meshes)[i + 1].bShouldMeshChange = true;

                std::vector<KMGVertex>& axisMesh = (*meshes)[i + 1].vertices;
                for (KMGVertex& vertex : axisMesh)
                {
                    vertex.Color = axisModes[i] == hoverMode ? hoverAxisColors[i] : axisColors[i];
                }

            }
        }


    }
    
    prevHoverMode = hoverMode;
}

void KMGScene::CreateAxis()
{
    // 먼저 AxisActor 만들기
    axisActor = std::make_unique<KMGActor>(-1, L"System_Axis");
    axisActor->SetScale(0.1f, 0.1f, 0.1f);

    LoadModelToActor(DEFAULT_AXISMESH_PATH, *axisActor, nullptr);
    
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

    ColorHoverAxis();

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
    }

}

void KMGScene::GrabAxis(DirectX::XMVECTOR moveWorldDir)
{
    std::cout << "GrabAxis\n";

    DirectX::XMVECTOR xAxis = XMVectorSet(1,0,0,1);
    DirectX::XMVECTOR yAxis = XMVectorSet(0, 1, 0, 1);
    DirectX::XMVECTOR zAxis = XMVectorSet(0, 0, 1, 1);
    
    float moveSpeed = 10;

    DirectX::XMVECTOR dotXResult = DirectX::XMVector3Dot(xAxis, moveWorldDir) * moveSpeed;
    DirectX::XMVECTOR dotYResult = DirectX::XMVector3Dot(yAxis, moveWorldDir) * moveSpeed;
    DirectX::XMVECTOR dotZResult = DirectX::XMVector3Dot(zAxis, moveWorldDir) * moveSpeed;

    if (focusActor == nullptr) return;

    switch (hoverMode)
    {
    case EHoverMode::EHM_X:
        KMGCommand::TranslateActor(focusActor->GetName(), dotXResult);
        break;

    case EHoverMode::EHM_Y:
        KMGCommand::TranslateActor(focusActor->GetName(), dotYResult);

        break;

    case EHoverMode::EHM_Z:
        KMGCommand::TranslateActor(focusActor->GetName(), dotZResult);

        break;
    }
}


