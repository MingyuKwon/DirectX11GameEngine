#include <KMGUtility.h>
#include <DrawDebug.h>

using namespace DirectX;

DirectX::XMVECTOR KMGUtility::GenerateCameraRayDirection(
    float clickX, float clickY, 
    float viewportWidth, float viewportHeight, 
    const DirectX::XMMATRIX& viewMatrix, 
    const DirectX::XMMATRIX& projectionMatrix)
{
    // NDC 변환
    float ndcX = (2.0f * clickX) / viewportWidth - 1.0f;
    float ndcY = 1.0f - (2.0f * clickY) / viewportHeight;

    // Near & Far Clip Space 점
    DirectX::XMVECTOR rayClipNear = DirectX::XMVectorSet(ndcX, ndcY, 0.0f, 1.0f);
    DirectX::XMVECTOR rayClipFar = DirectX::XMVectorSet(ndcX, ndcY, 1.0f, 1.0f);

    // 역 변환
    DirectX::XMMATRIX invProj = XMMatrixInverse(nullptr, projectionMatrix);
    DirectX::XMMATRIX invView = XMMatrixInverse(nullptr, viewMatrix);

    DirectX::XMVECTOR rayEyeNear = XMVector3TransformCoord(rayClipNear, invProj);
    DirectX::XMVECTOR rayEyeFar = XMVector3TransformCoord(rayClipFar, invProj);

    DirectX::XMVECTOR rayWorldNear = XMVector3TransformCoord(rayEyeNear, invView);
    DirectX::XMVECTOR rayWorldFar = XMVector3TransformCoord(rayEyeFar, invView);


    XMFLOAT3 start, end;
    XMStoreFloat3(&start, rayWorldNear);
    XMStoreFloat3(&end, rayWorldFar);

    float delayTime = 2.f;
    DrawDebug::DrawLine(L"TestLine", start, end, XMFLOAT4(0, 0, 0, 1), delayTime);

    DrawDebug::DrawSphere(L"TestSphere", start, 0.3, XMFLOAT4(1, 0, 0, 1), delayTime);


    // 방향 = Far - Near
    DirectX::XMVECTOR rayDir = DirectX::XMVector3Normalize(rayWorldFar - rayWorldNear);

    return rayDir;

}
