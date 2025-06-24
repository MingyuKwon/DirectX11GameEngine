#include <DirectX11Run.h>
#include <EngineData.h>

HRESULT CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);

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


DirectX11Wrapper::DirectX11Wrapper(HWND mainWindow, int width, int height) : mainWindow(mainWindow)
{
    InitDirectX11(width, height);
}

DirectX11Wrapper::~DirectX11Wrapper()
{
    CleanupDevice();
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

HRESULT DirectX11Wrapper::TryPresent()
{
    if (!bCanDrawUI.exchange(false)) return S_FALSE;

    pSwapChain->Present(0, 0);
    return S_OK;
}

HRESULT DirectX11Wrapper::SetDrawReady()
{
    bCanDrawUI.exchange(true);
    return S_OK;
}

void DirectX11Wrapper::GetD3DDeviceContext(ID3D11Device** outDevice, ID3D11DeviceContext** outContext)
{
    *outDevice = pd3dDevice;
    *outContext = pImmediateContext;
}


//--------------------------------------------------------------------------------------
// 전역 변수 값 최초 할당
//--------------------------------------------------------------------------------------
HRESULT DirectX11Wrapper::InitDirectX11(int width, int height)
{
    HRESULT hr = S_OK;

    hr = Init_Device_Context();
    if (FAILED(hr)) return hr;

    hr = Init_RTV_DSV_Viewport(width, height);
    if (FAILED(hr)) return hr;
    
    hr = CompileShader(L"KMGLib\\VertexShader.hlsli", L"KMGLib\\PixelShader.hlsli");
    if (FAILED(hr)) return hr;

    CreateConstBuffers();
    // Initialize the world matrices


    return S_OK;
}

HRESULT DirectX11Wrapper::CreateConstBuffers()
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
    View = XMMatrixLookAtLH(Eye, At, Up);

    CBNeverChanges cbNeverChanges = {};
    cbNeverChanges.mView = XMMatrixTranspose(View);
    pImmediateContext->UpdateSubresource(pCBNeverChanges, 0, nullptr, &cbNeverChanges, 0, 0);
}

HRESULT DirectX11Wrapper::Init_Device_Context()
{
    HRESULT hr = S_OK;
    UINT createDeviceFlags = 0;

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    D3D_DRIVER_TYPE                     g_driverType = D3D_DRIVER_TYPE_NULL;
    D3D_FEATURE_LEVEL                   g_featureLevel = D3D_FEATURE_LEVEL_11_0;

    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        g_driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDevice(nullptr, g_driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
            D3D11_SDK_VERSION, &pd3dDevice, &g_featureLevel, &pImmediateContext);

        if (hr == E_INVALIDARG)
        {
            // DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
            hr = D3D11CreateDevice(nullptr, g_driverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
                D3D11_SDK_VERSION, &pd3dDevice, &g_featureLevel, &pImmediateContext);
        }

        if (SUCCEEDED(hr))
            break;
    }

    return hr;
}

HRESULT DirectX11Wrapper::Init_RTV_DSV_Viewport(int width, int height)
{
    HRESULT hr = S_OK;

    IDXGIFactory1* dxgiFactory = nullptr;
    {
        IDXGIDevice* dxgiDevice = nullptr;
        hr = pd3dDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
        if (SUCCEEDED(hr))
        {
            IDXGIAdapter* adapter = nullptr;
            hr = dxgiDevice->GetAdapter(&adapter);
            if (SUCCEEDED(hr))
            {
                hr = adapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgiFactory));
                adapter->Release();
            }
            dxgiDevice->Release();
        }
    }
    if (FAILED(hr)) return hr;
        
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 1;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = mainWindow;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    hr = dxgiFactory->CreateSwapChain(pd3dDevice, &sd, &pSwapChain);
    dxgiFactory->Release();
    if (FAILED(hr)) return hr;
       
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
    if (FAILED(hr)) return hr;
        
    hr = pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pRenderTargetView);
    pBackBuffer->Release();
    if (FAILED(hr)) return hr;

    D3D11_TEXTURE2D_DESC descDepth = {};
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    hr = pd3dDevice->CreateTexture2D(&descDepth, nullptr, &pDepthStencil);
    if (FAILED(hr)) return hr;

    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hr = pd3dDevice->CreateDepthStencilView(pDepthStencil, &descDSV, &pDepthStencilView);
    if (FAILED(hr)) return hr;

    // Depth도 빼야 하나?

    pImmediateContext->OMSetRenderTargets(1, &pRenderTargetView, pDepthStencilView);

    D3D11_VIEWPORT vp = {};
    vp.Width = static_cast<FLOAT>(width);
    vp.Height = static_cast<FLOAT>(height);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    pImmediateContext->RSSetViewports(1, &vp);
        
    return hr;
}

HRESULT DirectX11Wrapper::CompileShader(const WCHAR* vertexShaderName, const WCHAR* pixelShaderName)
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


//--------------------------------------------------------------------------------------
// 전역 변수 값 전부 초기화
//--------------------------------------------------------------------------------------
void DirectX11Wrapper::CleanupDevice()
{
    if (pImmediateContext) pImmediateContext->ClearState();
    if (pSamplerLinear) pSamplerLinear->Release();
    if (pTextureRV) pTextureRV->Release();
    if (pCBNeverChanges) pCBNeverChanges->Release();
    if (pCBChangeOnResize) pCBChangeOnResize->Release();
    if (pCBChangesEveryFrame) pCBChangesEveryFrame->Release();
    if (pVertexBuffer) pVertexBuffer->Release();
    if (pIndexBuffer) pIndexBuffer->Release();
    if (pVertexLayout) pVertexLayout->Release();
    if (pVertexShader) pVertexShader->Release();
    if (pPixelShader) pPixelShader->Release();
    if (pDepthStencil) pDepthStencil->Release();
    if (pDepthStencilView) pDepthStencilView->Release();
    if (pRenderTargetView) pRenderTargetView->Release();
    if (pSwapChain) pSwapChain->Release();
    if (pImmediateContext) pImmediateContext->Release();
    if (pd3dDevice) pd3dDevice->Release();
}



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

D3D11Machine::~D3D11Machine()
{
    CleanupDeviceD3D();
}

D3D11Machine::D3D11Machine(EDirectXMode mode, HWND hWnd) : mode(mode)
{
    if (mode == EDirectXMode::EDXM_IMGUI)
    {
        CreateDeviceD3D(hWnd);
    }
    else if (mode == EDirectXMode::EDXM_TEXTURE)
    {
        CreateDeviceD3D();
    }
    
}

bool D3D11Machine::CreateDeviceD3D(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
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

    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };

    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &pSwapChain, &pd3dDevice, &featureLevel, &pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &pSwapChain, &pd3dDevice, &featureLevel, &pd3dDeviceContext);
    if (res != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

bool D3D11Machine::CreateDeviceD3D()
{
    // 1. 디바이스와 디바이스 컨텍스트 생성
    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0
    };
    D3D_FEATURE_LEVEL createdFeatureLevel;

    HRESULT hr = D3D11CreateDevice(
        nullptr,                       
        D3D_DRIVER_TYPE_HARDWARE,      
        nullptr,                        
        createDeviceFlags,             
        featureLevels,                  
        ARRAYSIZE(featureLevels),
        D3D11_SDK_VERSION,
        &pd3dDevice,                   
        &createdFeatureLevel,           
        &pd3dDeviceContext              
    );

    if (FAILED(hr)) {
        return false;
    }

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

void D3D11Machine::ClearScreen()
{
    pd3dDeviceContext->OMSetRenderTargets(1, &pRenderTargetView, nullptr);
    pd3dDeviceContext->ClearRenderTargetView(pRenderTargetView, Colors::Black);
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