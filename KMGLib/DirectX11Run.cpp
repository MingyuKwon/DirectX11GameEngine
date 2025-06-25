#include <DirectX11Run.h>
#include <EngineData.h>
#include <iostream>

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

    cout << "Release Buffer in Draw Resource\n";

}

DrawResource::DrawResource(DrawResource&& resource) noexcept : vertices(move(resource.vertices)), indices(move(resource.indices)), worldMatrix(resource.worldMatrix)
{
    swap(device, resource.device);
    swap(vertexBuffer, resource.vertexBuffer);
    swap(indexBuffer, resource.indexBuffer);
}

DrawResource& DrawResource::operator=(DrawResource&& resource) noexcept
{
    cout << "operator = Move Buffer in Draw Resource\n";

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

    cout << "Create Buffer in Draw Resource\n";

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


void RenderThread(
    vector<ID3D11CommandList*>& DX11CommandLists, mutex& dx11CommandMutex,
    ID3D11Device* pMainDevice, 
    ID3D11VertexShader* pVertexShader,
    ID3D11PixelShader* pPixelShader,
    ID3D11InputLayout* pVertexLayout,
    ID3D11RenderTargetView* pRTV, ID3D11DepthStencilView* pDSV,
    ID3D11Buffer* pCBChangeOnResize, ID3D11Buffer* pCBChangesEveryFrame,
    ID3D11Buffer* pVertexBuffer, ID3D11Buffer* pIdexBuffer,
    int drawIndexCount,
    const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix,
    int textureWidth, int textureHeight)
{
    ID3D11DeviceContext* pDeferredContext = nullptr;
    HRESULT hr = pMainDevice->CreateDeferredContext(0, &pDeferredContext);
    if (FAILED(hr)) {
        std::cout << "CreateDeferredContext failed: HRESULT = 0x"
            << std::hex << hr << std::endl;
    }

    pDeferredContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    pDeferredContext->IASetInputLayout(pVertexLayout);
    pDeferredContext->VSSetShader(pVertexShader, nullptr, 0);
    pDeferredContext->PSSetShader(pPixelShader, nullptr, 0);

    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = static_cast<float>(textureWidth);
    viewport.Height = static_cast<float>(textureHeight);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    pDeferredContext->RSSetViewports(1, &viewport);

    pDeferredContext->OMSetRenderTargets(1, &pRTV, pDSV);

    pDeferredContext->VSSetConstantBuffers(0, 1, &pCBChangesEveryFrame);
    pDeferredContext->VSSetConstantBuffers(1, 1, &pCBChangeOnResize);

    UINT stride = sizeof(KMGVertex);
    UINT offset = 0;

    pDeferredContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
    pDeferredContext->IASetIndexBuffer(pIdexBuffer, DXGI_FORMAT_R32_UINT, 0);

    CBChangesEveryFrame cb = {};
    cb.mWorld = XMMatrixTranspose(worldMatrix);
    cb.mView = XMMatrixTranspose(viewMatrix);
    pDeferredContext->UpdateSubresource(pCBChangesEveryFrame, 0, nullptr, &cb, 0, 0);

    pDeferredContext->DrawIndexed(drawIndexCount, 0, 0);

    ID3D11CommandList* pCmdList = nullptr;
    pDeferredContext->FinishCommandList(FALSE, &pCmdList);

    {
        std::lock_guard<std::mutex> lock(dx11CommandMutex);
        DX11CommandLists.push_back(pCmdList);
    }

    if (pDeferredContext)
    {
        pDeferredContext->Release();
        pDeferredContext = nullptr;
    }
}