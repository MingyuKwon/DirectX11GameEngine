#include <EngineData.h>
#include <Window32APIRun.h>

using namespace std;

// 이건 메인 로직에서 처리하도록 해야하나?
LRESULT CALLBACK KMGEngine::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_ESCAPE:
            StopEngine();
            break;

        default:
            break;
        }
        return 0;
    case WM_SIZE:
    {
        currentWindowWidth = LOWORD(lParam);
        currentWindowHeight = HIWORD(lParam);

        if (sceneWindow) sceneWindow->ResizeWindow();
        if (contentWindow) contentWindow->ResizeWindow();
        if (detailWindow) detailWindow->ResizeWindow();

        return 0;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case 1:
            MessageBox(hWnd, L"Open clicked", L"Info", MB_OK);
            break;

        case 2:
            PostMessage(hSceneWnd, WM_CLOSE, 0, 0);
            StopEngine();
            break;
        }
        break;
    case WM_DESTROY:
    {
        StopEngine();
        return 0;
    }

    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
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

void KMGEngine::StartEngine()
{
    if (bRunning) return;

    bRunning = true;
    gameThread = thread(&KMGEngine::MainLoop, this);
    renderThread = thread(&KMGEngine::RenderLoop, this);

}

void KMGEngine::StopEngine()
{
    if (!bRunning) return;

    bRunning = false;
    if (gameThread.joinable()) gameThread.join();
    if (renderThread.joinable()) renderThread.join();

    PostMessage(hSceneWnd, WM_CLOSE, 0, 0);
    PostMessage(hContentWnd, WM_CLOSE, 0, 0);
    PostMessage(hDetailWnd, WM_CLOSE, 0, 0);

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

int KMGEngine::InitBaseWindow()
{
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

    hSceneWnd = sceneWindow->getWindowHandle();
    hContentWnd = contentWindow->getWindowHandle();
    hDetailWnd = detailWindow->getWindowHandle();

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

        // 여기서 그리는 전처리를 해야한다

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

