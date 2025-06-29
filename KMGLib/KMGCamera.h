#pragma once
#include <KMGDataStructure.h>
#include <iostream>

class KMGCamera 
{
public:
    inline DirectX::XMMATRIX GetViewMatrix() { return DirectX::XMMatrixLookAtLH(CameraPosition, DirectX::XMVectorAdd(CameraPosition, ForwardVector), Up); }

    inline DirectX::XMVECTOR GetCameraPosition() { return CameraPosition; }
    inline void SetCameraPosition(DirectX::XMVECTOR inCameraPosition) 
    { 
        DirectX::XMFLOAT3 debugOut;
        DirectX::XMStoreFloat3(&debugOut, inCameraPosition); 

        std::cout << "CameraPosition: ("
            << debugOut.x << ", "
            << debugOut.y << ", "
            << debugOut.z << ")\n";

        CameraPosition = inCameraPosition;
    }

    inline DirectX::XMVECTOR GetForwardVector() { return ForwardVector; }
    inline void SetForwardVector(DirectX::XMVECTOR inForwardVector) { ForwardVector = inForwardVector; }

private:
    DirectX::XMVECTOR CameraPosition = DirectX::XMVectorSet(0.0f, 2.0f, -6.0f, 0.0f);
    DirectX::XMVECTOR ForwardVector = DirectX::XMVectorSet(0.0f, -2.0f, 6.0f, 0.0f);
    DirectX::XMVECTOR Up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
};