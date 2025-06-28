#pragma once

#include <unordered_map>
#include <KMGActor.h>
#include <memory>
#include <KMGDataStructure.h>


class KMGScene {
public:
    std::unordered_map<std::wstring, std::unique_ptr<KMGActor>> actors;

    KMGActor* CreateActor(const std::wstring& name);
    void EraseActor(const std::wstring& name);

    KMGActor* GetActor(const std::wstring& name);
   
};