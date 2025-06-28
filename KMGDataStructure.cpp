#include <KMGDataStructure.h>

using namespace std;
using namespace DirectX;

DrawResource::DrawResource(ID3D11Device* device, vector<KMGVertex> vertices, vector<int> indices, XMMATRIX WorldMatrix) : device(device), vertices(vertices), indices(indices), worldMatrix(WorldMatrix)
{
    CreateBuffers();
}

DrawResource::~DrawResource()
{
    if (vertexBuffer)
    {
        vertexBuffer->Release();
        vertexBuffer = nullptr;
    }

    if (indexBuffer)
    {
        indexBuffer->Release();
        indexBuffer = nullptr;
    }

}

DrawResource::DrawResource(DrawResource&& resource) noexcept : vertices(move(resource.vertices)), indices(move(resource.indices)), worldMatrix(resource.worldMatrix)
{
    swap(device, resource.device);
    swap(vertexBuffer, resource.vertexBuffer);
    swap(indexBuffer, resource.indexBuffer);
}

DrawResource& DrawResource::operator=(DrawResource&& resource) noexcept
{
    if (this != &resource)
    {
        if (vertexBuffer) vertexBuffer->Release();
        if (indexBuffer) indexBuffer->Release();

        vertices = move(resource.vertices);
        indices = move(resource.indices);
        worldMatrix = move(resource.worldMatrix);

        swap(device, resource.device);
        swap(vertexBuffer, resource.vertexBuffer);
        swap(indexBuffer, resource.indexBuffer);
    }

    return *this;
}

void DrawResource::CreateBuffers()
{
    if (!device) return;

    D3D11_BUFFER_DESC vbd = {};
    vbd.Usage = D3D11_USAGE_DEFAULT;
    vbd.ByteWidth = sizeof(KMGVertex) * (UINT)vertices.size();
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vinitData = {};
    vinitData.pSysMem = vertices.data();

    device->CreateBuffer(&vbd, &vinitData, &vertexBuffer);

    // Index Buffer
    D3D11_BUFFER_DESC ibd = {};
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.ByteWidth = sizeof(UINT) * (UINT)indices.size();
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA iinitData = {};
    iinitData.pSysMem = indices.data();

    device->CreateBuffer(&ibd, &iinitData, &indexBuffer);
}
