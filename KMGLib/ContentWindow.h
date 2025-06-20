#pragma once
#include <windows.h>

int InitContentPanel();
void ResizeContentWindows();

LRESULT CALLBACK ContentWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
