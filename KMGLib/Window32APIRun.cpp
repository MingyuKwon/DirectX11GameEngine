#include <EngineData.h>
#include <Window32APIRun.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
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

        ResizeSceneWindows();
        ResizeContentWindows();
        ResizeDetailWindows();

        return 0;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case 1:
            MessageBox(hWnd, L"Open clicked", L"Info", MB_OK);
            break;

        case 2:
            PostMessage(g_hSceneView, WM_CLOSE, 0, 0);
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
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}

int InitBaseWindow()
{
    const wchar_t CLASS_NAME[] = ENGINE_NAME;

    WNDCLASSEX wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hWindowInstance;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszClassName = CLASS_NAME;

    RegisterClassEx(&wcex);

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    int midXPos = (screenWidth - currentWindowWidth) / 2;
    int midYPos = (screenHeight - currentWindowHeight) / 2;

    g_hMainWnd = CreateWindowEx(
        0,
        CLASS_NAME,
        ENGINE_NAME,
        WS_OVERLAPPEDWINDOW,
        midXPos, midYPos,
        currentWindowWidth, currentWindowHeight,
        nullptr, nullptr, hWindowInstance, nullptr);

    if (!g_hMainWnd) return 0;
    ShowWindow(g_hMainWnd, 1);

    return 1;
}

int InitMenuBar()
{
    if (!g_hMainWnd) return 0;

    hMenu = CreateMenu();
    hFileMenu = CreatePopupMenu();

    AppendMenu(hFileMenu, MF_STRING, 1, L"Open");
    AppendMenu(hFileMenu, MF_STRING, 2, L"Exit");

    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"File");

    SetMenu(g_hMainWnd, hMenu);

    return 1;
}

void CloseEngine()
{
    PostMessage(g_hSceneView, WM_CLOSE, 0, 0);
    PostMessage(g_hContentBrowser, WM_CLOSE, 0, 0);
    PostMessage(g_hDetailPanel, WM_CLOSE, 0, 0);

    PostQuitMessage(0);
}


