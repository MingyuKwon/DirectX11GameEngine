#pragma once
#include <windows.h>
#include <SceneWindow.h>
#include <ContentWindow.h>
#include <DetailWindow.h>

int InitBaseWindow();
int InitMenuBar();

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
