#pragma once
#include <ContentWindow.h>
#include <EngineData.h>

int InitContentPanel()
{
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = ContentWndProc;
    wc.hInstance = hWindowInstance;
    wc.lpszClassName = CONTENT_WINDOW_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);

    RegisterClassEx(&wc);

    g_hContentBrowser = CreateWindowEx(
        0,
        CONTENT_WINDOW_NAME,
        nullptr,
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        0, 0,
        0, 0,
        g_hMainWnd, (HMENU)2, hWindowInstance, nullptr);

    ResizeContentWindows();

    return 1;
}

void ResizeContentWindows()
{
    if (!g_hContentBrowser) return;

    int contentPanelPosX = 0;
    int contentPanelPosY = currentWindowHeight * SCENE_CONTENT_HEIGHT_RATIO + 2;

    int contentPanelWidth = currentWindowWidth * SCENE_DETAIL_WIDTH_RATIO;
    int contentPanelHeight = currentWindowHeight * (1 - SCENE_CONTENT_HEIGHT_RATIO);

    MoveWindow(
        g_hContentBrowser,
        contentPanelPosX, contentPanelPosY,
        contentPanelWidth, contentPanelHeight,
        TRUE
    );
}

LRESULT ContentWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}