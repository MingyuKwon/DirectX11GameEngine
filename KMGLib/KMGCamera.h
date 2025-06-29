#pragma once
#include <KMGDataStructure.h>

class KMGCamera 
{
public:
    inline DirectX::XMMATRIX GetViewMatrix() { return DirectX::XMMatrixLookAtLH(CameraPosition, TargetPosition, Up); }

    inline DirectX::XMVECTOR GetCameraPosition() { return CameraPosition; }
    inline void SetCameraPosition(DirectX::XMVECTOR inCameraPosition) { CameraPosition = inCameraPosition; }

    inline DirectX::XMVECTOR GetTargetPosition() { return TargetPosition; }
    inline void SetTargetPosition(DirectX::XMVECTOR inTargetPosition) { TargetPosition = inTargetPosition; }

private:
    DirectX::XMVECTOR CameraPosition = DirectX::XMVectorSet(0.0f, 2.0f, -6.0f, 0.0f);
    DirectX::XMVECTOR TargetPosition = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    DirectX::XMVECTOR Up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
};