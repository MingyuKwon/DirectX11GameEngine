
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

    std::vector<KMGMesh> allMeshes;

    Recursive_NodeProcess(scene->mRootNode, scene, allMeshes);

    outActor.SetMeshData(std::move(allMeshes));
    return true;
}

void Recursive_NodeProcess(aiNode* node, const aiScene* scene, std::vector<KMGMesh>& allMeshes)
{
    for (unsigned int i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        KMGMesh currentMesh;

        // Vertices
        for (unsigned int v = 0; v < mesh->mNumVertices; ++v)
        {
            KMGVertex vertex;

            vertex.Pos = { mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z };

            if (mesh->HasNormals())
                vertex.Normal = { mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z };
            else
                vertex.Normal = { 0.0f, 1.0f, 0.0f };

            if (mesh->HasTextureCoords(0))
                vertex.Tex = { mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y };
            else
                vertex.Tex = { 0.0f, 0.0f };

            vertex.Color = { 1.0f, 1.0f, 1.0f, 1.0f };

            currentMesh.vertices.push_back(vertex);
        }

        // Indices (with offset!)
        for (unsigned int f = 0; f < mesh->mNumFaces; ++f)
        {
            aiFace face = mesh->mFaces[f];
            for (unsigned int j = 0; j < face.mNumIndices; ++j)
            {
                currentMesh.indices.push_back(face.mIndices[j]);
            }
        }

        allMeshes.push_back(std::move(currentMesh));
    }

    for (unsigned int i = 0; i < node->mNumChildren; ++i)
    {
        Recursive_NodeProcess(node->mChildren[i], scene, allMeshes);
    }
}
