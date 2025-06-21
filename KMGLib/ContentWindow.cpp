#pragma once
#include <ContentWindow.h>
#include <EngineData.h>


LRESULT ContentWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}

ContentWindow::ContentWindow(HWND hMainWnd) : SubWindow(hMainWnd)
{
    InitWindowPanel();
}

void ContentWindow::Tick(float deltaTime)
{
}

int ContentWindow::InitWindowPanel()
{
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = ContentWndProc;
    wc.hInstance = hWindowInstance;
    wc.lpszClassName = CONTENT_WINDOW_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);

    RegisterClassEx(&wc);

    hSubWnd = CreateWindowEx(
        0,
        CONTENT_WINDOW_NAME,
        nullptr,
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        0, 0,
        0, 0,
        hMainWnd, (HMENU)2, hWindowInstance, nullptr);

    ResizeWindow();

    return 1;
}

void ContentWindow::ResizeWindow()
{
    if (!hSubWnd) return;

    int contentPanelPosX = 0;
    int contentPanelPosY = currentWindowHeight * SCENE_CONTENT_HEIGHT_RATIO + 2;

    int contentPanelWidth = currentWindowWidth * SCENE_DETAIL_WIDTH_RATIO;
    int contentPanelHeight = currentWindowHeight * (1 - SCENE_CONTENT_HEIGHT_RATIO);

    MoveWindow(
        hSubWnd,
        contentPanelPosX, contentPanelPosY,
        contentPanelWidth, contentPanelHeight,
        TRUE
    );

}

LRESULT ContentWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}
