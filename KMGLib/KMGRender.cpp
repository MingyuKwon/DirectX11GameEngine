#include <EngineData.h>
#include <KMGRender.h>
#include <sstream>
#include <iostream>
#include <KMGScene.h>
#include <LoadingManager.h>
#include <CommandSchedular.h>
#include <KMGUtility.h>

using namespace std;
using namespace DirectX;

extern std::atomic<float> deltaTime;
extern std::atomic<bool> bSceneFocused;
extern std::atomic<bool> bContentFocused;
extern std::atomic<bool> bDetailFocused;

extern float g_cameraMoveSpeed;
extern float g_cameraRotateSpeed;
extern CommandSchedular* schedular;

ID3D11Device* g_pMainDevice = nullptr;

void RenderThread(
    std::vector<ID3D11CommandList*>& DX11CommandLists, std::mutex& dx11CommandMutex,
    ID3D11Device* pMainDevice,
    D3D_PRIMITIVE_TOPOLOGY topology,
    ID3D11VertexShader* pVertexShader,
    ID3D11PixelShader* pPixelShader,
    ID3D11InputLayout* pVertexLayout,
    ID3D11RenderTargetView* pRTV, ID3D11DepthStencilView* pDSV,
    ID3D11Buffer* pCBChangeOnResize, ID3D11Buffer* pCBChangeOnPlayer, ID3D11Buffer* pCBChangesEveryFrame, ID3D11Buffer* pCBLightArray,
    ID3D11Buffer* pVertexBuffer, ID3D11Buffer* pIdexBuffer,
    ID3D11ShaderResourceView* pTextureSRV, ID3D11ShaderResourceView* pNormalMapSRV, ID3D11SamplerState* pSamplerState,
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

    if (pVertexShader_Default)
    {
        pVertexShader_Default->Release();
        pVertexShader_Default = nullptr;
    }

    if (pPixelShader_Default)
    {
        pPixelShader_Default->Release();
        pPixelShader_Default = nullptr;
    }

    if (pPixelShader_NoNormalMap)
    {
        pPixelShader_NoNormalMap->Release();
        pPixelShader_NoNormalMap = nullptr;
    }

    if (pPixelShader_NoLight)
    {
        pPixelShader_NoLight->Release();
        pPixelShader_NoLight = nullptr;
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


    ///////////////////////////////
    // 텍스처를 읽을 방법인 Sample을 하나만 우선 제작함
    ///////////////////////////////
    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    HRESULT hr = g_pMainDevice->CreateSamplerState(&sampDesc, &pSamplerState);

    CompileVertexShader(L"KMGLib\\VertexShader.hlsli", pVertexShader_Default);

    CompilePixelShader(L"KMGLib\\PixelShader.hlsli", pPixelShader_Default);
    CompilePixelShader(L"KMGLib\\PixelShader_NoNormalMap.hlsli", pPixelShader_NoNormalMap);
    CompilePixelShader(L"KMGLib\\PixelShader_NoLight.hlsli", pPixelShader_NoLight);

    
    CreateConstBuffers();
    CreateRenderTarget();
}

HRESULT KMGRender::CreateConstBuffers()
{
    HRESULT hr = S_OK;

    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;

    ////////////////////////////////
    // 창이 바뀔 때 마다 변하는 값을 저장하는 버퍼 생성
    ////////////////////////////////
    bd.ByteWidth = sizeof(CBChangeOnResize);
    hr = g_pMainDevice->CreateBuffer(&bd, nullptr, &pCBChangeOnResize);
    if (FAILED(hr)) return hr;

    float fovAngleY = XMConvertToRadians(45.0f);
    float aspectRatio = (float)sceneWindowWidth / (float)sceneWindowHeight;
    float nearZ = 0.1f;
    float farZ = 100.0f;
    XMMATRIX Projection = XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, nearZ, farZ);

    CBChangeOnResize cbr = {};
    currentCameraProjectionMatrix = Projection;
    cbr.mProjection = currentCameraProjectionMatrix;
    pMainContext->UpdateSubresource(pCBChangeOnResize, 0, nullptr, &cbr, 0, 0);

    ///////////////////////////////
    // 플레이어(카메라)가 변할 때마다 저장하는 버퍼 생성
    ////////////////////////////////
    bd.ByteWidth = sizeof(CBChangeOnPlayer);
    hr = g_pMainDevice->CreateBuffer(&bd, nullptr, &pCBChangeOnPlayer);
    if (FAILED(hr)) return hr;

    bd.ByteWidth = sizeof(CBLightArray);
    hr = g_pMainDevice->CreateBuffer(&bd, nullptr, &pCBLightArray);
    if (FAILED(hr)) return hr;

    pMainContext->UpdateSubresource(pCBLightArray, 0, nullptr, &lightArray, 0, 0);

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
    float fovAngleY = XMConvertToRadians(70.0f);
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

    if (!scene)
    {
        std::cout << "This Scene is nullptr not suitable to Render\n";
        return;
    }

    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);

    const double targetFrameTime = 1.0 / 120.0; // 기준을 60fps로 맞춤

    LARGE_INTEGER frameStart;
    QueryPerformanceCounter(&frameStart);

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

    resourceManager.ClearShouldDrawActor();

    ////////////////////////////////////////////
    ///////////////////////////////////////////
    KMGCamera currentCamera = scene->GetCurrentCamera();

    CBChangeOnPlayer cbp = {};
    currentCameraViewMatrix = XMMatrixTranspose(currentCamera.GetViewMatrix());
    cbp.mView = currentCameraViewMatrix;

    pMainContext->UpdateSubresource(pCBChangeOnPlayer, 0, nullptr, &cbp, 0, 0);

    // 여기서 deferred로 렌더링 준비 작업을 메시의 개수만큼 멀티 스레드로 돌린다
    vector<thread> renderSettingThreads;

    const unordered_map<wstring, unique_ptr<KMGActor>>& actors = scene->getAllActors();
    lightArray.clear();

    // 여기에 이름이 기록된 리소스들은 그리지 말고, 마지막에 지워져야함
    unordered_set<wstring> shouldEraseResourceName;
    unordered_set<wstring> shouldEraseActorName;
    unordered_map<wstring, int> ActorMeshCount;

    for (auto& bucket : actors)
    {
        KMGActor* actor = bucket.second.get();
        wstring actorName = actor->GetName();

        
        bool hasLightComp = false;
        // 빛 컴포넌트가 있는 지 확인함
        if (actor->HasComponent(EComponentType::ECT_LIGHT))
        {
            LightComponent* lightComp = actor->GetComponent<LightComponent>(EComponentType::ECT_LIGHT);
            lightArray.AddLight(lightComp->GetLight());

            hasLightComp = true;
        }

        // 가져올 Mesh가 있는지 확인함
        const vector<KMGStaticMesh>* actorMeshes = nullptr;
        if (actor->HasComponent(EComponentType::ECT_STATICMESH))
        {
            StaticMeshComponent* staticComp = actor->GetComponent<StaticMeshComponent>(EComponentType::ECT_STATICMESH);
            actorMeshes = staticComp->GetMeshes();
            ActorMeshCount[actorName] = actorMeshes->size();
        }
        
        resourceManager.AddShouldDrawActor(
            actor->bShouldDrawResourceChange,
            actorName,
            actorMeshes,
            pMainContext,
            hasLightComp,
            actor->getWorldMatrix()
        );

        // 여기서 추가로 resource가 디버그를 그려야 한다면 DrawResrouce에 추가한다

        //resourceManager.AddShouldDrawDebug(meshName, mesh, pMainContext, worldMat);

    }

    pMainContext->UpdateSubresource(pCBLightArray, 0, nullptr, &lightArray, 0, 0);

    std::unordered_map<std::wstring, DrawResource>& drawResources = resourceManager.GetDrawResources();

    for (auto& bucket : drawResources)
    {
        DrawResource& resource = bucket.second;
        wstring resourceName = bucket.first;

        ID3D11PixelShader* pCurrentPixelShader = SelectPixelShader(resource);

        renderSettingThreads.emplace_back([this, pCurrentPixelShader, &resource] {
            RenderThread(
                DX11CommandLists, dx11CommandMutex,
                g_pMainDevice,
                resource.bDebug ? D3D10_PRIMITIVE_TOPOLOGY_LINELIST : D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
                pVertexShader_Default,
                pCurrentPixelShader,
                pVertexLayout,
                pSceneRTV, pSceneDSV,
                pCBChangeOnResize, pCBChangeOnPlayer,resource.pCBChangesEveryFrame, pCBLightArray,
                resource.pVertexBuffer, resource.pIndexBuffer,
                resource.pTextureSRV, resource.pNormalMapSRV, pSamplerState, 
                resource.indexCount,
                sceneWindowWidth, sceneWindowHeight
            );}
         );
    }

    //여기에 최종으로 모인 scene의 Debug를 싹다 모아서 resource에 추가한다
    resourceManager.ClearShouldDrawDebug();
    std::unordered_map<std::wstring, KMGDebugMesh> debugMeshed = scene->GetDebugMeshes();
    for (const auto& bucket : debugMeshed)
    {
        // 디벅는 메시는 절대 안바뀐다
        std::wstring meshName = bucket.first;
        const KMGDebugMesh& mesh = bucket.second;
        XMMATRIX worldMat = XMMatrixIdentity();

        resourceManager.AddShouldDrawDebug(meshName, mesh, pMainContext, worldMat);
    }

    std::unordered_map<std::wstring, DrawResource>& drawDebugResources = resourceManager.GetDebugResources();

    for (auto& bucket : drawDebugResources)
    {
        DrawResource& resource = bucket.second;
        wstring resourceName = bucket.first;

        ID3D11PixelShader* pCurrentPixelShader = SelectPixelShader(resource);

        renderSettingThreads.emplace_back([this, pCurrentPixelShader, &resource] {
            RenderThread(
                DX11CommandLists, dx11CommandMutex,
                g_pMainDevice,
                resource.bDebug ? D3D10_PRIMITIVE_TOPOLOGY_LINELIST : D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
                pVertexShader_Default,
                pCurrentPixelShader,
                pVertexLayout,
                pSceneRTV, pSceneDSV,
                pCBChangeOnResize, pCBChangeOnPlayer, resource.pCBChangesEveryFrame, pCBLightArray,
                resource.pVertexBuffer, resource.pIndexBuffer,
                resource.pTextureSRV, resource.pNormalMapSRV, pSamplerState,
                resource.indexCount,
                sceneWindowWidth, sceneWindowHeight
            ); }
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

    //if(count > 0) cout << "DrawCommand " << count << "\n";
    
    DX11CommandLists.clear();
}

ID3D11PixelShader* KMGRender::SelectPixelShader(DrawResource& resource)
{
    ID3D11PixelShader* pCurrentPixelShader = pPixelShader_Default;
    if (resource.pNormalMapSRV == nullptr)
    {
        pCurrentPixelShader = pPixelShader_NoNormalMap;
    }

    if (!resource.bLightEffected)
    {
        pCurrentPixelShader = pPixelShader_NoLight;
    }

    return pCurrentPixelShader;
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

    bSceneFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows | ImGuiFocusedFlags_RootAndChildWindows);

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

    // 여기에서 화면의 어느 지점을 플레이어가 눌렀는지를 확인한다
    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0)) {
        ImVec2 windowPos = ImGui::GetWindowPos();              
        ImVec2 contentRegionMin = ImGui::GetWindowContentRegionMin(); 
        ImVec2 texScreenPos = ImVec2(windowPos.x + contentRegionMin.x, windowPos.y + contentRegionMin.y);   

        ImVec2 mousePos = ImGui::GetMousePos();

        ImVec2 localClick = ImVec2(mousePos.x - texScreenPos.x, mousePos.y - texScreenPos.y);

        if (localClick.x >= 0 && localClick.y >= 0 &&
            localClick.x < sceneWindowWidth && localClick.y < sceneWindowHeight) {

            XMVECTOR rayDir = KMGUtility::GenerateCameraRayDirection(
                localClick.x, localClick.y,
                sceneWindowWidth, sceneWindowHeight,
                currentCameraViewMatrix, currentCameraProjectionMatrix);

            // 여기서 스케큘러에게 줘야 한다
            if (schedular)
            {
                schedular->PushCommand(KMGCommand::CameraRayTrace(rayDir));
            }
        }
    }


    ImVec2 imagePos = ImGui::GetItemRectMin();   
    ImVec2 imageSize = ImGui::GetItemRectSize(); 

    ImVec2 bgPadding = ImVec2(8, 4);
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    ///////////////////////////////
    // 이게 FPS 보여주는 텍스트 띄우기
    //////////////////////////////
    static float showDeltaTime = deltaTime;
    if (deltaTime > 0.000001f) showDeltaTime = deltaTime;

    int framePerSecond = static_cast<int>(1 / showDeltaTime);
    string fpsStr = "FPS : " + to_string(framePerSecond);
    const char* c_showStr = fpsStr.c_str();

    ImVec2 textSize = ImGui::CalcTextSize(c_showStr);

    ImVec2 textPos = ImVec2(
        imagePos.x + imageSize.x - 8,
        imagePos.y + 4
    );
    textPos.x -= textSize.x;

    drawList->AddRectFilled(
        ImVec2(textPos.x - bgPadding.x, textPos.y - bgPadding.y),
        ImVec2(textPos.x + textSize.x + bgPadding.x, textPos.y + textSize.y + bgPadding.y),
        IM_COL32(0, 0, 0, 220)
    );

    drawList->AddText(
        textPos,
        framePerSecond > 40 ? IM_COL32(29, 219, 22, 255) : IM_COL32(255, 0, 0, 255),
        c_showStr
    );

    textPos.x += textSize.x;
    textPos.y += (textSize.y + 2 * bgPadding.y);
    ///////////////////////////////
    // 이게 카메라 속도 보여주는 텍스트 띄우기
    //////////////////////////////

    int cameraSpeed = g_cameraMoveSpeed * 100;
    string cameraSpeedStr = "Camera Speed : " + to_string(cameraSpeed);
    c_showStr = cameraSpeedStr.c_str();
    textSize = ImGui::CalcTextSize(c_showStr);

    textPos.x -= textSize.x;

    drawList->AddRectFilled(
        ImVec2(textPos.x - bgPadding.x, textPos.y - bgPadding.y),
        ImVec2(textPos.x + textSize.x + bgPadding.x, textPos.y + textSize.y + bgPadding.y),
        IM_COL32(0, 20, 0, 220)
    );

    drawList->AddText(
        textPos,
        IM_COL32(255, 255, 255, 255),
        c_showStr
    );

    textPos.x += textSize.x;
    textPos.y += (textSize.y + bgPadding.y);

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

    bContentFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows | ImGuiFocusedFlags_RootAndChildWindows);

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

    bDetailFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows | ImGuiFocusedFlags_RootAndChildWindows);

    if (!storage->GetBool(id)) {
        storage->SetBool(id, true);
    }

    ImGui::Text("Hello");
    ImGui::End();
}

HRESULT KMGRender::CompileVertexShader(const WCHAR* vertexShaderName, ID3D11VertexShader*& pVertexShader)
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
        { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 60, D3D11_INPUT_PER_VERTEX_DATA, 0 },

    };
    UINT numElements = ARRAYSIZE(layout);

    hr = g_pMainDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
        pVSBlob->GetBufferSize(), &pVertexLayout);
    pVSBlob->Release();
    if (FAILED(hr)) return hr;

    return hr;
}

HRESULT KMGRender::CompilePixelShader(const WCHAR* pixelShaderName, ID3D11PixelShader*& pPixelShader)
{
    ID3DBlob* pPSBlob = nullptr;

    HRESULT hr = CompileShaderFromFile(pixelShaderName, "PS", "ps_4_0", &pPSBlob);
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

