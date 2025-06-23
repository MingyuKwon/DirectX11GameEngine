#include <EngineData.h>
#include <Window32APIRun.h>
#include <sstream>

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

LRESULT CALLBACK KMGEngine::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    /*wchar_t buffer[128];
    swprintf(buffer, 128, L"WndProc Message: %s (0x%04X)\n", GetMessageName(msg), msg);
    if(0x0200 != msg && 0x00A0 != msg) OutputDebugString(buffer);*/
    
    switch (msg)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        EndPaint(hWnd, &ps);
        break;
    }

    case WM_EXITSIZEMOVE:
    {
        int scenePanelWidth = currentWindowWidth * SCENE_DETAIL_WIDTH_RATIO;
        int scenePanelHeight = currentWindowHeight * SCENE_CONTENT_HEIGHT_RATIO;

        AddRenderCommand(RenderCommand::MakeResizeViewTargetCommand(scenePanelWidth, scenePanelHeight));
        break;
    }

    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_ESCAPE:
            {
                StopEngine();
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

        if (sceneWindow) sceneWindow->ResizeWindow();
        if (contentWindow) contentWindow->ResizeWindow();
        if (detailWindow) detailWindow->ResizeWindow();
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case 1:
            MessageBox(hWnd, L"Open clicked", L"Info", MB_OK);
            break;

        case 2:
            PostMessage(sceneWindow->getWindowHandle(), WM_CLOSE, 0, 0);
            StopEngine();
            break;
        }
        break;

    case WM_DESTROY:
        StopEngine();
        break;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}


KMGEngine::KMGEngine()
{
    InitBaseWindow();
    InitSubWindow();
    InitMenuBar();
}

KMGEngine::~KMGEngine()
{
    if (sceneWindow) delete sceneWindow;
    if (contentWindow) delete contentWindow;
    if (detailWindow) delete detailWindow;
}

int KMGEngine::StartEngine()
{
    if (bRunning) return 0;

    bRunning = true;

    directx11Wraper = new DirectX11Wrapper(sceneWindow->getWindowHandle());

    gameThread = thread(&KMGEngine::MainLoop, this);
    renderThread = thread(&KMGEngine::RenderLoop, this);

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

        if (directx11Wraper)
        {
            directx11Wraper->TrySceneWindowPresent();
        }
    }

    StopEngine();

    return static_cast<int>(msg.wParam);

}

void KMGEngine::StopEngine()
{
    if (!bRunning) return;

    bRunning = false;
    if (gameThread.joinable()) gameThread.join();
    if (renderThread.joinable()) renderThread.join();

    if (directx11Wraper) delete directx11Wraper;

    PostMessage(sceneWindow->getWindowHandle(), WM_CLOSE, 0, 0);
    PostMessage(contentWindow->getWindowHandle(), WM_CLOSE, 0, 0);
    PostMessage(detailWindow->getWindowHandle(), WM_CLOSE, 0, 0);

    PostQuitMessage(0);

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

void KMGEngine::AddRenderCommand(RenderCommand command)
{
    lock_guard<mutex> lock(renderCommandMutex);
    renderCommandQueue.push(command);
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

    if (!hMainWnd) return 0;
    ShowWindow(hMainWnd, 1);

    return 1;
}

int KMGEngine::InitSubWindow()
{
    sceneWindow = new SceneWindow(hMainWnd);
    contentWindow = new ContentWindow(hMainWnd);
    detailWindow = new DetailWindow(hMainWnd);

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

void KMGEngine::MainLoop() 
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
            MainLogicTick(deltaTime);
        }

        this_thread::sleep_for(chrono::milliseconds(1));
    }
}

void KMGEngine::RenderLoop()
{
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    LARGE_INTEGER prev;
    QueryPerformanceCounter(&prev);

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
                    if (directx11Wraper)
                    {
                        int width = 0;
                        int height = 0;
                        command.GetViewTargetWidthHeight(width, height);

                        directx11Wraper->ResizeViewtarget(width, height);
                    }
                    break;
                }
            case RenderCommandtype::ERC_ADD_ACTOR:
            {
                if (directx11Wraper)
                {
                    KMGActor actor;
                    command.GetActor(actor);
                    directx11Wraper->AddActor(actor);
                }
                break;
            }
            }

        }

        float deltaTime = static_cast<float>(frameStart.QuadPart - prev.QuadPart) / frequency.QuadPart;
        prev = frameStart;

        {
            lock_guard<mutex> lock(engineMutex);
            RenderTick(deltaTime);
        }

        LARGE_INTEGER frameEnd;
        QueryPerformanceCounter(&frameEnd);
        double frameDuration = static_cast<double>(frameEnd.QuadPart - frameStart.QuadPart) / frequency.QuadPart;

        double remainingTime = targetFrameTime - frameDuration;
        if (remainingTime > 0.0) {
            this_thread::sleep_for(chrono::duration<double>(remainingTime));
        }
    }
}


void KMGEngine::MainLogicTick(float deltaTime)
{
    if (!contentWindow) return;
    if (!detailWindow) return;

    contentWindow->Tick(deltaTime);
    detailWindow->Tick(deltaTime);
}

void KMGEngine::RenderTick(float deltaTime)
{
    if (!sceneWindow) return;

    sceneWindow->Tick(deltaTime);
}
