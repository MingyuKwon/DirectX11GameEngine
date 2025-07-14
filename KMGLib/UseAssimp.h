#pragma once
#include <KMGDataStructure.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "KMGActor.h"

class LoadingManager;

std::vector<KMGStaticMesh> LoadModelToActor(const std::string& filePath, const std::wstring& textureName = DEFAULT_TEXTURE_FILEPATH, const std::wstring& normalMapName = DEFAULT_NORMAL_FILEPATH);

void Recursive_NodeProcess(aiNode* node, const aiScene* scene, std::vector<KMGStaticMesh>& allMeshes, LoadingManager* loadingManager, std::string meshFolder);
