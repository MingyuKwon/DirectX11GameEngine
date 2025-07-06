#include <DrawResourceManager.h>
#include <LoadingManager.h>

using namespace std;
using namespace DirectX;

void DrawResourceManager::AddShouldDrawActor(
    std::atomic<bool>& bUpdateReource, 
    std::wstring actorName, 
    const std::vector<KMGStaticMesh>* actorMeshes, 
    ID3D11DeviceContext* pMainContext, 
    XMFLOAT4 lightColor, 
    DirectX::XMMATRIX worldMatrix)
{
    shouldDrawActor[actorName] = (actorMeshes == nullptr) ? 0 : actorMeshes->size();

    LoadingManager* loading = nullptr;

    if (bUpdateReource)
    {
        loading = new LoadingManager(ELoadingType::ELT_MAKE_GPU_DATA);
        loading->SetTotalCount((actorMeshes == nullptr) ? 1 : actorMeshes->size());
    }

    if (actorMeshes == nullptr)
    {
        wstring DefaultMeshName = actorName + L"___DEFAULT";
        if (bUpdateReource)
        {
            drawActorResources.emplace(DefaultMeshName, DrawResource(DefaultMeshName));

            KMGStaticMesh newDefaultMesh = KMGStaticMesh::CreateDefaultSphereMesh(1.f, lightColor);
            drawActorResources[DefaultMeshName].UpdateBuffers(newDefaultMesh.vertices, newDefaultMesh.indices, newDefaultMesh.textureFilePath, newDefaultMesh.normalMapFilePath);
        }

        drawActorResources[DefaultMeshName].bLightEffected = lightColor.x < 0;
        drawActorResources[DefaultMeshName].UpdateActorCB(pMainContext, worldMatrix);

    }
    else
    {
        for (int i = 0; i < actorMeshes->size(); i++)
        {
            wstring meshName = actorName + L"___" + to_wstring(i);
            const KMGStaticMesh& actorMesh = (*actorMeshes)[i];

            if (drawActorResources.count(meshName) == 0)
            {
                drawActorResources.emplace(meshName, DrawResource(meshName));
            }

            if (bUpdateReource)
            {
                drawActorResources[meshName].UpdateBuffers(actorMesh.vertices, actorMesh.indices, actorMesh.textureFilePath, actorMesh.normalMapFilePath);
                loading->PlusCurrentCount();
            }

            drawActorResources[meshName].bLightEffected = lightColor.x < 0;
            drawActorResources[meshName].UpdateActorCB(pMainContext, worldMatrix);
        }
    }

    bUpdateReource = false;

    if (loading)
    {
        loading->StopLoading();
        delete loading;
        loading = nullptr;
    }
}


void DrawResourceManager::AddShouldDrawDebug(
    bool bUpdateResource,
    std::wstring meshName, const KMGDebugMesh& debugMesh, ID3D11DeviceContext* pMainContext, DirectX::XMMATRIX worldMatrix)
{
    if (shouldDrawDebug.count(meshName) == 0)
    {
        drawDebugResources.emplace(meshName, DrawResource(meshName));
        drawDebugResources[meshName].UpdateBuffers(debugMesh.vertices, debugMesh.indices, DEFAULT_TEXTURE_FILEPATH, DEFAULT_NORMAL_FILEPATH);
        drawDebugResources[meshName].bDebug = true;
    }

    shouldDrawDebug[meshName]++;

    if (bUpdateResource)
    {
        drawDebugResources[meshName].UpdateBuffers(debugMesh.vertices, debugMesh.indices, DEFAULT_TEXTURE_FILEPATH, DEFAULT_NORMAL_FILEPATH);
    }

    drawDebugResources[meshName].UpdateActorCB(pMainContext, worldMatrix);

}

void DrawResourceManager::ArrangeActorResource()
{
    unordered_set<wstring> shouldDrawResourceName;

    for (const auto& bucket : shouldDrawActor)
    {
        wstring actorName = bucket.first;
        int actorMeshCount = bucket.second;

        if (actorMeshCount == 0)
        {
            shouldDrawResourceName.insert(actorName + L"___DEFAULT");
        }

        for (int i=0; i< actorMeshCount; i++)
        {
            wstring meshName = actorName + L"___" + to_wstring(i);
            shouldDrawResourceName.insert(meshName);
        }
    }

    for (auto it = drawActorResources.begin(); it != drawActorResources.end(); )
    {
        if (shouldDrawResourceName.count(it->first) == 0)
        {
            it = drawActorResources.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void DrawResourceManager::ArrangeDebugResource()
{
    // 여기서 먼저 쭉 둘러보면서 count가 0이면 죽을 차례라는 거다
    unordered_set<wstring> shouldEraseDebugName;
    for (const auto& bucket : shouldDrawDebug)
    {
        wstring resourceName = bucket.first;
        int count = bucket.second;

        if (count == 0)
        {
            shouldEraseDebugName.insert(resourceName);
        }
    }

    for (const std::wstring& name : shouldEraseDebugName)
    {
        drawDebugResources.erase(name);
        shouldDrawDebug.erase(name);
    }

    // 그리고 마지막으로 살아남은 놈들은 전부 -1씩 해준다
    // 이러고 다음 틱에 +1을 받지 못하면 그놈은 다음 틱에서 죽는다
    for (auto& bucket : shouldDrawDebug)
    {
        int& count = bucket.second;
        --count;
    }
}
