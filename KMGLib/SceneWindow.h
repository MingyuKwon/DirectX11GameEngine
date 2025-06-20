#pragma once
#include <windows.h>

int InitScenePanel();
void ResizeSceneWindows();

LRESULT CALLBACK SceneWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
