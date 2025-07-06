#pragma once
#include <KMGDataStructure.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "KMGActor.h"

class LoadingManager;

bool LoadModelToActor(const std::string& filePath, KMGActor& outActor, LoadingManager& loadingManager, const std::wstring& textureName, const std::wstring& normalMapName);

void Recursive_NodeProcess(aiNode* node, const aiScene* scene, std::vector<KMGStaticMesh>& allMeshes, LoadingManager& loadingManager, const std::wstring& textureName, const std::wstring& normalMapName);
