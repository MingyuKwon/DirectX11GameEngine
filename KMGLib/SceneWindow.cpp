#include <SceneWindow.h>
#include <EngineData.h>

int InitScenePanel()
{
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = SceneWndProc;
    wc.hInstance = hWindowInstance;
    wc.lpszClassName = SCENE_WINDOW_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClassEx(&wc);

    g_hSceneView = CreateWindowEx(
        0,
        SCENE_WINDOW_NAME,
        nullptr,
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        0, 0,
        0, 0,
        g_hMainWnd, (HMENU)1, hWindowInstance, nullptr);

    ResizeSceneWindows();

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

LRESULT SceneWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}