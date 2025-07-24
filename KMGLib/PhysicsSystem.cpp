#include "PhysicsSystem.h"
#include "KMGUtility.h"
#include "KMGActor.h"
#include "KMGScene.h"

void PhysicsSystem::Simulate(KMGScene* currentScene, float physicsDeltaTime)
{
    if (currentScene == nullptr) return;

    KMGActor* axisActor = currentScene->GetAxisActor();
    if (axisActor)
    {
        axisActor->ExecuteAllKineticCommand();
    }


    std::lock_guard<std::mutex> actorMapLock(currentScene->GetActorMapLock());

    const std::unordered_map<std::wstring, std::unique_ptr<KMGActor>>& actors = currentScene->getAllActors();

    for (auto& bucket : actors)
    {
        bucket.second->ExecuteAllKineticCommand();

        bucket.second->IntegrateVelocity(physicsDeltaTime);

        bucket.second->PredictPosition(physicsDeltaTime);
    }

    DetectAndResolveAll(actors);

    for (auto& bucket : actors)
    {
        bucket.second->ApplyFinalPosition();
    }
}

void PhysicsSystem::DetectAndResolveAll(const std::unordered_map<std::wstring, std::unique_ptr<KMGActor>>& actors)
{
    std::vector<CollisionInfo> collisions;

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
                actorA->GetAABBBox(), actorA->getWriteWorldMatrix(),
                actorB->GetAABBBox(), actorB->getWriteWorldMatrix()
            );

            if (info.bCollide)
            {
                info.rbA = rbA;
                info.rbB = rbB;

                collisions.push_back(info);
            }
        }
    }

    for (const CollisionInfo& info : collisions)
    {
        ResolveCollision(info.rbA, info.rbB, info); 
    }

}

void PhysicsSystem::ResolveCollision(RigidBodyComponent* a, RigidBodyComponent* b, const CollisionInfo& info)
{
    if (a->IsKinematic() && b->IsKinematic()) return;

    XMVECTOR relativeVel = a->GetVelocity();
    if (!b->IsKinematic())
        relativeVel = relativeVel - b->GetVelocity(); 

    float velAlongNormal = XMVectorGetX(XMVector3Dot(relativeVel, info.normal));

    XMFLOAT3 coutFloat;
    XMStoreFloat3(&coutFloat, info.normal);
    std::cout << "normal = " << coutFloat.x << " " << coutFloat.y << " " << coutFloat.z;
    std::cout << " depth = " << info.penetrationDepth << "\n";

    if (velAlongNormal > 0.0f) return; // 이미 멀어지고 있는 중이면 무시


    // 탄성 계수
    float restitution = 0.5f;

    float invMassA = a->IsKinematic() ? 0.0f : 1.0f / a->GetMass();
    float invMassB = b->IsKinematic() ? 0.0f : 1.0f / b->GetMass();

    float impulseMag = -(1.0f + restitution) * velAlongNormal;
    impulseMag /= (invMassA + invMassB);

    XMVECTOR impulse = info.normal * impulseMag;

    // impulse 적용 (속도 변화)
    if (!a->IsKinematic())
        a->ApplyImpulse(impulse);

    if (!b->IsKinematic())
        b->ApplyImpulse(-impulse);

    // --------------------------
    // 침투 보정해줘서 특정 액터가 다른 액터를 뚫고 지나갈 수 없도록 해준다
    // 그리고 이게 kinetic으로 이동시켜도 겹침을 막게 해주는 1등 공신이다
    // 이거 좀 보정해 보자
    // --------------------------

    const float percent = 0.1f; // 보정 강도 
    const float slop = 0.01f;   // 허용 침투 오차

    float penetration = max(info.penetrationDepth - slop, 0.0f);

    XMVECTOR correction = XMVectorScale(info.normal, penetration * percent / (invMassA + invMassB));

    if (!a->IsKinematic())
    {
        XMVECTOR corrected = a->GetPredictedPosition() + correction * invMassA;

        XMStoreFloat3(&coutFloat, corrected);
        a->SetPredictedPosition(corrected);
    }

    if (!b->IsKinematic())
    {
        XMVECTOR corrected = b->GetPredictedPosition() - correction * invMassB;
        b->SetPredictedPosition(corrected);
    }
    
}

CollisionInfo PhysicsSystem::CheckOBBCollision(
    const DirectX::BoundingBox& aBox, DirectX::XMMATRIX aWorldMat,
    const DirectX::BoundingBox& bBox, DirectX::XMMATRIX bWorldMat)
{
    OBB a = CreateOBBFromAABB(aBox, aWorldMat);
    OBB b = CreateOBBFromAABB(bBox, bWorldMat);

    CollisionInfo result;

    XMVECTOR centerGap = a.center - b.center;
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


OBB PhysicsSystem::CreateOBBFromAABB(const BoundingBox& box, const DirectX::XMMATRIX& world)
{
    OBB obb;
    obb.center = XMVector3Transform(XMLoadFloat3(&box.Center), world);
    obb.halfSize = XMLoadFloat3(&box.Extents);

    obb.axis[0] = XMVector3Normalize(XMVector3TransformNormal(XMVectorSet(1, 0, 0, 0), world)); 
    obb.axis[1] = XMVector3Normalize(XMVector3TransformNormal(XMVectorSet(0, 1, 0, 0), world)); 
    obb.axis[2] = XMVector3Normalize(XMVector3TransformNormal(XMVectorSet(0, 0, 1, 0), world)); 

    return obb;
}