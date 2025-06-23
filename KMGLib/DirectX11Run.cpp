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


DirectX11Wrapper::DirectX11Wrapper(HWND mainWindow, HWND sceneWindow) : mainWindow(mainWindow), sceneWindow(sceneWindow)
{
    InitDirectX11();
}

DirectX11Wrapper::~DirectX11Wrapper()
{
    CleanupDevice();
}

HRESULT DirectX11Wrapper::AddActor(const KMGActor& actor)
{
    if(drawResources.count(actor.name) > 0) return S_FALSE;
    if(!pd3dDevice) return S_FALSE;

    drawResources[actor.name] = DrawResource(pd3dDevice, actor.vertices, actor.indices);

    return S_OK;
}

HRESULT DirectX11Wrapper::deleteActor(wstring name)
{
    if (!drawResources.count(name) == 0) return S_FALSE;

    drawResources.erase(name);

    return S_OK;
}

void DirectX11Wrapper::SceneWindowRender()
{
    // Update our time
    static float t = 0.0f;
    if (g_driverType == D3D_DRIVER_TYPE_REFERENCE)
    {
        t += (float)XM_PI * 0.0125f;
    }
    else
    {
        static ULONGLONG timeStart = 0;
        ULONGLONG timeCur = GetTickCount64();
        if (timeStart == 0)
            timeStart = timeCur;
        t = (timeCur - timeStart) / 1000.0f;
    }

    // Rotate cube around the origin
    World = XMMatrixRotationY(t);

    // Clear the back buffer
    pImmediateContext->ClearRenderTargetView(pMainRenderTargetView, Colors::MidnightBlue);

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

HRESULT DirectX11Wrapper::TryUIPresent()
{
    if (!bCanDrawUI.exchange(false)) return S_FALSE;

    pMainSwapChain->Present(0, 0);
    return S_OK;
}

HRESULT DirectX11Wrapper::SetUIDrawReady()
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
HRESULT DirectX11Wrapper::InitDirectX11()
{
    HRESULT hr = S_OK;

    hr = Init_Device_Context();
    if (FAILED(hr)) return hr;

    hr = Init_RTV_DSV_Viewport(pMainSwapChain, pMainRenderTargetView, currentWindowWidth, currentWindowHeight);
    if (FAILED(hr)) return hr;

    int initialSceneWidth = currentWindowWidth;
    int initialSceneHeight = currentWindowHeight;

    hr = Init_RTV_DSV_Viewport(pSceneSwapChain, pSceneRenderTargetView, initialSceneWidth, initialSceneHeight);
    if (FAILED(hr)) return hr;
    
    hr = CompileShader(L"KMGLib\\VertexShader.hlsli", L"KMGLib\\PixelShader.hlsli");
    if (FAILED(hr)) return hr;

    CreateConstBuffers();

    ResizeViewtarget(currentWindowWidth, currentWindowHeight);

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

HRESULT DirectX11Wrapper::Init_RTV_DSV_Viewport(IDXGISwapChain*& pSwapChain, ID3D11RenderTargetView*& pRenderTargetView, int width, int height)
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

//--------------------------------------------------------------------------------------
// 주어진 너비와 높이로 뷰 타깃과 깊이 버퍼, 뷰 포트의 크기를 바꿈
//--------------------------------------------------------------------------------------
void DirectX11Wrapper::ResizeViewtarget(int width, int height)
{
    if (!pMainRenderTargetView || !pDepthStencilView || !pDepthStencil) return;

    HRESULT hr = S_OK;

    pMainRenderTargetView->Release();
    pDepthStencilView->Release();
    pDepthStencil->Release();

    hr = pMainSwapChain->ResizeBuffers(
        1,
        width,
        height,
        DXGI_FORMAT_R8G8B8A8_UNORM,
        0);
    if (FAILED(hr)) return;

    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = pMainSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
    if (FAILED(hr)) return;

    hr = pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pMainRenderTargetView);
    pBackBuffer->Release();
    if (FAILED(hr)) return;

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
    if (FAILED(hr)) return;

    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hr = pd3dDevice->CreateDepthStencilView(pDepthStencil, &descDSV, &pDepthStencilView);
    if (FAILED(hr)) return;

    pImmediateContext->OMSetRenderTargets(1, &pMainRenderTargetView, pDepthStencilView);

    D3D11_VIEWPORT vp = {};
    vp.Width = static_cast<FLOAT>(width);
    vp.Height = static_cast<FLOAT>(height);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    pImmediateContext->RSSetViewports(1, &vp);

    // Initialize the projection matrix
    Projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, width / (FLOAT)height, 0.01f, 100.0f);

    CBChangeOnResize cbChangesOnResize = {};
    cbChangesOnResize.mProjection = XMMatrixTranspose(Projection);
    pImmediateContext->UpdateSubresource(pCBChangeOnResize, 0, nullptr, &cbChangesOnResize, 0, 0);

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
    if (pMainRenderTargetView) pMainRenderTargetView->Release();
    if (pMainSwapChain) pMainSwapChain->Release();
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


