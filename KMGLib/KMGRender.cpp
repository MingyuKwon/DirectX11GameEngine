#include <EngineData.h>
#include <KMGRender.h>
#include <sstream>
#include <iostream>
#include <KMGScene.h>

using namespace std;
using namespace DirectX;

ID3D11Device* g_pMainDevice = nullptr;

void RenderThread(
    std::vector<ID3D11CommandList*>& DX11CommandLists, std::mutex& dx11CommandMutex,
    ID3D11Device* pMainDevice,
    ID3D11VertexShader* pVertexShader,
    ID3D11PixelShader* pPixelShader,
    ID3D11InputLayout* pVertexLayout,
    ID3D11RenderTargetView* pRTV, ID3D11DepthStencilView* pDSV,
    ID3D11Buffer* pCBChangeOnResize, ID3D11Buffer* pCBChangesEveryFrame,
    ID3D11Buffer* pVertexBuffer, ID3D11Buffer* pIdexBuffer,
    int drawIndexCount,
    int textureWidth, int textureHeight);

HRESULT CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void KMGRender::ResizeScreen(int width, int height)
{
    mainWindowWidth.store(width);
    mainWindowHeight.store(height);
    resizeRequested.store(true);
}

KMGRender::KMGRender(HWND hMainWnd) : hMainWnd(hMainWnd)
{
    InitD3D_IMGUI();
}

KMGRender::~KMGRender()
{
    CleanupRenderTarget();

    if (g_pMainDevice)
    {
        g_pMainDevice->Release();
        g_pMainDevice = nullptr;
    }

    if (pMainContext)
    {
        pMainContext->Release();
        pMainContext = nullptr;
    }

    if (pSwapChain)
    {
        pSwapChain->Release();
        pSwapChain = nullptr;
    }

    if (pMainRTV)
    {
        pMainRTV->Release();
        pMainRTV = nullptr;
    }

    if (pVertexShader)
    {
        pVertexShader->Release();
        pVertexShader = nullptr;
    }

    if (pPixelShader)
    {
        pPixelShader->Release();
        pPixelShader = nullptr;
    }

    if (pVertexLayout)
    {
        pVertexLayout->Release();
        pVertexLayout = nullptr;
    }
}

void KMGRender::StartRenderEngine()
{
    if (bRunning) return;

    bRunning = true;

}

void KMGRender::StopRenderEngine()
{
    if (!bRunning) return;

    bRunning = false;

    // 이건 엔진 끝날 때 넣기
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

bool KMGRender::CreateDeviceD3D()
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
    sd.OutputWindow = hMainWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &pSwapChain, &g_pMainDevice, &featureLevel, &pMainContext);
    if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &pSwapChain, &g_pMainDevice, &featureLevel, &pMainContext);
    if (res != S_OK)
        return false;

    ID3D11Texture2D* pBackBuffer;
    pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    D3D11_TEXTURE2D_DESC desc;
    pBackBuffer->GetDesc(&desc);
    mainWindowWidth.store(desc.Width);
    mainWindowHeight.store(desc.Height);
    pBackBuffer->Release();

    CompileShader(L"KMGLib\\VertexShader.hlsli", L"KMGLib\\PixelShader.hlsli");
    CreateConstBuffers();
    CreateRenderTarget();
}

HRESULT KMGRender::CreateConstBuffers()
{
    HRESULT hr = S_OK;

    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(CBChangeOnResize);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    hr = g_pMainDevice->CreateBuffer(&bd, nullptr, &pCBChangeOnResize);
    if (FAILED(hr)) return hr;

    float fovAngleY = XMConvertToRadians(45.0f); 
    float aspectRatio = (float)sceneWindowWidth / (float)sceneWindowHeight; 
    float nearZ = 0.1f;  
    float farZ = 100.0f;

    XMMATRIX Projection = XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, nearZ, farZ);
    CBChangeOnResize cb = {};
    cb.mProjection = Projection;
    pMainContext->UpdateSubresource(pCBChangeOnResize, 0, nullptr, &cb, 0, 0);
}

void KMGRender::CreateRenderTarget()
{
    // 여기선 전체 swapChain에 해당하는 RTV를 갱신
    ID3D11Texture2D* pBackBuffer;
    pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));

    D3D11_TEXTURE2D_DESC desc;
    pBackBuffer->GetDesc(&desc);

    std::cout << "BackBuffer Size: " << desc.Width << " x " << desc.Height << "\n";

    // RenderTargetView 생성
    g_pMainDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pMainRTV);
    pBackBuffer->Release();

    // 여기선 Scene에 그릴 텍스처에 해당하는 RTV를 갱신
    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width = sceneWindowWidth;
    texDesc.Height = sceneWindowHeight;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    cout << "Texture Size : " << texDesc.Width << "  " << texDesc.Height << "\n";

    ID3D11Texture2D* renderTexture = nullptr;

    HRESULT hr = g_pMainDevice->CreateTexture2D(&texDesc, nullptr, &renderTexture);
    if (FAILED(hr)) return;

    hr = g_pMainDevice->CreateRenderTargetView(renderTexture, nullptr, &pSceneRTV);
    if (FAILED(hr))
    {
        renderTexture->Release();
        renderTexture = nullptr;
        return;
    }

    hr = g_pMainDevice->CreateShaderResourceView(renderTexture, nullptr, &pSceneSRV);
    if (FAILED(hr))
    {
        renderTexture->Release();
        renderTexture = nullptr;

        pSceneRTV->Release();
        pSceneRTV = nullptr;

        return;
    }

    if (renderTexture) {
        renderTexture->Release();
        renderTexture = nullptr;
    }

    // 여기선 Depth STencil buffer을 갱신
    D3D11_TEXTURE2D_DESC descDepth = {};
    descDepth.Width = sceneWindowWidth;
    descDepth.Height = sceneWindowHeight;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    ID3D11Texture2D* depthStencilBuffer = nullptr;
    hr = g_pMainDevice->CreateTexture2D(&descDepth, nullptr, &depthStencilBuffer);
    if (FAILED(hr)) return;
    
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;

    hr = g_pMainDevice->CreateDepthStencilView(depthStencilBuffer, &descDSV, &pSceneDSV);
    
    if (depthStencilBuffer)
    {
        depthStencilBuffer->Release();
        depthStencilBuffer = nullptr;
    }

    // 여기선 투영 행렬를 저장하는 버퍼를 업데이트 시켜준다
    float fovAngleY = XMConvertToRadians(45.0f);
    float aspectRatio = (float)sceneWindowWidth / (float)sceneWindowHeight;
    float nearZ = 0.1f;
    float farZ = 100.0f;

    XMMATRIX Projection = XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, nearZ, farZ);
    CBChangeOnResize cb = {};
    cb.mProjection = XMMatrixTranspose(Projection);
    pMainContext->UpdateSubresource(pCBChangeOnResize, 0, nullptr, &cb, 0, 0);
}

void KMGRender::CleanupRenderTarget()
{
    if (pMainRTV) 
    { 
        pMainRTV->Release(); 
        pMainRTV = nullptr; 
    }

    if (pSceneRTV)
    {
        pSceneRTV->Release();
        pSceneRTV = nullptr;
    }

    if (pSceneSRV)
    {
        pSceneSRV->Release();
        pSceneSRV = nullptr;
    }

    if (pSceneDSV)
    { 
        pSceneDSV->Release();
        pSceneDSV = nullptr;
    }

}


int KMGRender::InitD3D_IMGUI()
{
    CreateDeviceD3D();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    io.IniFilename = nullptr;  // 저장된 위치 무시

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui_ImplWin32_Init(hMainWnd);
    ImGui_ImplDX11_Init(g_pMainDevice, pMainContext);

    return 0;
}

void KMGRender::RenderScene(KMGScene* scene)
{
    if (!bRunning) return;

    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);

    const double targetFrameTime = 1.0 / 120.0; // 기준을 60fps로 맞춤

    LARGE_INTEGER frameStart;
    QueryPerformanceCounter(&frameStart);

    CheckRenderQueue();

    if (resizeRequested.exchange(false))
    {
        CleanupRenderTarget();
        pSwapChain->ResizeBuffers(0, mainWindowWidth, mainWindowHeight, DXGI_FORMAT_UNKNOWN, 0);
        CreateRenderTarget();
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    DrawScene(scene);
    // 이건 최종적인 UI를 그리는 것이므로 이 전에 그릴 텍스처가 다 준비되어 있어야 한다
    DrawIMGUI_UI();

    ImGui::Render();
    pMainContext->OMSetRenderTargets(1, &pMainRTV, nullptr);
    pMainContext->ClearRenderTargetView(pMainRTV, Colors::Aqua);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    pSwapChain->Present(0, 0);

    LARGE_INTEGER frameEnd;
    QueryPerformanceCounter(&frameEnd);
    double frameDuration = static_cast<double>(frameEnd.QuadPart - frameStart.QuadPart) / frequency.QuadPart;

    double remainingTime = targetFrameTime - frameDuration;
    if (remainingTime > 0.0) {
        this_thread::sleep_for(chrono::duration<double>(remainingTime));
    }
}

void KMGRender::DrawScene(KMGScene* scene)
{
    // 여기선 뷰 포트를 만들어서 적용시켜줘야 한다

    pMainContext->ClearDepthStencilView(pSceneDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    pMainContext->ClearRenderTargetView(pSceneRTV, Colors::White);

    ////////////////////////////////////////////
    ///////////////////////////////////////////
    static ULONGLONG timeStart = 0;
    static ULONGLONG prevTime = 0;

    static float t = 0;
    ULONGLONG timeCur = GetTickCount64();
    if (timeStart == 0)
        timeStart = timeCur;
     t = (timeCur - timeStart) / 1000.0f;

     if (prevTime == 0)
         prevTime = timeCur;

     ULONGLONG elapsedTimeMs = timeCur - prevTime;
     float newDeltaTime = elapsedTimeMs / 1000.0f; 

     if (newDeltaTime > 0.000001f) deltaTime = newDeltaTime;
         
     prevTime = timeCur;

    XMMATRIX RotateMatrix = XMMatrixRotationY(t);

    XMVECTOR Eye = XMVectorSet(0.0f, 2.0f, -6.0f, 0.0f);
    XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMMATRIX View = XMMatrixLookAtLH(Eye, At, Up);

    vector<thread> renderSettingThreads;

    const unordered_map<wstring, unique_ptr<KMGActor>>& actors = scene->getAllActors();
    for (auto& bucket : actors)
    {
        KMGActor* actor = bucket.second.get();
        wstring actorName = actor->GetName();

        auto emplaceResult = drawResources.emplace(actorName, DrawResource(actorName));
        if (emplaceResult.second) {
            emplaceResult.first->second.CreateBuffers(actor->getVertices(), actor->getIndices());
        }
        emplaceResult.first->second.UpdateWorldMatrix(pMainContext, actor->getWorldMatrix());
    }

    for (auto& bucket : drawResources)
    {
        DrawResource& resource = bucket.second;

        renderSettingThreads.emplace_back([&] {
            RenderThread(
                DX11CommandLists, dx11CommandMutex,
                g_pMainDevice,
                pVertexShader,
                pPixelShader,
                pVertexLayout,
                pSceneRTV, pSceneDSV,
                pCBChangeOnResize, resource.pCBChangesEveryFrame,
                resource.pVertexBuffer, resource.pIndexBuffer,
                resource.indexCount,
                sceneWindowWidth, sceneWindowHeight
            );}
         );
    }

    for (auto& t : renderSettingThreads)
    {
        t.join();
    }

    int count = 0;
    for (auto cmd : DX11CommandLists) {
        count++;
        pMainContext->ExecuteCommandList(cmd, TRUE);
        cmd->Release(); 
    }
    if(count > 0) cout << "DrawCommand " << count << "\n";
    
    DX11CommandLists.clear();
}

void KMGRender::DrawIMGUI_UI()
{
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
    const ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);

    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    ImGui::Begin("KMG Engine", nullptr, window_flags);
    ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    ImGui::End();

    ImGui::PopStyleVar(2);

    Render_SceneWindow();
    Render_ContentWindow();
    Render_DetailWindow();
}

void KMGRender::CheckRenderQueue()
{
    // 긴 lock을 피하기 위해서 스냅샷을 사용
    /*std::queue<RenderCommand> tempQueue;
    {
        std::lock_guard<std::mutex> lock(renderCommandMutex);
        std::swap(tempQueue, renderCommandQueue);
    }

    while (!tempQueue.empty())
    {
        lock_guard<mutex> lock(renderCommandMutex);
        RenderCommand frontCommand = tempQueue.front();
        tempQueue.pop();

        RenderCommandtype type = command.getType();
        switch (type)
        {

        }

    }*/
}


void KMGRender::Render_SceneWindow()
{
    ImGuiID id = ImGui::GetID(SCENE_WINDOW_NAME);
    ImGuiStorage* storage = ImGui::GetStateStorage();
    if (!storage->GetBool(id)) {
        int WindowPosX = 0;
        int WindowPosY = 0;

        int WindowWidth = mainWindowWidth * SCENE_DETAIL_WIDTH_RATIO;
        int WindowHeight = mainWindowHeight * SCENE_CONTENT_HEIGHT_RATIO;

        ImGui::SetNextWindowSize(ImVec2(WindowWidth, WindowHeight));
        ImGui::SetNextWindowPos(ImVec2(WindowPosX, WindowPosY));

    }

    ImGui::Begin(SCENE_WINDOW_NAME);

    if (!storage->GetBool(id)) {
        storage->SetBool(id, true);
    }

    ImVec2 contentSize = ImGui::GetContentRegionAvail();
    if (sceneWindowWidth != contentSize.x || sceneWindowHeight != contentSize.y)
    {
        sceneWindowWidth.store(contentSize.x);
        sceneWindowHeight.store(contentSize.y);
        resizeRequested.store(true);
    }

    ImGui::Image(pSceneSRV, ImVec2(sceneWindowWidth, sceneWindowHeight));

    ImVec2 imagePos = ImGui::GetItemRectMin();   
    ImVec2 imageSize = ImGui::GetItemRectSize(); 

    int framePerSecond = static_cast<int>(1 / deltaTime);
    string fpsStr = "FPS : " + to_string(framePerSecond);
    const char* c_fpsStr = fpsStr.c_str();

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 textSize = ImGui::CalcTextSize(c_fpsStr);
    ImVec2 bgPadding = ImVec2(8, 4);

    ImVec2 textPos = ImVec2(
        imagePos.x + imageSize.x - textSize.x + 20,
        imagePos.y 
    );

    drawList->AddRectFilled(
        ImVec2(textPos.x - textSize.x * 0.5f - bgPadding.x, textPos.y - bgPadding.y),
        ImVec2(textPos.x + textSize.x * 0.5f + bgPadding.x, textPos.y + textSize.y + bgPadding.y),
        IM_COL32(0, 0, 0, 220)
    );

    drawList->AddText(
        ImVec2(textPos.x - textSize.x * 0.5f, textPos.y),
        framePerSecond > 40 ? IM_COL32(29, 219, 22, 255) : IM_COL32(255, 0, 0, 255),
        c_fpsStr
    );

    ImGui::End();
}

void KMGRender::Render_ContentWindow()
{
    ImGuiID id = ImGui::GetID(CONTENT_WINDOW_NAME);
    ImGuiStorage* storage = ImGui::GetStateStorage();
    if (!storage->GetBool(id)) {
        int WindowPosX = 0;
        int WindowPosY = mainWindowHeight * SCENE_CONTENT_HEIGHT_RATIO;

        int WindowWidth = mainWindowWidth * SCENE_DETAIL_WIDTH_RATIO;
        int WindowHeight = mainWindowHeight * (1 - SCENE_CONTENT_HEIGHT_RATIO);

        ImGui::SetNextWindowSize(ImVec2(WindowWidth, WindowHeight));
        ImGui::SetNextWindowPos(ImVec2(WindowPosX, WindowPosY));

    }

    ImGui::Begin(CONTENT_WINDOW_NAME);

    if (!storage->GetBool(id)) {
        storage->SetBool(id, true);
    }

    ImGui::Text("Hello");
    ImGui::End();
}

void KMGRender::Render_DetailWindow()
{
    ImGuiID id = ImGui::GetID(DETAIL_WINDOW_NAME);
    ImGuiStorage* storage = ImGui::GetStateStorage();
    if (!storage->GetBool(id)) {
        int WindowPosX = mainWindowWidth * SCENE_DETAIL_WIDTH_RATIO;
        int WindowPosY = 0;

        int WindowWidth = mainWindowWidth * (1 - SCENE_DETAIL_WIDTH_RATIO);
        int WindowHeight = mainWindowHeight;

        ImGui::SetNextWindowSize(ImVec2(WindowWidth, WindowHeight));
        ImGui::SetNextWindowPos(ImVec2(WindowPosX, WindowPosY));

    }

    ImGui::Begin(DETAIL_WINDOW_NAME);

    if (!storage->GetBool(id)) {
        storage->SetBool(id, true);
    }

    ImGui::Text("Hello");
    ImGui::End();
}

HRESULT KMGRender::CompileShader(const WCHAR* vertexShaderName, const WCHAR* pixelShaderName)
{
    ID3DBlob* pVSBlob = nullptr;
    HRESULT hr = CompileShaderFromFile(vertexShaderName, "VS", "vs_4_0", &pVSBlob);
    if (FAILED(hr))
    {
        MessageBox(nullptr,
            L"The Vertex Shader file cannot be compiled.  Please run this executable from the directory that contains the Shader file.", L"Error", MB_OK);
        return hr;
    }

    hr = g_pMainDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &pVertexShader);
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

    hr = g_pMainDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
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

    hr = g_pMainDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &pPixelShader);
    pPSBlob->Release();
    if (FAILED(hr)) return hr;

    return hr;
}