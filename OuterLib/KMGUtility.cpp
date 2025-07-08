#include <KMGUtility.h>
#include <DrawDebug.h>
#include <windows.h> // OutputDebugString 사용
#include <iostream>
#include <locale>
#include <codecvt>

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
