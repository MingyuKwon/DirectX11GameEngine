#include "KMGScene.h"

KMGActor* KMGScene::CreateActor(const std::wstring& name)
{
    auto actor = std::make_unique<KMGActor>(name);

    KMGActor* ptr = actor.get();
    actors[name] = std::move(actor);
    return ptr;
}

void KMGScene::EraseActor(const std::wstring& name)
{

}

KMGActor* KMGScene::GetActor(const std::wstring& name)
{
    auto it = actors.find(name);
    if (it != actors.end()) return it->second.get();
    return nullptr;
}
