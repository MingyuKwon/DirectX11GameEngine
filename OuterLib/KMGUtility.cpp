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

DirectX::XMVECTOR KMGUtility::GenerateScreenDeltaVector(
    float prevMousePosX, float prevMousePosY,
    float MousePosX, float MousePosY,
    float viewportWidth, float viewportHeight,
    const DirectX::XMMATRIX& viewMatrix,
    const DirectX::XMMATRIX& projectionMatrix)
{
    // 1. 마우스 → NDC 좌표
    float prevNdcX = (2.0f * prevMousePosX) / viewportWidth - 1.0f;
    float prevNdcY = 1.0f - (2.0f * prevMousePosY) / viewportHeight;

    float currNdcX = (2.0f * MousePosX) / viewportWidth - 1.0f;
    float currNdcY = 1.0f - (2.0f * MousePosY) / viewportHeight;

    // 2. Clip Space 좌표 (Z=0은 Near Plane)
    DirectX::XMVECTOR clipPrev = DirectX::XMVectorSet(prevNdcX, prevNdcY, 0.0f, 1.0f);
    DirectX::XMVECTOR clipCurr = DirectX::XMVectorSet(currNdcX, currNdcY, 0.0f, 1.0f);

    // 3. 역투영 행렬
    DirectX::XMMATRIX invProj = DirectX::XMMatrixInverse(nullptr, projectionMatrix);
    DirectX::XMMATRIX invView = DirectX::XMMatrixInverse(nullptr, viewMatrix);

    // 4. Clip → Eye → World
    DirectX::XMVECTOR eyePrev = DirectX::XMVector3TransformCoord(clipPrev, invProj);
    DirectX::XMVECTOR eyeCurr = DirectX::XMVector3TransformCoord(clipCurr, invProj);

    DirectX::XMVECTOR worldPrev = DirectX::XMVector3TransformCoord(eyePrev, invView);
    DirectX::XMVECTOR worldCurr = DirectX::XMVector3TransformCoord(eyeCurr, invView);

    // 5. 이동 방향 (벡터 차)

    XMFLOAT3 worldFloatPrev, worldFloat;
    XMStoreFloat3(&worldFloatPrev, worldPrev);
    XMStoreFloat3(&worldFloat, worldCurr);

    DRAW_DEBUG_LINE(worldFloatPrev, worldFloat, XMFLOAT4(1,0,0,0), 0);

    return worldCurr - worldPrev;
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
