#pragma once
#include <windows.h>

int InitDetailPanel();
void ResizeDetailWindows();

LRESULT CALLBACK DetailWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
