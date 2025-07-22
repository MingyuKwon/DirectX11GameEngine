#pragma once
#include <KMGDataStructure.h>

namespace KMGUtility {
    DirectX::XMVECTOR GenerateCameraRayDirection(
        float clickX, float clickY,
        float viewportWidth, float viewportHeight,
        const DirectX::XMMATRIX& viewMatrix,
        const DirectX::XMMATRIX& projectionMatrix
        );


    DirectX::XMVECTOR GenerateMoveDeltaVector(
        DirectX::XMVECTOR rayDir,
        DirectX::XMVECTOR rayOrigin,
        DirectX::XMVECTOR focusActorPosition,
        DirectX::XMVECTOR aimVec);

    DirectX::XMVECTOR QuaternionToEulerXYZ(DirectX::XMVECTOR q);
    DirectX::XMVECTOR EulerXYZToQuaternion(DirectX::XMVECTOR euler);
    DirectX::XMVECTOR QuaternionToClosestEulerXYZ(
        DirectX::XMVECTOR q,
        DirectX::XMVECTOR prevEuler);

    std::string WStringToString(std::wstring wstr);
    std::wstring StringToWString(const std::string& str);
    
    std::wstring OpenFileDialog();


    CollisionInfo BuildOBBCollisionInfo(
        const OBB& a, const OBB& b,
        const float R[3][3],
        const float AbsR[3][3],
        const float t[3],
        const XMVECTOR& centerGap);


    inline float WrapAngleRad(float angle)
    {
        const float twoPi = DirectX::XM_2PI;

        if (angle < 0.0f)
            angle += twoPi;
        else if (angle > twoPi)
            angle -= twoPi;

        return angle;
    }

}