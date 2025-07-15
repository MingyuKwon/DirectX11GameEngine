#include <DrawResourceManager.h>
#include <LoadingManager.h>
#include <TextureLoader.h>

using namespace std;
using namespace DirectX;

DrawResourceManager::DrawResourceManager()
{

}

DrawResourceManager::~DrawResourceManager()
{
    for (auto& bucket : textureSRVs)
    {
        ID3D11ShaderResourceView* srv = bucket.second;
        if (srv)
        {
            srv->Release();
            srv = nullptr;
        }
    }
}

void DrawResourceManager::AddAxisActor(bool bVisible,
    std::vector<KMGStaticMesh>* actorMeshes, ID3D11DeviceContext* pMainContext, DirectX::XMMATRIX worldMatrix)
{
    if (actorMeshes)
    {
        for (int i = 0; i < actorMeshes->size(); i++)
        {
            wstring meshName = L"EDIT_AXIS";
            KMGStaticMesh& actorMesh = (*actorMeshes)[i];

            if (axisResource[i] == nullptr)
            {
                axisResource[i] = std::make_unique<DrawResource>(meshName);
            }

            if (actorMesh.bShouldMeshChange)
            {
                axisResource[i]->UpdateBuffers(actorMesh.vertices, actorMesh.indices);
            }

            axisResource[i]->pTextureSRV = GetTextureSRV(actorMesh.textureFilePath);
            axisResource[i]->pNormalMapSRV = GetTextureSRV(actorMesh.normalMapFilePath);

            axisResource[i]->bVisible = bVisible;
            axisResource[i]->bLightEffected = false;
            axisResource[i]->UpdateActorCB(pMainContext, worldMatrix);

            actorMesh.bShouldMeshChange = false;
        }
    }
}

void DrawResourceManager::AddShouldDrawActor(
    bool bVisible,
    std::wstring actorName,
    std::vector<KMGStaticMesh>* actorMeshes, 
    ID3D11DeviceContext* pMainContext, 
    XMFLOAT4 lightColor, 
    DirectX::XMMATRIX worldMatrix)
{
    shouldDrawActor[actorName] = (actorMeshes == nullptr) ? 0 : actorMeshes->size();

    LoadingManager* loading = nullptr;
    int shouldUpdateCount = 0;

    if (actorMeshes)
    {
        for (int i = 0; i < actorMeshes->size(); ++i)
        {
            if ((*actorMeshes)[i].bShouldMeshChange) ++shouldUpdateCount;
        }
    }

    if (shouldUpdateCount > 0)
    {
        loading = new LoadingManager(ELoadingType::ELT_MAKE_GPU_DATA);
        loading->SetTotalCount(shouldUpdateCount);
    }

    if (actorMeshes)
    {
        for (int i = 0; i < actorMeshes->size(); i++)
        {
            wstring meshName = actorName + DEFAULT_NAME_SEPERATOR + to_wstring(i);
            KMGStaticMesh& actorMesh = (*actorMeshes)[i];

            if (drawActorResources.count(meshName) == 0)
            {
                drawActorResources.emplace(meshName, DrawResource(meshName));
            }

            if (actorMesh.bShouldMeshChange)
            {
                cout << "actorMesh.bShouldMeshChange = true\n";
                drawActorResources[meshName].UpdateBuffers(actorMesh.vertices, actorMesh.indices);
                if (loading)
                {
                    loading->PlusCurrentCount();
                }

            }

            drawActorResources[meshName].pTextureSRV = GetTextureSRV(actorMesh.textureFilePath);
            drawActorResources[meshName].pNormalMapSRV = GetTextureSRV(actorMesh.normalMapFilePath);

            drawActorResources[meshName].bLightEffected = lightColor.x < 0;
            drawActorResources[meshName].bVisible = bVisible;

            drawActorResources[meshName].UpdateActorCB(pMainContext, worldMatrix);

            actorMesh.bShouldMeshChange = false;
        }
    }

    if (loading)
    {
        loading->StopLoading();
        delete loading;
        loading = nullptr;
    }
}


void DrawResourceManager::AddShouldDrawDebug(
    std::wstring meshName, KMGDebugMesh& debugMesh, ID3D11DeviceContext* pMainContext, DirectX::XMMATRIX worldMatrix)
{

    if (shouldDrawDebug.count(meshName) == 0)
    {
        drawDebugResources.emplace(meshName, DrawResource(meshName));
        drawDebugResources[meshName].UpdateBuffers(debugMesh.vertices, debugMesh.indices);
        drawDebugResources[meshName].bDebug = true;
    }

    shouldDrawDebug[meshName]++;

    if (debugMesh.bShouldDebugChange)
    {
        drawDebugResources[meshName].UpdateBuffers(debugMesh.vertices, debugMesh.indices);
    }

    drawDebugResources[meshName].UpdateActorCB(pMainContext, worldMatrix);

    debugMesh.bShouldDebugChange = false;
}

void DrawResourceManager::ArrangeActorResource()
{
    unordered_set<wstring> shouldDrawResourceName;

    for (const auto& bucket : shouldDrawActor)
    {
        wstring actorName = bucket.first;
        int actorMeshCount = bucket.second;

        for (int i=0; i< actorMeshCount; i++)
        {
            wstring meshName = actorName + DEFAULT_NAME_SEPERATOR + to_wstring(i);
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

void DrawResourceManager::AddTextureSRVs(std::wstring textureFilePath)
{
    std::lock_guard<std::mutex> addSrvLock(SRVLock);

    if (textureSRVs.count(textureFilePath) > 0) return;

    ID3D11ShaderResourceView* srv;

    HRESULT hr = CreateSrvFromTexture(g_pMainDevice, textureFilePath.c_str(), &srv);
    if (FAILED(hr))
    {
        std::wcout << L"textureFilePath : " << textureFilePath << L"  Failed \n";
        return;
    }

    std::wcout << L"textureFilePath : " << textureFilePath << L"  Success \n";
    textureSRVs[textureFilePath] = srv;

}

void DrawResourceManager::MakeRequestTextures()
{
    std::lock_guard<mutex> lock(textureRequestLock);

    for (wstring textureFilePath : textureRequest)
    {
        AddTextureSRVs(textureFilePath);
    }

    textureRequest.clear();
}

ID3D11ShaderResourceView* DrawResourceManager::GetTextureSRV(std::wstring textureFilePath)
{
    {
        std::lock_guard<std::mutex> getSrvLock(SRVLock);

        if (textureSRVs.count(textureFilePath) != 0)
        {
            return textureSRVs[textureFilePath];
        }
    }

    {
        std::lock_guard<std::mutex> lock(textureRequestLock);
        textureRequest.insert(textureFilePath);
    }
    
    return textureSRVs[DEFAULT_NORMAL_FILEPATH];
}
