#include "KMGScene.h"
#include "KMGUtility.h"
#include "UseAssimp.h"
#include "QueueCommand.h"

using namespace DirectX;

KMGScene::KMGScene()
{
    CreateAxis();
}

void KMGScene::InitializeScene()
{
    KMGCommand::AddActor(L"Directional Light");
    KMGCommand::TranslateActor(L"Directional Light", XMVectorSet(0, 10, 0, 0));
    KMGCommand::AddLightComponent(L"Directional Light");
    KMGCommand::UpdateLightComponent_Type(L"Directional Light", 0);

}

void KMGScene::CreateAxis()
{
    // 먼저 AxisActor 만들기
    axisActor = std::make_unique<KMGActor>(-1, L"System_Axis");
    LoadModelToActor(DEFAULT_AXISMESH_PATH, *axisActor, nullptr);
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
                (*meshes)[i].bShouldMeshChange = true;

                std::vector<KMGVertex>& axisMesh = (*meshes)[i].vertices;
                for (KMGVertex& vertex : axisMesh)
                {
                    vertex.Color = axisModes[i] == hoverMode ? hoverAxisColors[i] : axisColors[i];
                }

            }
        }
    }
    
    prevHoverMode = hoverMode;
}

void KMGScene::ChangeAxisTransform()
{
    if (focusActor == nullptr || sceneMode == ESceneMode::ESM_SELECT)
    {
        axisActor->SetVisibility(false);
        return;
    }

    axisActor->SetVisibility(true);
    axisActor->SetPosition(focusActor->GetPosition());

    // 여기서 카메라와 actor의 사이 간격에 따라 scale을 바꾸도록 하자

    // 대신 카메라의 위치가 정해져 있다면 axis의 위치도 고정이어야 한다
    DirectX::XMVECTOR cameraGap = currentCamera.GetCameraPosition() - axisActor->GetPosition();
    float distance = XMVectorGetX(XMVector3Length(cameraGap));
    float scaledDistance = sqrtf(distance) * 0.02f;
    axisActor->SetScale(scaledDistance, scaledDistance, scaledDistance);


    if (sceneMode == ESceneMode::ESM_MOVE)
    {
        axisActor->SetRotation_Q(XMVectorSet(0, 0, 0, 0));
    }
    else
    {
        axisActor->SetRotation_Q(focusActor->GetRotation_Q());
    }
}

KMGActor* KMGScene::CreateActor(std::wstring name)
{
    std::lock_guard<std::mutex> lock(actorMapLock);

    if (actors.count(name) != 0) return nullptr;

    // 이 경우에는 그냥 이름 안주고 만들라고 하는거다
    if (DEFAULT_ACTOR_NAME == name)
    {
        name += std::to_wstring(ActorAddCount);
    }

    auto actor = std::make_unique<KMGActor>(ActorAddCount, name);
    KMGActor* ptr = actor.get();

    AddActor(std::move(actor));

    return ptr;
}

void KMGScene::CopyActor(std::wstring name)
{
    std::lock_guard<std::mutex> lock(actorMapLock);

    KMGActor* copyTargetActor = actors[name].get();

    name += L"_";

    int i = 0;
    while (true)
    {
        std::wstring newName = name + std::to_wstring(i);

        if (actors.count(newName) == 0)
        {
            name = newName;
            break;
        }

        i++;
    }

    auto actor = std::make_unique<KMGActor>(ActorAddCount, name);

    copyTargetActor->CopyActorToTarget(actor.get());

    AddActor(std::move(actor));
}

void KMGScene::AddActor(std::unique_ptr<KMGActor>&& actor)
{
    if (actors.count(actor->GetName()) != 0) return;
    
    std::wstring actorName = actor->GetName();

    actors[actorName] = std::move(actor);

    ActorAddCount++;

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
    ChangeAxisTransform();
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

    actors[aftername]->SetName(aftername);

}

const std::unordered_map<std::wstring, std::unique_ptr<KMGActor>>& KMGScene::getAllActors()
{
    std::lock_guard<std::mutex> lock(actorMapLock);

    return actors;
}

void KMGScene::CheckHoverAxis(DirectX::XMVECTOR rayDir)
{
    if (axisActor == nullptr) return;

    if (!axisActor->IsVisible()) return;

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

void KMGScene::GrabAxis(DirectX::XMVECTOR rayDir, bool bTrigger)
{
    static bool bInitialized = false;
    static DirectX::XMVECTOR prevPosition = XMVectorSet(0,0,0,0);

    if (focusActor == nullptr || !bTrigger)
    {
        bInitialized = false;
        return;
    }

    DirectX::XMVECTOR rayOrigin = currentCamera.GetCameraPosition();
    DirectX::XMVECTOR focusActorPosition = focusActor->GetPosition();

    switch (sceneMode)
    {
    case ESceneMode::ESM_NONE:
        break;
    case ESceneMode::ESM_SELECT:
        break;
    case ESceneMode::ESM_MOVE:
        TranslateAxis(rayDir, rayOrigin, focusActorPosition, bInitialized);
        break;
    case ESceneMode::ESM_ROTATE:
        RotateAxis(rayDir, rayOrigin, focusActorPosition, bInitialized);
        break;
    case ESceneMode::ESM_SCALE:
        ScaleAxis(rayDir, rayOrigin, focusActorPosition, bInitialized);
        break;
    case ESceneMode::ESM_MAX:
        break;
    default:
        break;
    }

    bInitialized = true;
}

void KMGScene::TranslateAxis(DirectX::XMVECTOR rayDir, DirectX::XMVECTOR rayOrigin, DirectX::XMVECTOR focusActorPosition, bool bInitialized)
{
    static DirectX::XMVECTOR prevPosition = XMVectorSet(0, 0, 0, 0);

    DirectX::XMVECTOR aimVec;

    switch (hoverMode)
    {
    case EHoverMode::EHM_X:
        aimVec = XMVectorSet(1, 0, 0, 0);
        break;

    case EHoverMode::EHM_Y:
        aimVec = XMVectorSet(0, 1, 0, 0);

        break;

    case EHoverMode::EHM_Z:
        aimVec = XMVectorSet(0, 0, 1, 0);
        break;

    default:
        return;
    }

    DirectX::XMVECTOR moveVec = KMGUtility::GenerateMoveDeltaVector(
        rayDir,
        rayOrigin,
        focusActorPosition,
        aimVec
    );

    DirectX::XMVECTOR pointPosition = focusActorPosition + moveVec;

    if (bInitialized)
    {
        DirectX::XMVECTOR gapVector = pointPosition - prevPosition;
        KMGCommand::TranslateActor(focusActor->GetName(), gapVector);
    }

    prevPosition = pointPosition;

}

void KMGScene::RotateAxis(DirectX::XMVECTOR rayDir, DirectX::XMVECTOR rayOrigin, DirectX::XMVECTOR focusActorPosition, bool bInitialized)
{
    static XMVECTOR prevPosition = XMVectorSet(0, 0, 0, 0);
    static XMVECTOR cachedWorldAxis;

    switch (hoverMode)
    {
    case EHoverMode::EHM_X: cachedWorldAxis = XMVectorSet(1, 0, 0, 0); break;
    case EHoverMode::EHM_Y: cachedWorldAxis = XMVectorSet(0, 1, 0, 0); break;
    case EHoverMode::EHM_Z: cachedWorldAxis = XMVectorSet(0, 0, 1, 0); break;
    default: return;
    }

    DirectX::XMMATRIX focusWorldMat = focusActor->getWorldMatrix();
    DirectX::XMMATRIX inv_focusWorldMat = XMMatrixInverse(nullptr, focusWorldMat);

    cachedWorldAxis = XMVector3TransformNormal(cachedWorldAxis, focusWorldMat);
    cachedWorldAxis = XMVector3Normalize(cachedWorldAxis);

    DirectX::XMVECTOR moveVec = KMGUtility::GenerateMoveDeltaVector(
        rayDir,
        rayOrigin,
        focusActorPosition,
        cachedWorldAxis
    );

    DirectX::XMVECTOR pointPosition = focusActorPosition + moveVec;

    if (bInitialized)
    {
        XMVECTOR gapVector = pointPosition - prevPosition;

        if (!XMVector3Equal(gapVector, XMVectorZero()))
        {
            gapVector = XMVector3Normalize(gapVector);
            float dot = XMVectorGetX(XMVector3Dot(gapVector, cachedWorldAxis));
            float radian = XMVectorGetX(XMVector3Length(pointPosition - prevPosition));

            radian *= (dot >= 0 ? 1.0f : -1.0f);

            if (fabs(radian) > 0.0001f)
            {
                cachedWorldAxis = XMVector3TransformNormal(cachedWorldAxis, inv_focusWorldMat);

                KMGCommand::RotateActor(focusActor->GetName(), cachedWorldAxis, radian * 0.5f);
            }
        }
    }

    prevPosition = pointPosition;
}




void KMGScene::ScaleAxis(DirectX::XMVECTOR rayDir, DirectX::XMVECTOR rayOrigin, DirectX::XMVECTOR focusActorPosition, bool bInitialized)
{
    static DirectX::XMVECTOR prevPosition = XMVectorSet(0, 0, 0, 0);

    DirectX::XMVECTOR aimVec;

    switch (hoverMode)
    {
    case EHoverMode::EHM_X:
        aimVec = XMVectorSet(1, 0, 0, 0);
        break;

    case EHoverMode::EHM_Y:
        aimVec = XMVectorSet(0, 1, 0, 0);

        break;

    case EHoverMode::EHM_Z:
        aimVec = XMVectorSet(0, 0, 1, 0);
        break;

    default:
        return;
    }

    DirectX::XMMATRIX focusWorldMat = focusActor->getWorldMatrix();
    DirectX::XMMATRIX inv_focusWorldMat = XMMatrixInverse(nullptr, focusWorldMat);

    aimVec = XMVector3TransformNormal(aimVec, focusWorldMat);
    aimVec = XMVector3Normalize(aimVec);

    DirectX::XMVECTOR moveVec = KMGUtility::GenerateMoveDeltaVector(
        rayDir,
        rayOrigin,
        focusActorPosition,
        aimVec
    );

    DirectX::XMVECTOR pointPosition = focusActorPosition + moveVec;


    if (bInitialized)
    {
        DirectX::XMVECTOR gapVector = pointPosition - prevPosition;

        if (!XMVector3Equal(gapVector, XMVectorZero()))
        {
            float radian = XMVectorGetX(XMVector3Length(gapVector));

            gapVector = XMVector3TransformNormal(gapVector, inv_focusWorldMat);
            gapVector = XMVector3Normalize(gapVector);

            gapVector = XMVectorSetZ(gapVector, -XMVectorGetZ(gapVector));

            KMGCommand::UpdateActorScale(focusActor->GetName(), focusActor->GetScale() + gapVector * radian * 0.2);

        }


    }

    prevPosition = pointPosition;

}


