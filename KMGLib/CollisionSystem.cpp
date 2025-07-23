#include "CollisionSystem.h"
#include "KMGUtility.h"
#include "KMGActor.h"

void CollisionSystem::DetectAndResolveAll(const std::unordered_map<std::wstring, std::unique_ptr<KMGActor>>& actors)
{
    for (auto itA = actors.begin(); itA != actors.end(); ++itA)
    {
        for (auto itB = std::next(itA); itB != actors.end(); ++itB)
        {
            KMGActor* actorA = itA->second.get();
            KMGActor* actorB = itB->second.get();

            RigidBodyComponent* rbA = actorA->GetComponent<RigidBodyComponent>(EComponentType::ECT_RIGIDBODY);
            RigidBodyComponent* rbB = actorB->GetComponent<RigidBodyComponent>(EComponentType::ECT_RIGIDBODY);

            if (rbA == nullptr) continue;
            if (rbB == nullptr) continue;

            CollisionInfo info = CheckOBBCollision(
                actorA->GetAABBBox(), actorA->getWorldMatrix(),
                actorB->GetAABBBox(), actorB->getWorldMatrix()
            );

            if (info.bCollide)
            {
                ResolveCollision(rbA, rbB, info);
            }
        }
    }
}

void CollisionSystem::ResolveCollision(RigidBodyComponent* a, RigidBodyComponent* b, const CollisionInfo& info)
{
    // 둘다 키네마틱이면 애초에 충돌 자체가 안일어 난다
    if (a->IsKinematic() && b->IsKinematic()) return;

    XMVECTOR relativeVel = a->GetVelocity();
    if (!b->IsKinematic())
        relativeVel = XMVectorSubtract(relativeVel, b->GetVelocity());

    float velAlongNormal = XMVectorGetX(XMVector3Dot(relativeVel, info.normal));
    if (velAlongNormal > 0.0f) return; // 이미 멀어지고 있는 중이라면 더 이상 힘을 줄 필요가 없음

    float restitution = 0.0f; // 탄성

    float invMassA = a->IsKinematic() ? 0.0f : 1.0f / a->GetMass();
    float invMassB = b->IsKinematic() ? 0.0f : 1.0f / b->GetMass();

    float impulseMag = -(1.0f + restitution) * velAlongNormal;
    impulseMag /= (invMassA + invMassB);

    XMVECTOR impulse = impulseMag * info.normal * 1;

    float size = XMVectorGetX(XMVector3Length(impulse));
    std::cout << size << "\n";

    if (!a->IsKinematic())
        a->ApplyImpulse(impulse); 

    if (!b->IsKinematic())
        b->ApplyImpulse(-impulse);
}

CollisionInfo CollisionSystem::CheckOBBCollision(
    const DirectX::BoundingBox& aBox, DirectX::XMMATRIX aWorldMat,
    const DirectX::BoundingBox& bBox, DirectX::XMMATRIX bWorldMat)
{
    OBB a = CreateOBBFromAABB(aBox, aWorldMat);
    OBB b = CreateOBBFromAABB(bBox, bWorldMat);

    CollisionInfo result;

    XMVECTOR centerGap = XMVectorSubtract(b.center, a.center);
    float R[3][3];
    float AbsR[3][3];

    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            R[i][j] = XMVectorGetX(XMVector3Dot(a.axis[i], b.axis[j]));
            AbsR[i][j] = std::abs(R[i][j]) + EPSILON; // float 오차 보정
        }
    }

    float t[3] = {
        XMVectorGetX(XMVector3Dot(centerGap, a.axis[0])),
        XMVectorGetX(XMVector3Dot(centerGap, a.axis[1])),
        XMVectorGetX(XMVector3Dot(centerGap, a.axis[2]))
    };

    XMFLOAT3 aExtentsF, bExtentsF;
    XMStoreFloat3(&aExtentsF, a.halfSize);
    XMStoreFloat3(&bExtentsF, b.halfSize);

    float aExtents[3] = { aExtentsF.x, aExtentsF.y, aExtentsF.z };
    float bExtents[3] = { bExtentsF.x, bExtentsF.y, bExtentsF.z };


    // 15축 검사
    // A의 로컬 축 기준 (a.axis[0~2])
    for (int i = 0; i < 3; ++i)
    {
        float ra = aExtents[i];
        float rb = bExtents[0] * AbsR[i][0] + bExtents[1] * AbsR[i][1] + bExtents[2] * AbsR[i][2];
        float ta = XMVectorGetX(XMVector3Dot(centerGap, a.axis[i]));

        if (std::abs(ta) > ra + rb)
            return result;
    }

    // B의 로컬 축 기준 (b.axis[0~2])
    for (int i = 0; i < 3; ++i)
    {
        float ra = aExtents[0] * AbsR[0][i] + aExtents[1] * AbsR[1][i] + aExtents[2] * AbsR[2][i];
        float rb = bExtents[i];
        float tb = XMVectorGetX(XMVector3Dot(centerGap, b.axis[i]));
        if (std::abs(tb) > ra + rb)
            return result;
    }

    for (int i = 0; i < 3; ++i) // A의 축
    {
        for (int j = 0; j < 3; ++j) // B의 축
        {
            // 외적 축: axis = a.axis[i] x b.axis[j]
            // 이 축은 a.axis[i]와 수직이므로 a.axis[i] 방향은 영향 없음
            // a의 나머지 두 축만 이 축에 대해 기여함

            float ra =
                aExtents[(i + 1) % 3] * AbsR[(i + 2) % 3][j] + 
                aExtents[(i + 2) % 3] * AbsR[(i + 1) % 3][j];  

            float rb =
                bExtents[(j + 1) % 3] * AbsR[i][(j + 2) % 3] +
                bExtents[(j + 2) % 3] * AbsR[i][(j + 1) % 3];

            float t_proj =
                std::abs(
                    t[(i + 2) % 3] * R[(i + 1) % 3][j] -
                    t[(i + 1) % 3] * R[(i + 2) % 3][j]
                );

            if (t_proj > ra + rb)
            {
                return result; 
            }
        }
    }

    CollisionInfo info = KMGUtility::BuildOBBCollisionInfo(a, b, R, AbsR, t, centerGap);
    return info;


}


OBB CollisionSystem::CreateOBBFromAABB(const BoundingBox& box, const DirectX::XMMATRIX& world)
{
    OBB obb;
    obb.center = XMVector3Transform(XMLoadFloat3(&box.Center), world);
    obb.halfSize = XMLoadFloat3(&box.Extents);

    obb.axis[0] = XMVector3Normalize(XMVector3TransformNormal(XMVectorSet(1, 0, 0, 0), world)); 
    obb.axis[1] = XMVector3Normalize(XMVector3TransformNormal(XMVectorSet(0, 1, 0, 0), world)); 
    obb.axis[2] = XMVector3Normalize(XMVector3TransformNormal(XMVectorSet(0, 0, 1, 0), world)); 

    return obb;
}