#include "KMGCamera.h"
#include <iostream>

using namespace DirectX;

void KMGCamera::SetForwardVector(DirectX::XMVECTOR inForwardVector)
{
    const float maxPitchY = 0.99f;

    XMVECTOR forward = XMVector3Normalize(inForwardVector);

    float y = XMVectorGetY(forward);

    if (y > maxPitchY || y < -maxPitchY) return;

    forwardVector = forward;

    XMVECTOR worldUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMVECTOR right = XMVector3Cross(worldUp, forward);

    rightVector = XMVector3Normalize(right);
    upVector = XMVector3Normalize(XMVector3Cross(forward, right));

}
