
#include <UseAssimp.h>
#include <iostream>

bool LoadModelToActor(const std::string& filePath, KMGActor& outActor)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(filePath,
        aiProcess_Triangulate |
        aiProcess_GenNormals |
        aiProcess_JoinIdenticalVertices |
        aiProcess_CalcTangentSpace |
        aiProcess_ConvertToLeftHanded);

    if (!scene || !scene->mRootNode || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE))
    {
        std::cerr << "Assimp error: " << importer.GetErrorString() << std::endl;
        return false;
    }

    std::vector<KMGVertex> allVertices;
    std::vector<int> allIndices;

    Recursive_NodeProcess(scene->mRootNode, scene, allVertices, allIndices);

    outActor.SetMeshData(std::move(allVertices), std::move(allIndices));
    return true;
}

void Recursive_NodeProcess(aiNode* node, const aiScene* scene, std::vector<KMGVertex>& allVertices, std::vector<int>& allIndices)
{
    for (unsigned int i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

        // Vertices
        for (unsigned int v = 0; v < mesh->mNumVertices; ++v)
        {
            KMGVertex vertex;

            // Position
            vertex.Pos = { mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z };

            // Normal
            if (mesh->HasNormals())
                vertex.Normal = { mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z };
            else
                vertex.Normal = { 0.0f, 1.0f, 0.0f }; // fallback

            // Texture Coordinates (use channel 0)
            if (mesh->HasTextureCoords(0))
                vertex.Tex = { mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y };
            else
                vertex.Tex = { 0.0f, 0.0f };

            // Default Color (no vertex color in Assimp by default)
            vertex.Color = { 1.0f, 1.0f, 1.0f, 1.0f };

            allVertices.push_back(vertex);
        }

        // Indices
        for (unsigned int f = 0; f < mesh->mNumFaces; ++f)
        {
            aiFace face = mesh->mFaces[f];
            for (unsigned int j = 0; j < face.mNumIndices; ++j)
            {
                allIndices.push_back(face.mIndices[j]);
            }
        }
    }

    for (unsigned int i = 0; i < node->mNumChildren; ++i)
    {
        Recursive_NodeProcess(node->mChildren[i], scene, allVertices, allIndices);
    }
}
