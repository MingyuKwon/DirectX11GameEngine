#include "KMGScene.h"

KMGActor* KMGScene::CreateActor(const std::wstring& name)
{
    if (actors.count(name) != 0) return nullptr;

    auto actor = std::make_unique<KMGActor>(name);

    KMGActor* ptr = actor.get();
    actors[name] = std::move(actor);
    return ptr;
}

void KMGScene::EraseActor(const std::wstring& name)
{
    if (actors.count(name) == 0) return;
    actors.erase(name);
}

KMGActor* KMGScene::GetActor(const std::wstring& name)
{
    if (actors.count(name) != 0) return actors[name].get();
    return nullptr;
}

const std::unordered_map<std::wstring, std::unique_ptr<KMGActor>>& KMGScene::getAllActors()
{
    return actors;
}
