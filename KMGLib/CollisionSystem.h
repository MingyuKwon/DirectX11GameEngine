#pragma once
#include <KMGDataStructure.h>
#include <KMGComponent.h>

class KMGActor;


class CollisionSystem {
public:
    void DetectAndResolveAll(const std::unordered_map<std::wstring, std::unique_ptr<KMGActor>>& actors);

private:
    CollisionInfo CheckOBBCollision(const DirectX::BoundingBox& a, DirectX::XMMATRIX aWolrdMat, const DirectX::BoundingBox& b, DirectX::XMMATRIX bWolrdMat);

    void ResolveCollision(RigidBodyComponent* a, RigidBodyComponent* b, const CollisionInfo& info);

    OBB CreateOBBFromAABB(const BoundingBox& box, const DirectX::XMMATRIX& world);

};