#pragma once

#include <unordered_map>
#include <KMGActor.h>
#include <KMGCamera.h>

#include <memory>
#include <KMGDataStructure.h>

class KMGScene {
public:
    
    KMGActor* CreateActor(const std::wstring& name);
    void EraseActor(const std::wstring& name);

    KMGActor* GetActor(const std::wstring& name);

    inline const KMGCamera& GetCurrentCamera() { return currentCamera; }
   
    // 이 함수는 무조건 모든 actors 접근이 끝난 후에 렌더링 단계에만 불러야 한다
    const std::unordered_map<std::wstring, std::unique_ptr<KMGActor>>& getAllActors();
private:
    std::mutex actorMapLock;
    std::unordered_map<std::wstring, std::unique_ptr<KMGActor>> actors;

    KMGCamera currentCamera;
};