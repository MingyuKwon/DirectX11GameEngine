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
    DirectX::XMVECTOR rayDir,
    DirectX::XMVECTOR rayOrigin,
    DirectX::XMVECTOR focusActorPosition,
    DirectX::XMVECTOR aimVec)
{
    // 1. 평면 법선 구하기
    XMVECTOR directionVec = focusActorPosition - rayOrigin;

    // aimVec에 정사영
    XMVECTOR proj = XMVector3Dot(directionVec, aimVec) * aimVec;

    // 수직 성분만 추출하여 법선 생성
    XMVECTOR normal = XMVector3Normalize(directionVec - proj);

    // 2. Ray-Plane 교차 계산
    float denom = XMVectorGetX(XMVector3Dot(rayDir, normal));
    if (fabs(denom) < 1e-6f)
    {
        // 광선이 평면과 거의 평행
        return XMVectorZero();
    }

    XMVECTOR diff = focusActorPosition - rayOrigin;
    float t = XMVectorGetX(XMVector3Dot(diff, normal)) / denom;
    XMVECTOR hitPoint = rayOrigin + rayDir * t;

    // 3. 이동 벡터 계산 (focusActor → hitPoint)
    XMVECTOR delta = hitPoint - focusActorPosition;

    // delta를 aimVec에 투영
    float projAmount = XMVectorGetX(XMVector3Dot(delta, aimVec));
    XMVECTOR result = aimVec * projAmount;

    return result;
}

float NormalizeAngle360(float radian)
{
    float twoPi = XM_2PI;
    float result = std::fmod(radian, twoPi);
    if (result < 0) result += twoPi;
    return result;
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

DirectX::XMVECTOR KMGUtility::QuaternionToEulerXYZ(DirectX::XMVECTOR q)
{
    XMMATRIX rot = XMMatrixRotationQuaternion(q);

    float r11 = rot.r[0].m128_f32[0]; // m00
    float r12 = rot.r[0].m128_f32[1]; // m01
    float r13 = rot.r[0].m128_f32[2]; // m02

    float r21 = rot.r[1].m128_f32[0]; // m10
    float r22 = rot.r[1].m128_f32[1]; // m11
    float r23 = rot.r[1].m128_f32[2]; // m12

    float r31 = rot.r[2].m128_f32[0]; // m20
    float r32 = rot.r[2].m128_f32[1]; // m21
    float r33 = rot.r[2].m128_f32[2]; // m22

    float pitch, yaw, roll;

    if (fabsf(r31) < 1.0f - 1e-6f) // 안전한 Gimbal Lock 판별
    {
        yaw = atan2f(-r31, sqrtf(r11 * r11 + r21 * r21));
        pitch = atan2f(r32, r33);
        roll = atan2f(r21, r11);
    }
    else
    {
        yaw = (r31 <= -1.0f) ? XM_PIDIV2 : -XM_PIDIV2;

        pitch = atan2f(-r12, r22);
        roll = 0.0f;
    }

    yaw = NormalizeAngle360(yaw);

    return XMVectorSet(pitch, yaw, roll, 0.0f);
}

DirectX::XMVECTOR KMGUtility::QuaternionToClosestEulerXYZ(DirectX::XMVECTOR q, DirectX::XMVECTOR prevEuler)
{
    XMVECTOR raw = KMGUtility::QuaternionToEulerXYZ(q);
    float rx = XMVectorGetX(raw);
    float ry = XMVectorGetY(raw);
    float rz = XMVectorGetZ(raw);

    float px = XMVectorGetX(prevEuler);
    float py = XMVectorGetY(prevEuler);
    float pz = XMVectorGetZ(prevEuler);

    auto adjustAngle = [](float target, float reference) {
        float diff = target - reference;
        while (diff > XM_PI)  diff -= XM_2PI;
        while (diff < -XM_PI) diff += XM_2PI;
        return reference + diff;
        };

    // 원래 방식: 각도 차이 기반 보정
    float a_rx = adjustAngle(rx, px);
    float a_ry = adjustAngle(ry, py);
    float a_rz = adjustAngle(rz, pz);

    // 대안 해: raw 해 + 2PI 보정 (즉, 튄 해와 반대되는 해)
    float alt_rx = adjustAngle(rx + XM_PI, px);
    float alt_ry = adjustAngle(ry, py); // yaw는 그대로
    float alt_rz = adjustAngle(rz + XM_PI, pz);

    // 180 튀는 해가 선택되지 않도록 norm 비교
    float delta_orig = fabsf(a_rx - px) + fabsf(a_rz - pz);
    float delta_alt = fabsf(alt_rx - px) + fabsf(alt_rz - pz);

    if (delta_alt < delta_orig)
    {
        std::cout << "[ClosestEulerXYZ] 튐 보정 적용됨\n";
        return XMVectorSet(alt_rx, alt_ry, alt_rz, 0);
    }

    return XMVectorSet(a_rx, a_ry, a_rz, 0);
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
