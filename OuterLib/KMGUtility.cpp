#include <KMGUtility.h>
#include <DrawDebug.h>
#include <windows.h> // OutputDebugString 사용
#include <iostream>
#include <locale>
#include <codecvt>
#include <DrawDebug.h> 

// 로그 출력 함수
void PrintVector(const std::wstring& label, const DirectX::XMVECTOR& vec)
{
    DirectX::XMFLOAT3 f3;
    XMStoreFloat3(&f3, vec);

    std::wcout << label << L": (" << f3.x << L", " << f3.y << L", " << f3.z << L")\n";
}
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

    // 방향 = Far - Near
    DirectX::XMVECTOR rayDir = DirectX::XMVector3Normalize(rayWorldFar - rayWorldNear);

    return rayDir;

}

DirectX::XMVECTOR KMGUtility::GenerateMoveDeltaVector(
    DirectX::XMVECTOR rayDIr,
    DirectX::XMVECTOR rayOrigin,
    DirectX::XMVECTOR focusActorPosition,
    DirectX::XMVECTOR normalVec,
    DirectX::XMVECTOR aimVec)
{
    // 1. Ray-Plane 교차 계산
    float denom = XMVectorGetX(DirectX::XMVector3Dot(rayDIr, normalVec));

    if (fabs(denom) < 1e-6f)
    {
        // 평면과 거의 평행 → fallback: 이동 없음
        return DirectX::XMVectorZero();
    }

    DirectX::XMVECTOR diff = focusActorPosition - rayOrigin;
    float t = XMVectorGetX(DirectX::XMVector3Dot(diff, normalVec)) / denom;
    DirectX::XMVECTOR hitPoint = rayOrigin + rayDIr * t;

    // 2. hitPoint에서 actor까지 벡터
    DirectX::XMVECTOR delta = hitPoint - focusActorPosition;

    // 3. delta를 aimVec(예: X축)으로 투영
    float projAmount = XMVectorGetX(DirectX::XMVector3Dot(delta, aimVec));
    DirectX::XMVECTOR result = aimVec * projAmount;

    return result;


}

DirectX::XMVECTOR KMGUtility::QuaternionToEulerXYZ(DirectX::XMVECTOR q)
{
    // 쿼터니언 → 회전 행렬
    XMMATRIX rot = XMMatrixRotationQuaternion(q);

    // 회전 행렬에서 요소 추출
    float r11 = rot.r[0].m128_f32[0]; // m00
    float r21 = rot.r[1].m128_f32[0]; // m10
    float r31 = rot.r[2].m128_f32[0]; // m20
    float r32 = rot.r[2].m128_f32[1]; // m21
    float r33 = rot.r[2].m128_f32[2]; // m22

    // 회전 순서: Y (yaw), X (pitch), Z (roll)
    float yaw = asinf(-r31);               // Y축 회전
    float pitch = atan2f(r32, r33);          // X축 회전
    float roll = atan2f(r21, r11);          // Z축 회전

    return XMVectorSet(pitch, yaw, roll, 0.0f); // 출력: X (pitch), Y (yaw), Z (roll)
}

DirectX::XMVECTOR KMGUtility::EulerXYZToQuaternion(DirectX::XMVECTOR euler)
{
    // euler = (pitch, yaw, roll, 0)
    float pitch = XMVectorGetX(euler); // X축 회전
    float yaw = XMVectorGetY(euler); // Y축 회전
    float roll = XMVectorGetZ(euler); // Z축 회전

    // 각 축 회전에 대한 쿼터니언 생성
    XMVECTOR qYaw = XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), yaw);   // Y
    XMVECTOR qPitch = XMQuaternionRotationAxis(XMVectorSet(1, 0, 0, 0), pitch); // X
    XMVECTOR qRoll = XMQuaternionRotationAxis(XMVectorSet(0, 0, 1, 0), roll);  // Z

    // 회전 순서: Y → X → Z 이므로 곱셈 순서 주의
    // 최종 회전 쿼터니언 = qRoll * qPitch * qYaw
    // 오른쪽부터 먼저 적용됨
    XMVECTOR q = XMQuaternionMultiply(qRoll, XMQuaternionMultiply(qPitch, qYaw));

    // 정규화하여 반환
    return XMQuaternionNormalize(q);
}

std::string KMGUtility::WStringToString(std::wstring wstr)
{
    static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.to_bytes(wstr);
}

std::wstring KMGUtility::StringToWString(const std::string& str)
{
    if (str.empty()) return {};

    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    std::wstring result(size_needed - 1, 0); // -1: null 제외
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &result[0], size_needed);
    return result;
}

std::wstring KMGUtility::OpenFileDialog()
{
    wchar_t filename[MAX_PATH] = L"";

    OPENFILENAMEW ofn = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFilter = L"All Files\0*.*\0Text Files\0*.txt\0";
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

    if (GetOpenFileNameW(&ofn))
    {
        return filename;
    }

    return L"";
}
