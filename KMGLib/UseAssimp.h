#pragma once
#include <KMGDataStructure.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "KMGActor.h"

bool LoadModelToActor(const std::string& filePath, KMGActor& outActor);

void Recursive_NodeProcess(aiNode* node, const aiScene* scene, std::vector<KMGMesh>& allMeshes);
