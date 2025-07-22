#pragma once
#include <KMGDataStructure.h>
#include <KMGComponent.h>

class KMGActor;

struct CollisionInfo {
    bool bCollide = false;

    DirectX::XMVECTOR normal;      // 충돌 방향
    float penetrationDepth;        // 겹친 정도
    DirectX::XMVECTOR contactPoint; // 접점 위치 
};

class CollisionSystem {
public:
    void DetectAndResolveAll(const std::unordered_map<std::wstring, std::unique_ptr<KMGActor>>& actors);

private:
    CollisionInfo CheckAABBCollision(const DirectX::BoundingBox& a, DirectX::XMMATRIX aWolrdMat, const DirectX::BoundingBox& b, DirectX::XMMATRIX bWolrdMat);

    void ResolveCollision(RigidBodyComponent* a, RigidBodyComponent* b, const CollisionInfo& info);
};