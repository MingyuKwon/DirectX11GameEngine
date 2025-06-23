#include <EngineData.h>

int currentWindowWidth = DEFAULT_WINDOW_WIDTH;
int currentWindowHeight = DEFAULT_WINDOW_HEIGHT;

HINSTANCE hWindowInstance = nullptr;

DirectX11Wrapper* DX11W_Main = nullptr;
DirectX11Wrapper* DX11W_Scene = nullptr;

HMENU hMenu = nullptr;
HMENU hFileMenu = nullptr;