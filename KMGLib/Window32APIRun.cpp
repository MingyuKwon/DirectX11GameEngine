#include <EngineData.h>
#include <Window32APIRun.h>

LRESULT CALLBACK KMGEngine::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_ESCAPE:
            CloseEngine();
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
            CloseEngine();
            break;
        }
        break;
    case WM_DESTROY:
    {
        CloseEngine();
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

void KMGEngine::CloseEngine()
{
    PostMessage(hSceneWnd, WM_CLOSE, 0, 0);
    PostMessage(hContentWnd, WM_CLOSE, 0, 0);
    PostMessage(hDetailWnd, WM_CLOSE, 0, 0);

    PostQuitMessage(0);
}
