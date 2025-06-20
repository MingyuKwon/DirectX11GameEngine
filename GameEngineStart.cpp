#include "EngineData.h"

int InitBaseWindow();

int InitContentPanel();
int InitDetailPanel();
int InitScenePanel();

int InitMenuBar();

void ResizeSceneWindows();
void ResizeContentWindows();
void ResizeDetailWindows();

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK SceneWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ContentWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK DetailWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    hWindowInstance = hInstance;

    if (!InitBaseWindow()) return 0;
    if (!InitContentPanel()) return 0;
    if (!InitDetailPanel()) return 0;
    if (!InitScenePanel()) return 0;
    if (!InitMenuBar()) return 0;
    
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg); 
        DispatchMessage(&msg);  
    }

    return (int)msg.wParam;
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

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_ESCAPE:
            PostQuitMessage(0);
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
            PostQuitMessage(0);
            break;
        }
        break;
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    }

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}

LRESULT SceneWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}

LRESULT ContentWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}

LRESULT DetailWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}

int InitScenePanel()
{
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = SceneWndProc;  
    wc.hInstance = hWindowInstance;
    wc.lpszClassName = L"ScenePanelWindow";
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);  

    RegisterClassEx(&wc);

    g_hSceneView = CreateWindowEx(
        0,
        L"ScenePanelWindow", 
        nullptr,
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        0, 0,
        0, 0,
        g_hMainWnd, (HMENU)1, hWindowInstance, nullptr);

    ResizeSceneWindows();

    return 1;
}

int InitContentPanel()
{
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = ContentWndProc;
    wc.hInstance = hWindowInstance;
    wc.lpszClassName = L"ContentPanelWindow";
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);

    RegisterClassEx(&wc);

    g_hContentBrowser = CreateWindowEx(
        0,
        L"ContentPanelWindow",
        nullptr,
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        0, 0,
        0, 0,
        g_hMainWnd, (HMENU)2, hWindowInstance, nullptr);

    ResizeContentWindows();

    return 1;
}

int InitDetailPanel()
{
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = DetailWndProc;
    wc.hInstance = hWindowInstance;
    wc.lpszClassName = L"DetailPanelWindow";
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 3);

    RegisterClassEx(&wc);

    g_hDetailPanel = CreateWindowEx(
        0,
        L"DetailPanelWindow",
        nullptr,
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        0, 0,
        0, 0,
        g_hMainWnd, (HMENU)3, hWindowInstance, nullptr);

    ResizeDetailWindows();

    return 1;
}

void ResizeSceneWindows()
{
    if (!g_hSceneView) return;

    int scenePanelPosX = 0;
    int scenePanelPosY = 0;

    int scenePanelWidth = currentWindowWidth * SCENE_DETAIL_WIDTH_RATIO;
    int scenePanelHeight = currentWindowHeight * SCENE_CONTENT_HEIGHT_RATIO;

    MoveWindow(
        g_hSceneView,
        scenePanelPosX, scenePanelPosY,
        scenePanelWidth, scenePanelHeight,
        TRUE
    );
}

void ResizeContentWindows()
{
    if (!g_hContentBrowser) return;

    int contentPanelPosX = 0;
    int contentPanelPosY = currentWindowHeight * SCENE_CONTENT_HEIGHT_RATIO + 1;

    int contentPanelWidth = currentWindowWidth * SCENE_DETAIL_WIDTH_RATIO;
    int contentPanelHeight = currentWindowHeight * (1 - SCENE_CONTENT_HEIGHT_RATIO);

    MoveWindow(
        g_hContentBrowser,
        contentPanelPosX, contentPanelPosY,
        contentPanelWidth, contentPanelHeight,
        TRUE
    );
}

void ResizeDetailWindows()
{
    if (!g_hDetailPanel) return;

    int detailPanelPosX = currentWindowWidth * SCENE_DETAIL_WIDTH_RATIO + 1;
    int detailPanelPosY = 0;

    int detailPanelWidth = currentWindowWidth * (1 - SCENE_DETAIL_WIDTH_RATIO);
    int detailPanelHeight = currentWindowHeight;

    MoveWindow(
        g_hDetailPanel,
        detailPanelPosX, detailPanelPosY,
        detailPanelWidth, detailPanelHeight,
        TRUE
    );
}




int InitMenuBar()
{
    if (!g_hMainWnd) return 0;

    hMenu = CreateMenu();
    hFileMenu = CreatePopupMenu();

    AppendMenu(hFileMenu, MF_STRING, 1, L"Open");
    AppendMenu(hFileMenu, MF_STRING, 2, L"Exit");

    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"File");
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"File");

    SetMenu(g_hMainWnd, hMenu);

    return 1;
}


