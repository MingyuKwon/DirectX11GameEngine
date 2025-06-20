#include <windows.h>
#include "EngineData.h"

int InitBaseWindow(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
int InitMenuBar();

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
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    int result = InitBaseWindow(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
    if (result == 0) return 0;

    result = InitMenuBar();
    if (result == 0) return 0;
    
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg); 
        DispatchMessage(&msg);  
    }

    return (int)msg.wParam;
}

int InitBaseWindow(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    const wchar_t CLASS_NAME[] = ENGINE_NAME;

    WNDCLASSEX wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszClassName = CLASS_NAME;

    RegisterClassEx(&wcex);

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    int midXPos = (screenWidth - DEFAULT_WINDOW_WIDTH) / 2;
    int midYPos = (screenHeight - DEFAULT_WINDOW_HEIGHT) / 2;

    hBaseWnd = CreateWindowEx(
        0,
        CLASS_NAME,
        ENGINE_NAME,
        WS_OVERLAPPEDWINDOW,
        midXPos, midYPos,
        DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT,
        nullptr, nullptr, hInstance, nullptr);

    if (!hBaseWnd) return 0;
    ShowWindow(hBaseWnd, nCmdShow);

    return 1;
}

int InitMenuBar()
{
    if (!hBaseWnd) return 0;

    hMenu = CreateMenu();
    hFileMenu = CreatePopupMenu();

    AppendMenu(hFileMenu, MF_STRING, 1, L"Open");
    AppendMenu(hFileMenu, MF_STRING, 2, L"Exit");

    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"File");
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"File");

    SetMenu(hBaseWnd, hMenu);

    return 1;
}
