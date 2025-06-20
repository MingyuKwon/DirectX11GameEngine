#pragma once
#include <windows.h>

#define ENGINE_NAME L"KMG Engine"
#define DEFAULT_WINDOW_WIDTH 1600
#define DEFAULT_WINDOW_HEIGHT 900

#define SCENE_DETAIL_WIDTH_RATIO 0.8
#define SCENE_CONTENT_HEIGHT_RATIO 0.6

int currentWindowWidth = DEFAULT_WINDOW_WIDTH;
int currentWindowHeight = DEFAULT_WINDOW_HEIGHT;

HINSTANCE hWindowInstance = nullptr;

HWND g_hMainWnd = nullptr;
HWND g_hSceneView = nullptr;
HWND g_hContentBrowser = nullptr;
HWND g_hDetailPanel = nullptr;

HMENU hMenu = nullptr;
HMENU hFileMenu = nullptr;