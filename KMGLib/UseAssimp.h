#pragma once
#include <KMGDataStructure.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "KMGActor.h"

class LoadingManager;

bool LoadModelToActor(const std::string& filePath, KMGActor& outActor, LoadingManager& loadingManager);

void Recursive_NodeProcess(aiNode* node, const aiScene* scene, std::vector<KMGMesh>& allMeshes, LoadingManager& loadingManager);
