#pragma once
#include <KMGDataStructure.h>

class KMGActor {

public:
    KMGActor(std::wstring name);

    std::wstring GetName();
    DirectX::XMMATRIX getWorldMatrix();

    inline DirectX::XMVECTOR GetPosition() const { return transform.position; }
    inline void SetPosition(float x, float y, float z) { transform.position = DirectX::XMVectorSet(x, y, z, 1); }

    inline DirectX::XMVECTOR GetRotation() const { return transform.rotation; }
    inline void SetRotation(float pitch, float yaw, float roll) { transform.rotation = DirectX::XMVectorSet(pitch, yaw, roll, 0); }

    inline DirectX::XMVECTOR GetScale() const { return transform.scale; }
    inline void SetScale(float x, float y, float z) { transform.scale = DirectX::XMVectorSet(x, y, z, 0); }

    void Translate(float dx, float dy, float dz);    // 상대이동
    void Rotate(float dpitch, float dyaw, float droll); // 상대회전

    const std::vector<KMGVertex>& getVertices();
    const std::vector<int>& getIndices();

private:
    std::wstring name;
    std::vector<KMGVertex> vertices = {
        { {-1,1,-1},{},{1,0,0,1},{1,0} }, { {1,1,-1},{},{1,0,0,1},{0,0} },
        { {1,1,1},{},{1,0,0,1},{0,1} }, { {-1,1,1},{},{1,0,0,1},{1,1} },
        { {-1,-1,-1},{},{1,0,0,1},{0,0} }, { {1,-1,-1},{},{1,0,0,1},{1,0} },
        { {1,-1,1},{},{1,0,0,1},{1,1} }, { {-1,-1,1},{},{1,0,0,1},{0,1} },
        { {-1,-1,1},{},{1,0,0,1},{0,1} }, { {-1,-1,-1},{},{1,0,0,1},{1,1} },
        { {-1,1,-1},{},{1,0,0,1},{1,0} }, { {-1,1,1},{},{1,0,0,1},{0,0} },
        { {1,-1,1},{},{1,0,0,1},{1,1} }, { {1,-1,-1},{},{1,0,0,1},{0,1} },
        { {1,1,-1},{},{1,0,0,1},{0,0} }, { {1,1,1},{},{1,0,0,1},{1,0} },
        { {-1,-1,-1},{},{1,0,0,1},{0,1} }, { {1,-1,-1},{},{1,0,0,1},{1,1} },
        { {1,1,-1},{},{1,0,0,1},{1,0} }, { {-1,1,-1},{},{1,0,0,1},{0,0} },
        { {-1,-1,1},{},{1,0,0,1},{1,1} }, { {1,-1,1},{},{1,0,0,1},{0,1} },
        { {1,1,1},{},{1,0,0,1},{0,0} }, { {-1,1,1},{},{1,0,0,1},{1,0} },
    };

    std::vector<int> indices = {
    3,1,0, 2,1,3, 6,4,5, 7,4,6, 11,9,8, 10,9,11,
    14,12,13, 15,12,14, 19,17,16, 18,17,19, 22,20,21, 23,20,22
    };

    KMGTransform transform;
};