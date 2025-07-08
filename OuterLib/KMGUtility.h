#pragma once
#include <KMGDataStructure.h>

namespace KMGUtility {
    DirectX::XMVECTOR GenerateCameraRayDirection(
        float clickX, float clickY,
        float viewportWidth, float viewportHeight,
        const DirectX::XMMATRIX& viewMatrix,
        const DirectX::XMMATRIX& projectionMatrix
        );

    std::string WStringToString(std::wstring wstr);
    std::wstring OpenFileDialog();

}