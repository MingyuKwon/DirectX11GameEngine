#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>

int main(int, char**)
{
    Assimp::Importer importer;

    // 테스트용 obj 경로 (반드시 유효한 경로로 교체!)
    const std::string filePath = "test.obj";

    const aiScene* scene = importer.ReadFile(filePath,
        aiProcess_Triangulate |             // 삼각형으로 변환
        aiProcess_JoinIdenticalVertices |   // 중복 정점 병합
        aiProcess_GenNormals);              // 노멀 자동 생성

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Assimp 오류: " << importer.GetErrorString() << std::endl;
        return -1;
    }

    std::cout << "파일 로딩 성공: " << filePath << std::endl;
    std::cout << "총 메시 개수: " << scene->mNumMeshes << std::endl;

    aiMesh* mesh = scene->mMeshes[0];
    std::cout << "첫 메시의 정점 개수: " << mesh->mNumVertices << std::endl;

    return 0;
}