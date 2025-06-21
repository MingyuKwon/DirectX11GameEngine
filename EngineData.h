#pragma once
#include <windows.h>

#define ENGINE_NAME L"KMG Engine"

#define SCENE_WINDOW_NAME L"Scene Window"
#define CONTENT_WINDOW_NAME L"Content Window"
#define DETAIL_WINDOW_NAME L"Detail Window"

#define DEFAULT_WINDOW_WIDTH 1600
#define DEFAULT_WINDOW_HEIGHT 900

#define SCENE_DETAIL_WIDTH_RATIO 0.8
#define SCENE_CONTENT_HEIGHT_RATIO 0.7

extern int currentWindowWidth;
extern int currentWindowHeight;

extern HINSTANCE hWindowInstance;

extern HMENU hMenu;
extern HMENU hFileMenu;