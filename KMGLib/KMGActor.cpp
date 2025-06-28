#include <KMGActor.h>

KMGActor::KMGActor(std::wstring name) : name(name)
{
}



std::wstring KMGActor::GetName()
{
    return name;
}

void KMGActor::UpdateWorldMatrix(DirectX::XMMATRIX newMatrix)
{
    WorldMatrix.exchange(newMatrix);
}

const std::vector<KMGVertex>& KMGActor::getVertices()
{
    return vertices;
}

const std::vector<int>& KMGActor::getIndices()
{
    return indices;
}

DirectX::XMMATRIX KMGActor::getWorldMatrix()
{
    return WorldMatrix;
}
