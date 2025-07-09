#include "KMGScene.h"
#include "KMGUtility.h"

KMGActor* KMGScene::CreateActor(const std::wstring& name)
{
    std::lock_guard<std::mutex> lock(actorMapLock);

    if (actors.count(name) != 0) return nullptr;

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
