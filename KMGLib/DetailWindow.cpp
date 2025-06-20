#include <DetailWindow.h>
#include <EngineData.h>

int InitDetailPanel()
{
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = DetailWndProc;
    wc.hInstance = hWindowInstance;
    wc.lpszClassName = DETAIL_WINDOW_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 3);

    RegisterClassEx(&wc);

    g_hDetailPanel = CreateWindowEx(
        0,
        DETAIL_WINDOW_NAME,
        nullptr,
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        0, 0,
        0, 0,
        g_hMainWnd, (HMENU)3, hWindowInstance, nullptr);

    ResizeDetailWindows();

    return 1;
}

void ResizeDetailWindows()
{
    if (!g_hDetailPanel) return;

    int detailPanelPosX = currentWindowWidth * SCENE_DETAIL_WIDTH_RATIO + 2;
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

LRESULT DetailWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}