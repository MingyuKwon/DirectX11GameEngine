#pragma once
#include <windows.h>

class SubWindow 
{
public:
    static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	virtual void Tick(float deltaTime) = 0;

	virtual int InitWindowPanel() = 0;
	virtual void ResizeWindow() = 0;

    HWND getWindowHandle() { return hSubWnd; }

	SubWindow(HWND hMainWnd) : hMainWnd(hMainWnd){};

protected:
	HWND hMainWnd = nullptr;
	HWND hSubWnd = nullptr;

	virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;

};



