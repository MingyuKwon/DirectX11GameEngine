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
        DirectX::XMVECTOR rayDIr,
        DirectX::XMVECTOR rayOrigin,
        DirectX::XMVECTOR focusActorPosition,
        DirectX::XMVECTOR normalVec,
        DirectX::XMVECTOR aimVec
        );

    DirectX::XMVECTOR QuaternionToEulerXYZ(DirectX::XMVECTOR q);
    DirectX::XMVECTOR EulerXYZToQuaternion(DirectX::XMVECTOR euler);


    std::string WStringToString(std::wstring wstr);
    std::wstring StringToWString(const std::string& str);
    
    std::wstring OpenFileDialog();

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