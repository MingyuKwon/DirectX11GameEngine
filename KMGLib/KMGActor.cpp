#include <KMGActor.h>
#include <iostream>

using namespace DirectX;

KMGActor::KMGActor(std::wstring name) : name(name)
{
}

std::wstring KMGActor::GetName()
{
    return name;
}

DirectX::XMMATRIX KMGActor::getWorldMatrix()
{
    return transform.GetWorldMatrix();
}

void KMGActor::Translate(float dx, float dy, float dz) {
    XMVECTOR pos = transform.position;
    XMVECTOR delta = XMVectorSet(dx, dy, dz, 0);
    transform.position = XMVectorAdd(pos, delta);
}

void KMGActor::Rotate(float dpitch, float dyaw, float droll) {
    XMVECTOR rot = transform.rotation;
    XMVECTOR delta = XMVectorSet(dpitch, dyaw, droll, 0);
    transform.rotation = XMVectorAdd(rot, delta);
}
