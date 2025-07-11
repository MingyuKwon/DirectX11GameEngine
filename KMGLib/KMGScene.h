#pragma once

#include <unordered_map>
#include <KMGActor.h>
#include <KMGCamera.h>

#include <memory>
#include <KMGDataStructure.h>

class KMGScene {
public:

    KMGScene();

    void CreateAxis();
    void ChangeAxisTransform();

    KMGActor* CreateActor(std::wstring name);
    void EraseActor(const std::wstring& name);

    void Tick(float deltaTime);

    inline void AddDebugMesh(std::wstring meshName, KMGDebugMesh mesh, float time)
    {
        debugMeshes[meshName] = mesh;

        if (time > 0) debugLifeTime[meshName] = time;
    }

    // 이 함수는 얻고 나서 debugMesh 항목을 초기화 하므로 그리기 전에 한번만 호출해야 한다
    inline std::unordered_map<std::wstring, KMGDebugMesh> GetDebugMeshes()
    {
        std::unordered_map<std::wstring, KMGDebugMesh> temp = debugMeshes;

        for (auto it = debugMeshes.begin(); it != debugMeshes.end(); )
        {
            if (debugLifeTime.count(it->first) == 0)
            {
                it = debugMeshes.erase(it);
            }
            else
            {
                ++it;
            }
        }

        return temp;
    }

    KMGActor* GetActor(const std::wstring& name);

    inline KMGActor* GetAxisActor() {
        return axisActor.get();
    }

    inline EHoverMode GetHoverMode() {
        return hoverMode;
    }


    inline KMGActor* GetFocusActor() {
        return focusActor;
    }

    inline void SetFocusActor(std::wstring actorName)
    {
        if (L"NONE" != actorName && actors.count(actorName) == 0)
        {
            return;
        }

        for (auto& actor : actors)
        {
            if (actor.first == actorName)
            {
                focusActor = actor.second.get();
            }
            
            actor.second->bShowBoundBox = actor.first == actorName;
        }

        if (L"NONE" == actorName)
        {
            focusActor = nullptr;
            SetSceneMode(ESceneMode::ESM_SELECT);
        }

    }

    inline KMGCamera& GetCurrentCamera() { return currentCamera; }

    void RenameActor(std::wstring beforeName, std::wstring aftername);
   
    // 이 함수는 무조건 모든 actors 접근이 끝난 후에 렌더링 단계에만 불러야 한다
    const std::unordered_map<std::wstring, std::unique_ptr<KMGActor>>& getAllActors();
    inline std::unordered_set<std::wstring>& GetActorNames() {
        return actorNames;
    }

    inline ESceneMode GetSceneMode()
    {
        return sceneMode;
    }

    inline void SetSceneMode(ESceneMode mode)
    {
        sceneMode = mode;

        switch (sceneMode)
        {
        case ESceneMode::ESM_NONE:
            break;
        case ESceneMode::ESM_SELECT:
            std::cout << "ESM_SELECT\n";

            break;
        case ESceneMode::ESM_MOVE:
            std::cout << "ESM_MOVE\n";

            break;
        case ESceneMode::ESM_ROTATE:
            std::cout << "ESM_ROTATE\n";

            break;
        case ESceneMode::ESM_SCALE:
            std::cout << "ESM_SCALE\n";

            break;
        case ESceneMode::ESM_MAX:

            break;
        default:
            break;
        }
    }

    void CheckHoverAxis(DirectX::XMVECTOR rayDir);
    void GrabAxis(DirectX::XMVECTOR rayDir, bool bTrigger);

    void ColorHoverAxis();

private:
    int ActorCreateCount = 0;
    ESceneMode sceneMode = ESceneMode::ESM_SELECT;
    EHoverMode hoverMode = EHoverMode::EHM_NONE;

    std::mutex actorMapLock;
    std::unordered_map<std::wstring, std::unique_ptr<KMGActor>> actors;
    std::unordered_set<std::wstring> actorNames;

    // 여기엔 디버그 용으로 그리는 메시들만 넣어둔다
    std::unordered_map<std::wstring, KMGDebugMesh> debugMeshes;
    std::unordered_map<std::wstring, float> debugLifeTime;

    std::unique_ptr<KMGActor> axisActor = nullptr;
    KMGActor* focusActor = nullptr;

    KMGCamera currentCamera;
};