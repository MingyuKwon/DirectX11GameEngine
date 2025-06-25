#include <DirectX11Run.h>
#include <EngineData.h>
#include <iostream>

DrawResource::DrawResource(ID3D11Device* device, vector<KMGVertex> vertices, vector<int> indices, XMMATRIX WorldMatrix) : device(device), vertices(vertices), indices(indices), WorldMatrix(WorldMatrix)
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

DrawResource::DrawResource(DrawResource&& resource) noexcept : vertices(move(resource.vertices)), indices(move(resource.indices))
{

    swap(device, resource.device);
    swap(vertexBuffer, resource.vertexBuffer);
    swap(indexBuffer, resource.indexBuffer);
}

DrawResource& DrawResource::operator=(DrawResource&& resource) noexcept
{
    cout << "Move Buffer in Draw Resource\n";

    if (this != &resource)
    {
        if (vertexBuffer) vertexBuffer->Release();
        if (indexBuffer) indexBuffer->Release();

        vertices = move(resource.vertices);
        indices = move(resource.indices);

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


void DeferredRenderThread::SceneWindowRender()
{

    // Clear the back buffer
    pd3dDeviceContext->ClearRenderTargetView(pRenderTargetView, Colors::MidnightBlue);

    // Clear the depth buffer to 1.0 (max depth)
    pd3dDeviceContext->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);


    // Set the input layout
    pd3dDeviceContext->IASetInputLayout(pVertexLayout);

    // Set primitive topology
    pd3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    pd3dDeviceContext->VSSetShader(pVertexShader, nullptr, 0);
    pd3dDeviceContext->VSSetConstantBuffers(0, 1, &pCBChangesEveryFrame);
    pd3dDeviceContext->VSSetConstantBuffers(1, 1, &pCBChangeOnResize);
    pd3dDeviceContext->PSSetShader(pPixelShader, nullptr, 0);

    // Set vertex buffer
    UINT stride = sizeof(KMGVertex);
    UINT offset = 0;

    pd3dDeviceContext->IASetVertexBuffers(0, 1, &pDrawResource->vertexBuffer, &stride, &offset);
    pd3dDeviceContext->IASetIndexBuffer(pDrawResource->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // 임시 고정 view 행렬
    XMVECTOR Eye = XMVectorSet(0.0f, 3.0f, -6.0f, 0.0f);
    XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMMATRIX View = XMMatrixLookAtLH(Eye, At, Up);

    // Update variables that change once per frame
    CBChangesEveryFrame cb = {};
    cb.mWorld = XMMatrixTranspose(pDrawResource->WorldMatrix);
    cb.mView = XMMatrixTranspose(View);
    pd3dDeviceContext->UpdateSubresource(pCBChangesEveryFrame, 0, nullptr, &cb, 0, 0);

    pd3dDeviceContext->DrawIndexed(pDrawResource->indices.size(), 0, 0);

}


HRESULT DeferredRenderThread::CreateConstBuffers()
{
    HRESULT hr = S_OK;

    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(CBChangeOnResize);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    hr = pd3dDevice->CreateBuffer(&bd, nullptr, &pCBChangeOnResize);
    if (FAILED(hr)) return hr;

    bd.ByteWidth = sizeof(CBChangesEveryFrame);
    hr = pd3dDevice->CreateBuffer(&bd, nullptr, &pCBChangesEveryFrame);
    if (FAILED(hr)) return hr;
}

DeferredRenderThread::~DeferredRenderThread()
{
    CleanupDeviceD3D();
}

DeferredRenderThread::DeferredRenderThread(ID3D11Device* pd3dDevice) : pd3dDevice(pd3dDevice)
{
    Initialize();
}

bool DeferredRenderThread::Initialize()
{
    pd3dDevice->AddRef(); // 참조 유지
    pd3dDevice->GetImmediateContext(&pd3dDeviceContext);

    CreateRenderTarget();
    return true;
}


void DeferredRenderThread::CreateRenderTarget()
{
    CleanupRenderTarget();

    HRESULT hr = S_OK;

    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width = screenWidth;
    texDesc.Height = screenHeight;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    ID3D11Texture2D* renderTexture = nullptr;

    hr = pd3dDevice->CreateTexture2D(&texDesc, nullptr, &renderTexture);
    if (FAILED(hr)) return;

    hr = pd3dDevice->CreateRenderTargetView(renderTexture, nullptr, &pRenderTargetView);
    if (FAILED(hr))
    {
        renderTexture->Release();
        renderTexture = nullptr;
        return;
    }

    hr = pd3dDevice->CreateShaderResourceView(renderTexture, nullptr, &pSceneSRV);
    if (FAILED(hr))
    {
        renderTexture->Release();
        renderTexture = nullptr;

        pRenderTargetView->Release();
        pRenderTargetView = nullptr;

        return;
    }

    if (renderTexture) {
        renderTexture->Release();
        renderTexture = nullptr;
    }


}

void DeferredRenderThread::CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (pd3dDeviceContext) { pd3dDeviceContext->Release(); pd3dDeviceContext = nullptr; }
    if (pd3dDevice) { pd3dDevice->Release(); pd3dDevice = nullptr; }

}


void DeferredRenderThread::CleanupRenderTarget()
{
    if (pRenderTargetView) 
    { 
        pRenderTargetView->Release(); 
        pRenderTargetView = nullptr; 
    }

    if (pSceneSRV)
    { 
        pSceneSRV->Release();
        pSceneSRV = nullptr;
    }


}

void DeferredRenderThread::DrawTexture()
{
    //ClearScreen();
}

void DeferredRenderThread::ClearScreen()
{
    pd3dDeviceContext->OMSetRenderTargets(1, &pRenderTargetView, nullptr);
    pd3dDeviceContext->ClearRenderTargetView(pRenderTargetView, Colors::Yellow);
}

void DeferredRenderThread::GetSRVTexture(ID3D11ShaderResourceView** outSRV)
{
    *outSRV = pSceneSRV;
}

void DeferredRenderThread::SetScreenSize(int width, int height)
{
    screenWidth.store(width);
    screenHeight.store(height);
}

void DeferredRenderThread::ResizeScreen()
{
    if (screenWidth == 0 || screenHeight == 0) return;
    CreateRenderTarget();

    screenWidth.store(0);
    screenHeight.store(0);
    
}
