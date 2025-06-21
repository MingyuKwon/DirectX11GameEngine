#pragma once
#include <DirectX11Run.h>

extern DirectX11Wrapper* directx11Wraper;

int InitScenePanel();
void ResizeSceneWindows();

LRESULT CALLBACK SceneWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
