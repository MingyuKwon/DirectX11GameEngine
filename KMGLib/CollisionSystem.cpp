#include "CollisionSystem.h"

void CollisionSystem::DetectAndResolveAll(const std::unordered_map<std::wstring, std::unique_ptr<KMGActor>>& actors)
{

}

CollisionInfo CollisionSystem::CheckAABBCollision(
    const DirectX::BoundingBox& a, DirectX::XMMATRIX aWorldMat,
    const DirectX::BoundingBox& b, DirectX::XMMATRIX bWorldMat)
{
    BoundingBox worldABox, worldBBox;
    a.Transform(worldABox, aWorldMat);
    b.Transform(worldBBox, bWorldMat);

    XMVECTOR aCenter = XMLoadFloat3(&worldABox.Center);
    XMVECTOR bCenter = XMLoadFloat3(&worldBBox.Center);
    XMVECTOR aExtents = XMLoadFloat3(&worldABox.Extents);
    XMVECTOR bExtents = XMLoadFloat3(&worldBBox.Extents);

    XMVECTOR delta = XMVectorSubtract(bCenter, aCenter);
    XMVECTOR overlap = XMVectorSubtract(
        XMVectorAdd(aExtents, bExtents),
        XMVectorAbs(delta)
    );

    CollisionInfo info;

    if (XMVectorGetX(overlap) > 0 &&
        XMVectorGetY(overlap) > 0 &&
        XMVectorGetZ(overlap) > 0)
    {
        info.bCollide = true;

        // 가장 적게 겹친 축으로 penetration 및 normal 계산
        float x = XMVectorGetX(overlap);
        float y = XMVectorGetY(overlap);
        float z = XMVectorGetZ(overlap);

        if (x < y && x < z)
        {
            info.penetrationDepth = x;
            info.normal = XMVectorSet((XMVectorGetX(delta) < 0 ? -1.0f : 1.0f), 0, 0, 0);
        }
        else if (y < z)
        {
            info.penetrationDepth = y;
            info.normal = XMVectorSet(0, (XMVectorGetY(delta) < 0 ? -1.0f : 1.0f), 0, 0);
        }
        else
        {
            info.penetrationDepth = z;
            info.normal = XMVectorSet(0, 0, (XMVectorGetZ(delta) < 0 ? -1.0f : 1.0f), 0);
        }

        // 접점 위치 (간단한 버전: 두 중심점 사이 중간 지점)
        info.contactPoint = XMVectorAdd(aCenter, XMVectorScale(delta, 0.5f));
    }

    return info;
}


void CollisionSystem::ResolveCollision(RigidBodyComponent* a, RigidBodyComponent* b, const CollisionInfo& info)
{

}
