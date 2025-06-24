#include <DirectX11Run.h>
#include <EngineData.h>
#include <iostream>

// Tutorial에서 가져온 코드
//--------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DCompile
//
// With VS 11, we could load up prebuilt .cso files instead...
//--------------------------------------------------------------------------------------
HRESULT CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows
    // the shaders to be optimized and to run exactly the way they will run in
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;

    // Disable optimizations to further improve shader debugging
    dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    ID3DBlob* pErrorBlob = nullptr;
    hr = D3DCompileFromFile(szFileName, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, szEntryPoint, szShaderModel,
        dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
    if (FAILED(hr))
    {
        if (pErrorBlob)
        {
            OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
            MessageBoxA(nullptr, (char*)pErrorBlob->GetBufferPointer(), "Shader Compile Error", MB_OK);
            pErrorBlob->Release();
        }
        return hr;
    }
    if (pErrorBlob) pErrorBlob->Release();

    return S_OK;
}

DrawResource::DrawResource(ID3D11Device* device, vector<KMGVertex> vertices, vector<int> indices) : device(device), vertices(vertices), indices(indices)
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

DrawResource::DrawResource(DrawResource&& resource) noexcept : vertices(move(resource.vertices)), indices(move(resource.indices))
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


void DirectX11Wrapper::SceneWindowRender()
{
    // Update our time
    static float t = 0.0f;
    static ULONGLONG timeStart = 0;
    ULONGLONG timeCur = GetTickCount64();
    if (timeStart == 0)
        timeStart = timeCur;
    t = (timeCur - timeStart) / 1000.0f;


    // Rotate cube around the origin
    World = XMMatrixRotationY(t);

    // Clear the back buffer
    pImmediateContext->ClearRenderTargetView(pRenderTargetView, Colors::MidnightBlue);

    // Clear the depth buffer to 1.0 (max depth)
    pImmediateContext->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    // Update variables that change once per frame
    CBChangesEveryFrame cb = {};
    cb.mWorld = XMMatrixTranspose(World);
    pImmediateContext->UpdateSubresource(pCBChangesEveryFrame, 0, nullptr, &cb, 0, 0);

    // Set the input layout
    pImmediateContext->IASetInputLayout(pVertexLayout);

    // Set primitive topology
    pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    pImmediateContext->VSSetShader(pVertexShader, nullptr, 0);
    pImmediateContext->VSSetConstantBuffers(0, 1, &pCBNeverChanges);
    pImmediateContext->VSSetConstantBuffers(1, 1, &pCBChangeOnResize);
    pImmediateContext->VSSetConstantBuffers(2, 1, &pCBChangesEveryFrame);
    pImmediateContext->PSSetShader(pPixelShader, nullptr, 0);

    // Set vertex buffer
    UINT stride = sizeof(KMGVertex);
    UINT offset = 0;

    for (auto& bucket : drawResources)
    {
        DrawResource& resource = bucket.second;

        UINT stride = sizeof(KMGVertex);
        UINT offset = 0;
        pImmediateContext->IASetVertexBuffers(0, 1, &resource.vertexBuffer, &stride, &offset);
        pImmediateContext->IASetIndexBuffer(resource.indexBuffer, DXGI_FORMAT_R32_UINT, 0);
        pImmediateContext->DrawIndexed(resource.indices.size(), 0, 0);
    }

}


HRESULT D3D11Machine::CreateConstBuffers()
{
    HRESULT hr = S_OK;

    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(CBNeverChanges);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    hr = pd3dDevice->CreateBuffer(&bd, nullptr, &pCBNeverChanges);
    if (FAILED(hr)) return hr;

    bd.ByteWidth = sizeof(CBChangeOnResize);
    hr = pd3dDevice->CreateBuffer(&bd, nullptr, &pCBChangeOnResize);
    if (FAILED(hr)) return hr;

    bd.ByteWidth = sizeof(CBChangesEveryFrame);
    hr = pd3dDevice->CreateBuffer(&bd, nullptr, &pCBChangesEveryFrame);
    if (FAILED(hr)) return hr;

    // 임시 고정 view 행렬
    XMVECTOR Eye = XMVectorSet(0.0f, 3.0f, -6.0f, 0.0f);
    XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMMATRIX View = XMMatrixLookAtLH(Eye, At, Up);

    CBNeverChanges cbNeverChanges = {};
    cbNeverChanges.mView = XMMatrixTranspose(View);
    pd3dDeviceContext->UpdateSubresource(pCBNeverChanges, 0, nullptr, &cbNeverChanges, 0, 0);
}

HRESULT D3D11Machine::CompileShader(const WCHAR* vertexShaderName, const WCHAR* pixelShaderName)
{
    ID3DBlob* pVSBlob = nullptr;
    HRESULT hr = CompileShaderFromFile(vertexShaderName, "VS", "vs_4_0", &pVSBlob);
    if (FAILED(hr))
    {
        MessageBox(nullptr,
            L"The Vertex Shader file cannot be compiled.  Please run this executable from the directory that contains the Shader file.", L"Error", MB_OK);
        return hr;
    }

    hr = pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &pVertexShader);
    if (FAILED(hr))
    {
        pVSBlob->Release();
        return hr;
    }

    const D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = ARRAYSIZE(layout);

    hr = pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
        pVSBlob->GetBufferSize(), &pVertexLayout);
    pVSBlob->Release();
    if (FAILED(hr)) return hr;

    ID3DBlob* pPSBlob = nullptr;
    hr = CompileShaderFromFile(pixelShaderName, "PS", "ps_4_0", &pPSBlob);
    if (FAILED(hr))
    {
        MessageBox(nullptr,
            L"The Pixel Shader file cannot be compiled.  Please run this executable from the directory that contains the Shader file.", L"Error", MB_OK);
        return hr;
    }

    hr = pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &pPixelShader);
    pPSBlob->Release();
    if (FAILED(hr)) return hr;

    return hr;
}

D3D11Machine::~D3D11Machine()
{
    CleanupDeviceD3D();
}

D3D11Machine::D3D11Machine(EDirectXMode mode, ID3D11Device* pd3dDevice, HWND hWnd) : mode(mode), pd3dDevice(pd3dDevice)
{
    if (mode == EDirectXMode::EDXM_IMGUI)
    {
        Initialize(hWnd);
    }
    else if (mode == EDirectXMode::EDXM_TEXTURE)
    {
        Initialize();
    }
    
}

bool D3D11Machine::Initialize(HWND hWnd)
{
    if (pd3dDevice == nullptr) return false;

    pd3dDevice->AddRef();

    pd3dDevice->GetImmediateContext(&pd3dDeviceContext);

    IDXGIDevice* dxgiDevice = nullptr;
    HRESULT hr = pd3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);
    if (FAILED(hr) || dxgiDevice == nullptr) return false;

    IDXGIAdapter* adapter = nullptr;
    hr = dxgiDevice->GetAdapter(&adapter);
    dxgiDevice->Release();
    if (FAILED(hr) || adapter == nullptr) return false;

    IDXGIFactory* factory = nullptr;
    hr = adapter->GetParent(__uuidof(IDXGIFactory), (void**)&factory);
    adapter->Release();
    if (FAILED(hr) || factory == nullptr) return false;

    // 스왑체인 설정
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    // 스왑체인 생성
    hr = factory->CreateSwapChain(pd3dDevice, &sd, &pSwapChain);
    factory->Release();
    if (FAILED(hr)) return false;

    // 셰이더 및 버퍼 생성
    CreateConstBuffers();
    CompileShader(L"KMGLib\\VertexShader.hlsli", L"KMGLib\\PixelShader.hlsli");

    // 렌더 타겟 생성
    CreateRenderTarget();

    return true;
}


bool D3D11Machine::Initialize()
{
    pd3dDevice->AddRef(); // 참조 유지
    pd3dDevice->GetImmediateContext(&pd3dDeviceContext);

    CreateRenderTarget();
    return true;
}


void D3D11Machine::CreateRenderTarget()
{
    CleanupRenderTarget();

    HRESULT hr = S_OK;

    if (mode == EDirectXMode::EDXM_IMGUI)
    {
        pSwapChain->ResizeBuffers(0, screenWidth, screenHeight, DXGI_FORMAT_UNKNOWN, 0);

        ID3D11Texture2D* pBackBuffer;
        pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
        pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pRenderTargetView);
        pBackBuffer->Release();
    }
    else if (mode == EDirectXMode::EDXM_TEXTURE)
    {
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

        HRESULT hr = S_OK;
        hr = pd3dDevice->CreateTexture2D(&texDesc, nullptr, &renderTexture);
        if (FAILED(hr)) return;

        hr = pd3dDevice->CreateRenderTargetView(renderTexture, nullptr, &pRenderTargetView);
        if (FAILED(hr))
        {
            renderTexture->Release();
            renderTexture = nullptr;
            return;
        }

        hr = pd3dDevice->CreateShaderResourceView(renderTexture, nullptr, &pTextureSRV);
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

}

void D3D11Machine::CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (pSwapChain) { pSwapChain->Release(); pSwapChain = nullptr; }
    if (pd3dDeviceContext) { pd3dDeviceContext->Release(); pd3dDeviceContext = nullptr; }
    if (pd3dDevice) { pd3dDevice->Release(); pd3dDevice = nullptr; }

}


void D3D11Machine::CleanupRenderTarget()
{
    if (pRenderTargetView) 
    { 
        pRenderTargetView->Release(); 
        pRenderTargetView = nullptr; 
    }

    if (pTextureSRV) 
    { 
        pTextureSRV->Release(); 
        pTextureSRV = nullptr;
    }


}

void D3D11Machine::DrawTexture()
{
    //ClearScreen();
}

void D3D11Machine::ClearScreen()
{
    pd3dDeviceContext->OMSetRenderTargets(1, &pRenderTargetView, nullptr);
    pd3dDeviceContext->ClearRenderTargetView(pRenderTargetView, Colors::Yellow);
}

HRESULT D3D11Machine::TryPresent()
{
    if (!bCanDrawUI) return S_FALSE;
    pSwapChain->Present(0, 0);
    return S_OK;
}

HRESULT D3D11Machine::SetDrawReady()
{
    bCanDrawUI.exchange(true);
    return S_OK;
}

void D3D11Machine::GetD3DDeviceContext(ID3D11Device** outDevice, ID3D11DeviceContext** outContext)
{
    *outDevice = pd3dDevice;
    *outContext = pd3dDeviceContext;

}

void D3D11Machine::GetSRVTexture(ID3D11ShaderResourceView** outSRV)
{
    *outSRV = pTextureSRV;
}

void D3D11Machine::SetScreenSize(int width, int height)
{
    screenWidth.store(width);
    screenHeight.store(height);
}

void D3D11Machine::ResizeScreen()
{
    if (screenWidth == 0 || screenHeight == 0) return;
    CreateRenderTarget();

    screenWidth.store(0);
    screenHeight.store(0);
    
}

HRESULT D3D11Machine::AddActor(const KMGActor& actor)
{
    if (drawResources.count(actor.name) > 0) return S_FALSE;
    if (!pd3dDevice) return S_FALSE;

    drawResources[actor.name] = DrawResource(pd3dDevice, actor.vertices, actor.indices);

    return S_OK;
}

HRESULT D3D11Machine::deleteActor(wstring name)
{
    if (!drawResources.count(name) == 0) return S_FALSE;

    drawResources.erase(name);

    return S_OK;
}