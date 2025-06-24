#include <EngineData.h>
#include <Window32APIRun.h>
#include <sstream>
#include <iostream>

using namespace std;

vector<KMGVertex> sampleVertices =
{
    { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(-1.0f, 1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
    { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(-1.0f, 1.0f, -1.0f, 1.0f),XMFLOAT2(0.0f, 0.0f) },
    { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(-1.0f, 1.0f, -1.0f, 1.0f),XMFLOAT2(0.0f, 1.0f) },
    { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(-1.0f, 1.0f, -1.0f, 1.0f),XMFLOAT2(1.0f, 1.0f) },

    { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(-1.0f, 1.0f, -1.0f, 1.0f),XMFLOAT2(0.0f, 0.0f) },
    { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(-1.0f, 1.0f, -1.0f, 1.0f),XMFLOAT2(1.0f, 0.0f) },
    { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(-1.0f, 1.0f, -1.0f, 1.0f),XMFLOAT2(1.0f, 1.0f) },
    { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(-1.0f, 1.0f, -1.0f, 1.0f),XMFLOAT2(0.0f, 1.0f) },

    { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(-1.0f, 1.0f, -1.0f, 1.0f),XMFLOAT2(0.0f, 1.0f) },
    { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(-1.0f, 1.0f, -1.0f, 1.0f),XMFLOAT2(1.0f, 1.0f) },
    { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(-1.0f, 1.0f, -1.0f, 1.0f),XMFLOAT2(1.0f, 0.0f) },
    { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(-1.0f, 1.0f, -1.0f, 1.0f),XMFLOAT2(0.0f, 0.0f) },

    { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(-1.0f, 1.0f, -1.0f, 1.0f),XMFLOAT2(1.0f, 1.0f) },
    { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(-1.0f, 1.0f, -1.0f, 1.0f),XMFLOAT2(0.0f, 1.0f) },
    { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(-1.0f, 1.0f, -1.0f, 1.0f),XMFLOAT2(0.0f, 0.0f) },
    { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(-1.0f, 1.0f, -1.0f, 1.0f),XMFLOAT2(1.0f, 0.0f) },

    { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(-1.0f, 1.0f, -1.0f, 1.0f),XMFLOAT2(0.0f, 1.0f) },
    { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(-1.0f, 1.0f, -1.0f, 1.0f),XMFLOAT2(1.0f, 1.0f) },
    { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(-1.0f, 1.0f, -1.0f, 1.0f),XMFLOAT2(1.0f, 0.0f) },
    { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(-1.0f, 1.0f, -1.0f, 1.0f),XMFLOAT2(0.0f, 0.0f) },

    { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(-1.0f, 1.0f, -1.0f, 1.0f),XMFLOAT2(1.0f, 1.0f) },
    { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(-1.0f, 1.0f, -1.0f, 1.0f),XMFLOAT2(0.0f, 1.0f) },
    { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(-1.0f, 1.0f, -1.0f, 1.0f),XMFLOAT2(0.0f, 0.0f) },
    { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(-1.0f, 1.0f, -1.0f, 1.0f),XMFLOAT2(1.0f, 0.0f) },
};

vector<int> sampleIndices =
{
    3,1,0,
    2,1,3,

    6,4,5,
    7,4,6,

    11,9,8,
    10,9,11,

    14,12,13,
    15,12,14,

    19,17,16,
    18,17,19,

    22,20,21,
    23,20,22
};

// 메시지 이름 반환용 함수
const wchar_t* GetMessageName(UINT msg)
{
    switch (msg)
    {
    case WM_PAINT: return L"WM_PAINT";
    case WM_SIZE: return L"WM_SIZE";
    case WM_MOVE: return L"WM_MOVE";
    case WM_MOUSEMOVE: return L"WM_MOUSEMOVE";
    case WM_LBUTTONDOWN: return L"WM_LBUTTONDOWN";
    case WM_LBUTTONUP: return L"WM_LBUTTONUP";
    case WM_RBUTTONDOWN: return L"WM_RBUTTONDOWN";
    case WM_RBUTTONUP: return L"WM_RBUTTONUP";
    case WM_KEYDOWN: return L"WM_KEYDOWN";
    case WM_KEYUP: return L"WM_KEYUP";
    case WM_CHAR: return L"WM_CHAR";
    case WM_COMMAND: return L"WM_COMMAND";
    case WM_DESTROY: return L"WM_DESTROY";
    case WM_QUIT: return L"WM_QUIT";
    case WM_ENTERSIZEMOVE: return L"WM_ENTERSIZEMOVE";
    case WM_EXITSIZEMOVE: return L"WM_EXITSIZEMOVE";
    case WM_NCLBUTTONDOWN: return L"WM_NCLBUTTONDOWN";
    case WM_HSCROLL: return L"WM_HSCROLL";
    case WM_VSCROLL: return L"WM_VSCROLL";
    case WVR_ALIGNTOP: return L"WVR_ALIGNTOP";
    case WVR_ALIGNLEFT: return L"WVR_ALIGNLEFT";
    case WVR_ALIGNBOTTOM: return L"WVR_ALIGNBOTTOM";
    case WVR_ALIGNRIGHT: return L"WVR_ALIGNRIGHT";
    default: return L"UNKNOWN";
    }
}

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

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


LRESULT CALLBACK KMGEngine::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    /*wchar_t buffer[128];
    swprintf(buffer, 128, L"WndProc Message: %s (0x%04X)\n", GetMessageName(msg), msg);
    if(0x0200 != msg && 0x00A0 != msg) OutputDebugString(buffer);*/
    
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        EndPaint(hWnd, &ps);
        break;
    }

    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_ESCAPE:
            {
                PostQuitMessage(0);
                break;
            }
        case VK_SPACE:
            {
                KMGActor actor1;
                actor1.name = L"actor1";
                actor1.vertices = sampleVertices;
                actor1.indices = sampleIndices;

                AddRenderCommand(RenderCommand::MakeAddActorCommand(actor1));
                break;
            }
            
        }
        break;

    case WM_SIZE:
        currentWindowWidth = LOWORD(lParam);
        currentWindowHeight = HIWORD(lParam);

        if (DX11C_Main) DX11C_Main->SetScreenSize(currentWindowWidth, currentWindowHeight);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case 1:
            MessageBox(hWnd, L"Open clicked", L"Info", MB_OK);
            break;

        }
        break;

    case WM_DESTROY:
        break;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT KMGEngine::StaticWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    KMGEngine* pThis = nullptr;

    if (msg == WM_NCCREATE) {
        CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
        pThis = static_cast<KMGEngine*>(cs->lpCreateParams);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
    }
    else {
        pThis = reinterpret_cast<KMGEngine*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    }

    if (pThis)
        return pThis->WndProc(hWnd, msg, wParam, lParam);
    else
        return DefWindowProc(hWnd, msg, wParam, lParam);
}

KMGEngine::KMGEngine()
{
    InitBaseWindow();
    InitD3D_IMGUI();
    InitMenuBar();
}

KMGEngine::~KMGEngine()
{
    StopEngine();
}

int KMGEngine::StartEngine()
{
    if (bRunning) return 0;

    bRunning = true;

    gameThread = thread(&KMGEngine::GameLogicLoop, this);
    renderThread = thread(&KMGEngine::RenderLoop, this);


    ////////////////////////////////////////////
    // 메인 로직 돌아가는 곳
    ////////////////////////////////////////////
    int result = MainLoop();

    return result;

}

void KMGEngine::StopEngine()
{
    if (!bRunning) return;

    bRunning = false;
    if (gameThread.joinable()) gameThread.join();
    if (renderThread.joinable()) renderThread.join();

    // 이건 엔진 끝날 때 넣기
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    if(DX11C_Main) delete DX11C_Main;
}

int KMGEngine::InitBaseWindow()
{
    hWindowInstance = GetModuleHandle(nullptr);

    const wchar_t CLASS_NAME[] = ENGINE_NAME;

    WNDCLASSEX wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = KMGEngine::StaticWndProc;
    wcex.hInstance = hWindowInstance;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszClassName = CLASS_NAME;

    RegisterClassEx(&wcex);

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    int midXPos = (screenWidth - currentWindowWidth) / 2;
    int midYPos = (screenHeight - currentWindowHeight) / 2;

    hMainWnd = CreateWindowEx(
        0,
        CLASS_NAME,
        ENGINE_NAME,
        WS_OVERLAPPEDWINDOW,
        midXPos, midYPos,
        currentWindowWidth, currentWindowHeight,
        nullptr, nullptr, hWindowInstance, this);

    if (!hMainWnd) return 1;
    ShowWindow(hMainWnd, 1);
    UpdateWindow(hMainWnd);


    return 0;
}

int KMGEngine::InitMenuBar()
{
    if (!hMainWnd) return 0;

    hMenu = CreateMenu();
    hFileMenu = CreatePopupMenu();

    AppendMenu(hFileMenu, MF_STRING, 1, L"Open");
    AppendMenu(hFileMenu, MF_STRING, 2, L"Exit");

    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"File");

    SetMenu(hMainWnd, hMenu);

    return 1;
}

void KMGEngine::AddRenderCommand(RenderCommand command)
{
    lock_guard<mutex> lock(renderCommandMutex);
    renderCommandQueue.push(command);
}

int KMGEngine::InitD3D_IMGUI()
{
    DX11C_Main = new D3D11Machine(EDirectXMode::EDXM_IMGUI, hMainWnd);

    ID3D11Device* pd3dDevice = nullptr;
    ID3D11DeviceContext* pImmediateContext = nullptr;

    if (DX11C_Main) DX11C_Main->GetD3DDeviceContext(&pd3dDevice, &pImmediateContext);
    if (pd3dDevice == nullptr || pImmediateContext == nullptr) return 1;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    io.IniFilename = nullptr;  // 저장된 위치 무시

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;


    ImGui_ImplWin32_Init(hMainWnd);
    ImGui_ImplDX11_Init(pd3dDevice, pImmediateContext);

    DX11C_Scene = new D3D11Machine(EDirectXMode::EDXM_TEXTURE, nullptr);

    return 0;
}

int KMGEngine::MainLoop()
{
    MSG msg = {};

    while (true)
    {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                return static_cast<int>(msg.wParam);

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}

int KMGEngine::Render_IMGUI_Windows()
{
    if (DX11C_Main)
    {
        DX11C_Main->ResizeScreen();
        DX11C_Main->ClearScreen();
    }

    // 프레임 시작
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // Docking 공간 정의
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

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    return 0;
}

void KMGEngine::Render_SceneWindow()
{
    ImGuiID id = ImGui::GetID(SCENE_WINDOW_NAME);
    ImGuiStorage* storage = ImGui::GetStateStorage();
    if (!storage->GetBool(id)) {
        int WindowPosX = 0;
        int WindowPosY = 0;

        int WindowWidth = currentWindowWidth * SCENE_DETAIL_WIDTH_RATIO;
        int WindowHeight = currentWindowHeight * SCENE_CONTENT_HEIGHT_RATIO;

        ImGui::SetNextWindowSize(ImVec2(WindowWidth, WindowHeight));
        ImGui::SetNextWindowPos(ImVec2(WindowPosX, WindowPosY));

    }

    ImGui::Begin(SCENE_WINDOW_NAME);

    if (!storage->GetBool(id)) {
        storage->SetBool(id, true);  
    }

    static ImVec2 prevSize = ImVec2(0,0);
    ImVec2 contentSize = ImGui::GetContentRegionAvail();

    if (prevSize.x != contentSize.x || prevSize.y != contentSize.y)
    {
        cout << contentSize.x << "  " << contentSize.y << "\n";
        if (DX11C_Scene)
        {
            DX11C_Scene->SetScreenSize(contentSize.x, contentSize.y);
        }

        prevSize = contentSize;
    }

    if (DX11C_Scene)
    {
        DX11C_Scene->ResizeScreen();
        DX11C_Scene->DrawTexture();
        DX11C_Scene->SetDrawReady();
    }

    ID3D11ShaderResourceView* srv = nullptr;
    DX11C_Scene->GetSRVTexture(&srv);

    PrintSRVInfo(srv);

    if (srv)
    {
        //ImGui::Image(srv, ImVec2(0, 0));
    }

    ImGui::End();



}

void KMGEngine::Render_ContentWindow()
{
    ImGuiID id = ImGui::GetID(CONTENT_WINDOW_NAME);
    ImGuiStorage* storage = ImGui::GetStateStorage();
    if (!storage->GetBool(id)) {
        int WindowPosX = 0;
        int WindowPosY = currentWindowHeight * SCENE_CONTENT_HEIGHT_RATIO;

        int WindowWidth = currentWindowWidth * SCENE_DETAIL_WIDTH_RATIO;
        int WindowHeight = currentWindowHeight * (1- SCENE_CONTENT_HEIGHT_RATIO);

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

void KMGEngine::Render_DetailWindow()
{
    ImGuiID id = ImGui::GetID(DETAIL_WINDOW_NAME);
    ImGuiStorage* storage = ImGui::GetStateStorage();
    if (!storage->GetBool(id)) {
        int WindowPosX = currentWindowWidth * SCENE_DETAIL_WIDTH_RATIO;
        int WindowPosY = 0;

        int WindowWidth = currentWindowWidth * (1- SCENE_DETAIL_WIDTH_RATIO);
        int WindowHeight = currentWindowHeight;

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

void KMGEngine::RenderLoop()
{
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);

    const double targetFrameTime = 1.0 / 60.0; // 기준을 60fps로 맞춤

    while (bRunning) {
        LARGE_INTEGER frameStart;
        QueryPerformanceCounter(&frameStart);

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
                    if (DX11C_Scene)
                    {
                        int width = 0;
                        int height = 0;
                        command.GetViewTargetWidthHeight(width, height);

                    }
                    break;
                }
            case RenderCommandtype::ERC_ADD_ACTOR:
            {
                if (DX11C_Scene)
                {
                    KMGActor actor;
                    command.GetActor(actor);

                }
                break;
            }
            }

        }

        Render_IMGUI_Windows();


        if (DX11C_Main) DX11C_Main->SetDrawReady();
        if (DX11C_Main) DX11C_Main->TryPresent();


        LARGE_INTEGER frameEnd;
        QueryPerformanceCounter(&frameEnd);
        double frameDuration = static_cast<double>(frameEnd.QuadPart - frameStart.QuadPart) / frequency.QuadPart;

        double remainingTime = targetFrameTime - frameDuration;
        if (remainingTime > 0.0) {
            this_thread::sleep_for(chrono::duration<double>(remainingTime));
        }


    }
}


void KMGEngine::GameLogicLoop()
{
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    LARGE_INTEGER prev;
    QueryPerformanceCounter(&prev);

    while (bRunning) {
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        float deltaTime = static_cast<float>(now.QuadPart - prev.QuadPart) / frequency.QuadPart;
        prev = now;

        {
            lock_guard<mutex> lock(engineMutex);
        }

        this_thread::sleep_for(chrono::milliseconds(1));
    }
}


