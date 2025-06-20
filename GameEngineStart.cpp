#include <windows.h>
#include "EngineData.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0); // 종료 메시지
        return 0;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
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

    HWND hWnd = CreateWindowEx(
        0,                             
        CLASS_NAME,                    
        ENGINE_NAME,
        WS_OVERLAPPEDWINDOW,           
        500, 200,
        DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT,
        nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
        return 0;

    ShowWindow(hWnd, nCmdShow);  
    UpdateWindow(hWnd);          

    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg); 
        DispatchMessage(&msg);  
    }

    return (int)msg.wParam;
}
