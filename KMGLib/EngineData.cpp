#include <EngineData.h>

int currentWindowWidth = DEFAULT_WINDOW_WIDTH;
int currentWindowHeight = DEFAULT_WINDOW_HEIGHT;

HINSTANCE hWindowInstance = nullptr;

HWND g_hMainWnd = nullptr;
HWND g_hSceneView = nullptr;
HWND g_hContentBrowser = nullptr;
HWND g_hDetailPanel = nullptr;

HMENU hMenu = nullptr;
HMENU hFileMenu = nullptr;