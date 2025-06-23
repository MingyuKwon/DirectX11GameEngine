#include <DetailWindow.h>
#include <EngineData.h>

DetailWindow::DetailWindow(HWND hMainWnd) : SubWindow(hMainWnd) 
{
    InitWindowPanel();
}

int DetailWindow::InitWindowPanel()
{
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = SubWindow::StaticWndProc;
    wc.hInstance = hWindowInstance;
    wc.lpszClassName = DETAIL_WINDOW_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 3);

    RegisterClassEx(&wc);

    hSubWnd = CreateWindowEx(
        0,
        DETAIL_WINDOW_NAME,
        nullptr,
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        0, 0,
        0, 0,
        hMainWnd, (HMENU)3, hWindowInstance, nullptr);

    ResizeWindow();

    return 1;
}

void DetailWindow::ResizeWindow()
{
    if (!hSubWnd) return;

    int detailPanelPosX = currentWindowWidth * SCENE_DETAIL_WIDTH_RATIO + 2;
    int detailPanelPosY = 0;

    int detailPanelWidth = currentWindowWidth * (1 - SCENE_DETAIL_WIDTH_RATIO);
    int detailPanelHeight = currentWindowHeight;

    MoveWindow(
        hSubWnd,
        detailPanelPosX, detailPanelPosY,
        detailPanelWidth, detailPanelHeight,
        TRUE
    );
}

LRESULT DetailWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}
