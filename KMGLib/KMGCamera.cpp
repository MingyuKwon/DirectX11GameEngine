#include "KMGCamera.h"

using namespace DirectX;

void KMGCamera::SetForwardVector(DirectX::XMVECTOR inForwardVector)
{
    const float maxPitchY = 0.99f;

    // 1. 정규화
    XMVECTOR forward = XMVector3Normalize(inForwardVector);

    // 2. Y값을 추출해서 제한
    float y = XMVectorGetY(forward);

    if (y > maxPitchY || y < -maxPitchY)
    {
        // Y값을 제한 (clamp)
        float x = XMVectorGetX(forward);
        float z = XMVectorGetZ(forward);

        // XZ 평면에서 방향 유지하면서 Y값 제한
        XMVECTOR flatForward = XMVector3Normalize(XMVectorSet(x, 0.0f, z, 0.0f));

        forward = XMVectorSet(
            XMVectorGetX(flatForward),
            (y > 0.0f ? maxPitchY : -maxPitchY),
            XMVectorGetZ(flatForward),
            0.0f
        );

        // 다시 정규화
        forward = XMVector3Normalize(forward);
    }

    forwardVector = forward;

    // 3. Up, Right 재계산
    XMVECTOR worldUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMVECTOR right = XMVector3Cross(worldUp, forward);

    if (XMVector3Equal(right, XMVectorZero()) || XMVectorGetX(XMVector3LengthSq(right)) < 1e-5f)
    {
        worldUp = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
        right = XMVector3Cross(worldUp, forward);
    }

    rightVector = XMVector3Normalize(right);
    upVector = XMVector3Normalize(XMVector3Cross(forward, right));

}
