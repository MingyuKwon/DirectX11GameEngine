#include <KMGUtility.h>

using namespace DirectX;

DirectX::XMVECTOR KMGUtility::GenerateCameraRayDirection(
    float clickX, float clickY, 
    float viewportWidth, float viewportHeight, 
    const DirectX::XMMATRIX& viewMatrix, 
    const DirectX::XMMATRIX& projectionMatrix)
{
    float ndcX = (2.0f * clickX) / viewportWidth - 1.0f;
    float ndcY = 1.0f - (2.0f * clickY) / viewportHeight;

    XMVECTOR rayClip = XMVectorSet(ndcX, ndcY, 1.0f, 1.0f);

    XMMATRIX invProj = XMMatrixInverse(nullptr, projectionMatrix);
    XMVECTOR rayEye = XMVector3Transform(rayClip, invProj);
    rayEye = XMVectorSetZ(rayEye, 1.0f); // Make it a direction vector

    XMMATRIX invView = XMMatrixInverse(nullptr, viewMatrix);
    XMVECTOR rayWorld = XMVector3TransformNormal(rayEye, invView);
    rayWorld = XMVector3Normalize(rayWorld);

    return rayWorld;
}
