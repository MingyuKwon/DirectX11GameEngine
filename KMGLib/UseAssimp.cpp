
#include <UseAssimp.h>
#include <iostream>
#include <LoadingManager.h>


aiTextureType types[] = {
    aiTextureType_DIFFUSE,
    aiTextureType_NORMALS,
    aiTextureType_SPECULAR,
    aiTextureType_HEIGHT,
    aiTextureType_EMISSIVE,
    aiTextureType_OPACITY
};

std::wstring Utf8ToWstring(const std::string& str)
{
    std::string fileName = str;
    size_t pos = str.find_last_of("\\\/");
    if (pos != std::string::npos)
        fileName = str.substr(pos + 1);

    int size_needed = MultiByteToWideChar(CP_UTF8, 0, fileName.c_str(), -1, nullptr, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, fileName.c_str(), -1, &wstrTo[0], size_needed);

    // null 문자 제거
    wstrTo.resize(size_needed - 1);

    return DEFAULT_TEXTURE_FOLDER + wstrTo;
}

bool LoadModelToActor(const std::string& filePath, KMGActor& outActor, LoadingManager& loadingManager)
{
    std::cout << "Loading Mesh Start : \n";

    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(filePath,
        aiProcess_Triangulate |
        aiProcess_GenNormals |
        aiProcess_CalcTangentSpace | 
        aiProcess_ConvertToLeftHanded);

    if (!scene || !scene->mRootNode || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE))
    {
        std::cerr << "Assimp error: " << importer.GetErrorString() << std::endl;
        return false;
    }

    std::vector<KMGStaticMesh> allMeshes;

    loadingManager.SetTotalCount(scene->mNumMeshes); // 여기에 메시의 총 개수 세팅

    Recursive_NodeProcess(scene->mRootNode, scene, allMeshes, loadingManager);

    outActor.SetMeshData(std::move(allMeshes));
    return true;
}

void Recursive_NodeProcess(aiNode* node, const aiScene* scene, std::vector<KMGStaticMesh>& allMeshes, LoadingManager& loadingManager)
{
    for (unsigned int i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        KMGStaticMesh currentMesh;

        // 텍스처 경로 가져오기
        if (mesh->mMaterialIndex >= 0)
        {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

            for (aiTextureType type : types)
            {
                unsigned int texCount = material->GetTextureCount(type);

                for (unsigned int i = 0; i < texCount; ++i)
                {

                    aiString path;
                    if (material->GetTexture(type, i, &path) == AI_SUCCESS)
                    {
                        std::string typeName;
                        switch (type)
                        {
                        case aiTextureType_DIFFUSE:
                        {
                            typeName = "DIFFUSE"; 
                            break;
                        }
                        case aiTextureType_NORMALS: 
                        {
                            typeName = "NORMALS";
                            std::cout << "[Normal Map : " << typeName << "] " << path.C_Str() << std::endl;
                            currentMesh.normalMapFilePath = Utf8ToWstring(path.C_Str());

                            break;
                        }
                        case aiTextureType_SPECULAR:
                        {
                            typeName = "SPECULAR";
                            break;
                        }
                        case aiTextureType_HEIGHT:
                        {
                            typeName = "HEIGHT";
                            std::cout << "[Normal Map : " << typeName << "] " << path.C_Str() << std::endl;
                            currentMesh.normalMapFilePath = Utf8ToWstring(path.C_Str());
                            break;
                        }
                        case aiTextureType_EMISSIVE: 
                        {
                            typeName = "EMISSIVE";
                            break;
                        }
                        case aiTextureType_OPACITY: 
                        {
                            typeName = "OPACITY";
                            break;
                        }
                        default: typeName = "OTHER"; break;
                        }


                        //currentMesh.textureFilePath = Utf8ToWstring(path.C_Str());

                    }
                    else
                    {
                        std::cout << "[텍스처를 가져올 수 없음] " << path.C_Str() << "\n";
                    }
                }
            }
           
        }

        // 정점 버퍼
        for (unsigned int v = 0; v < mesh->mNumVertices; ++v)
        {
            KMGVertex vertex;
            vertex.Pos = { mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z };

            if (mesh->HasTextureCoords(0))
                vertex.Tex = { mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y };
            else
                vertex.Tex = { 0.0f, 0.0f };

            vertex.Color = { 1.0f, 1.0f, 1.0f, 1.0f };

            if (mesh->HasNormals())
                vertex.Normal = { mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z };
            else
                vertex.Normal = { 0.0f, 1.0f, 0.0f };


            if (mesh->HasTangentsAndBitangents())
            {
                vertex.Tangent = {
                    mesh->mTangents[v].x,
                    mesh->mTangents[v].y,
                    mesh->mTangents[v].z
                };

                vertex.Binormal = {
                    mesh->mBitangents[v].x,
                    mesh->mBitangents[v].y,
                    mesh->mBitangents[v].z
                };

            }
            else
            {
                vertex.Tangent = { 1.0f, 0.0f, 0.0f };   // 기본값
                vertex.Binormal = { 0.0f, 0.0f, 1.0f };
            }


            currentMesh.vertices.push_back(vertex);
        }

        // 인덱스 버퍼
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
        Recursive_NodeProcess(node->mChildren[i], scene, allMeshes, loadingManager);
    }

    loadingManager.PlusCurrentCount();

}