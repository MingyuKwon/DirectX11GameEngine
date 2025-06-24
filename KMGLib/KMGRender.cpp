#include <EngineData.h>
#include <KMGRender.h>
#include <sstream>
#include <iostream>

using namespace std;

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
// 메시지 이름 반환용 함수
void PrintSRVInfo(ID3D11ShaderResourceView* srv)
{
    if (!srv)
    {
        std::cout << "SRV is null.\n";
        return;
    }

    ID3D11Resource* resource = nullptr;
    srv->GetResource(&resource);
    if (!resource)
    {
        std::cout << "SRV has no resource.\n";
        return;
    }

    D3D11_RESOURCE_DIMENSION dim;
    resource->GetType(&dim);

    if (dim == D3D11_RESOURCE_DIMENSION_TEXTURE2D)
    {
        ID3D11Texture2D* tex = nullptr;
        HRESULT hr = resource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&tex);
        if (SUCCEEDED(hr) && tex)
        {
            D3D11_TEXTURE2D_DESC desc;
            tex->GetDesc(&desc);

            std::cout << "Texture2D Info:\n";
            std::cout << "  Width       : " << desc.Width << "\n";
            std::cout << "  Height      : " << desc.Height << "\n";
            std::cout << "  MipLevels   : " << desc.MipLevels << "\n";
            std::cout << "  Format      : " << desc.Format << "\n";
            std::cout << "  BindFlags   : " << desc.BindFlags << "\n";
            std::cout << "  Usage       : " << desc.Usage << "\n";

            tex->Release();
        }
    }
    else
    {
        std::cout << "SRV resource is not Texture2D. Dimension: " << dim << "\n";
    }

    resource->Release();
}

void KMGRender::AddRenderCommand(RenderCommand command)
{
    lock_guard<mutex> lock(renderCommandMutex);
    renderCommandQueue.push(command);
}

void KMGRender::ResizeScreen(int width, int height)
{
    resizeRequested.store(true);
    windowWidth.store(width);
    windowHeight.store(height);
}

KMGRender::KMGRender(HWND hMainWnd) : hMainWnd(hMainWnd)
{
    InitD3D_IMGUI();
}

KMGRender::~KMGRender()
{
    if (!pMainDevice)
    {
        pMainDevice->Release();
        pMainDevice = nullptr;
    }

    if (!pMainContext)
    {
        pMainContext->Release();
        pMainContext = nullptr;
    }

    if (!pSwapChain)
    {
        pSwapChain->Release();
        pSwapChain = nullptr;
    }

    if (!mainRenderTargetView)
    {
        mainRenderTargetView->Release();
        mainRenderTargetView = nullptr;
    }

}

void KMGRender::StartRenderEngine()
{
    if (bRunning) return;

    bRunning = true;
    renderThread = thread(&KMGRender::RenderLoop, this);

}

void KMGRender::StopRenderEngine()
{
    if (!bRunning) return;

    bRunning = false;
    if (renderThread.joinable()) renderThread.join();

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
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &pSwapChain, &pMainDevice, &featureLevel, &pMainContext);
    if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &pSwapChain, &pMainDevice, &featureLevel, &pMainContext);
    if (res != S_OK)
        return false;

    CreateRenderTarget();
}

void KMGRender::CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    pMainDevice->CreateRenderTargetView(pBackBuffer, nullptr, &mainRenderTargetView);
    pBackBuffer->Release();
}

void KMGRender::CleanupRenderTarget()
{
    if (mainRenderTargetView) 
    { 
        mainRenderTargetView->Release(); 
        mainRenderTargetView = nullptr; 
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
    ImGui_ImplDX11_Init(pMainDevice, pMainContext);

    return 0;
}

void KMGRender::RenderLoop()
{
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);

    const double targetFrameTime = 1.0 / 60.0; // 기준을 60fps로 맞춤

    while (bRunning) {
        LARGE_INTEGER frameStart;
        QueryPerformanceCounter(&frameStart);

        CheckRenderQueue();

        if (resizeRequested.exchange(false))
        {
            CleanupRenderTarget();
            pSwapChain->ResizeBuffers(0, windowWidth, windowHeight, DXGI_FORMAT_UNKNOWN, 0);
            std::cout << "Resizing" << windowWidth << " " << windowHeight << "\n";
            CreateRenderTarget();
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        DrawIMGUI_UI();

        ImGui::Render();
        pMainContext->OMSetRenderTargets(1, &mainRenderTargetView, nullptr);
        pMainContext->ClearRenderTargetView(mainRenderTargetView, Colors::Aqua);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        // Present는 단 한번
        pSwapChain->Present(0,0);

        LARGE_INTEGER frameEnd;
        QueryPerformanceCounter(&frameEnd);
        double frameDuration = static_cast<double>(frameEnd.QuadPart - frameStart.QuadPart) / frequency.QuadPart;

        double remainingTime = targetFrameTime - frameDuration;
        if (remainingTime > 0.0) {
            this_thread::sleep_for(chrono::duration<double>(remainingTime));
        }


    }
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
    std::queue<RenderCommand> tempQueue;
    {
        std::lock_guard<std::mutex> lock(renderCommandMutex);
        std::swap(tempQueue, renderCommandQueue);
    }

    while (!tempQueue.empty())
    {
        lock_guard<mutex> lock(renderCommandMutex);
        RenderCommand command = tempQueue.front();
        tempQueue.pop();

        RenderCommandtype type = command.getType();
        switch (type)
        {
        case RenderCommandtype::ERC_RESIZE_VIEWTARGET:
        {

            break;
        }
        case RenderCommandtype::ERC_ADD_ACTOR:
        {

            break;
        }
        }

    }
}


void KMGRender::Render_SceneWindow()
{
    ImGuiID id = ImGui::GetID(SCENE_WINDOW_NAME);
    ImGuiStorage* storage = ImGui::GetStateStorage();
    if (!storage->GetBool(id)) {
        int WindowPosX = 0;
        int WindowPosY = 0;

        int WindowWidth = windowWidth * SCENE_DETAIL_WIDTH_RATIO;
        int WindowHeight = windowHeight * SCENE_CONTENT_HEIGHT_RATIO;

        ImGui::SetNextWindowSize(ImVec2(WindowWidth, WindowHeight));
        ImGui::SetNextWindowPos(ImVec2(WindowPosX, WindowPosY));

    }

    ImGui::Begin(SCENE_WINDOW_NAME);

    if (!storage->GetBool(id)) {
        storage->SetBool(id, true);
    }

    static ImVec2 prevSize = ImVec2(0, 0);
    ImVec2 contentSize = ImGui::GetContentRegionAvail();

    if (prevSize.x != contentSize.x || prevSize.y != contentSize.y)
    {
        cout << contentSize.x << "  " << contentSize.y << "\n";
        prevSize = contentSize;
    }

    ImGui::Text("Something Starnge");

    ImGui::End();
}

void KMGRender::Render_ContentWindow()
{
    ImGuiID id = ImGui::GetID(CONTENT_WINDOW_NAME);
    ImGuiStorage* storage = ImGui::GetStateStorage();
    if (!storage->GetBool(id)) {
        int WindowPosX = 0;
        int WindowPosY = windowHeight * SCENE_CONTENT_HEIGHT_RATIO;

        int WindowWidth = windowWidth * SCENE_DETAIL_WIDTH_RATIO;
        int WindowHeight = windowHeight * (1 - SCENE_CONTENT_HEIGHT_RATIO);

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
        int WindowPosX = windowWidth * SCENE_DETAIL_WIDTH_RATIO;
        int WindowPosY = 0;

        int WindowWidth = windowWidth * (1 - SCENE_DETAIL_WIDTH_RATIO);
        int WindowHeight = windowHeight;

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
