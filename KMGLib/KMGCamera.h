#pragma once
#include <KMGDataStructure.h>
#include <iostream>

class KMGCamera 
{
public:
    inline KMGCamera()
    {
        SetForwardVector(forwardVector);
    }

    inline DirectX::XMMATRIX GetViewMatrix() { return DirectX::XMMatrixLookAtLH(CameraPosition, DirectX::XMVectorAdd(CameraPosition, forwardVector), upVector); }

    inline DirectX::XMVECTOR GetCameraPosition() { return CameraPosition; }
    inline void SetCameraPosition(DirectX::XMVECTOR inCameraPosition) 
    { 
        CameraPosition = inCameraPosition;
    }

    void SetForwardVector(DirectX::XMVECTOR inForwardVector);
    

    inline DirectX::XMVECTOR GetForwardVector() { return forwardVector; }
    inline DirectX::XMVECTOR GetUpVector() { return upVector; }
    inline DirectX::XMVECTOR GetRightVector() { return rightVector; }

private:
    DirectX::XMVECTOR CameraPosition = DirectX::XMVectorSet(0.0f, 2.0f, -10.0f, 0.0f);

    DirectX::XMVECTOR forwardVector = DirectX::XMVectorSet(0.0f, -2.0f, 6.0f, 0.0f);
    DirectX::XMVECTOR upVector = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    DirectX::XMVECTOR rightVector = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
};