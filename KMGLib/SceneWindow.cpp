#include <SceneWindow.h>
#include <EngineData.h>

SceneWindow::SceneWindow(HWND hMainWnd) : SubWindow(hMainWnd) 
{
    InitWindowPanel();
}

SceneWindow::~SceneWindow()
{
    
}

void SceneWindow::Tick(float deltaTime)
{
    if (directx11Wraper)
    {
        directx11Wraper->SceneWindowRender();
    }
}

int SceneWindow::InitWindowPanel()
{
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = SubWindow::StaticWndProc;
    wc.hInstance = hWindowInstance;
    wc.lpszClassName = SCENE_WINDOW_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClassEx(&wc);

    hSubWnd = CreateWindowEx(
        0,
        SCENE_WINDOW_NAME,
        nullptr,
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        0, 0,
        0, 0,
        hMainWnd, (HMENU)1, hWindowInstance, this);

    ResizeWindow();

    return 1;
}

void SceneWindow::ResizeWindow()
{
    if (!hSubWnd) return;

    int scenePanelPosX = 0;
    int scenePanelPosY = 0;

    int scenePanelWidth = currentWindowWidth * SCENE_DETAIL_WIDTH_RATIO;
    int scenePanelHeight = currentWindowHeight * SCENE_CONTENT_HEIGHT_RATIO;

    MoveWindow(
        hSubWnd,
        scenePanelPosX, scenePanelPosY,
        scenePanelWidth, scenePanelHeight,
        TRUE
    );
}

LRESULT SceneWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}

