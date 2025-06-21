#pragma once
#include <windows.h>

class SubWindow 
{
public:
    static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	virtual void Tick(float deltaTime) = 0;

	virtual int InitWindowPanel() = 0;
	virtual void ResizeWindow() = 0;

	SubWindow(HWND hMainWnd) : hMainWnd(hMainWnd){};

protected:
	HWND hMainWnd = nullptr;
	HWND hSubWnd = nullptr;

	virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;

};


LRESULT CALLBACK SubWindow::StaticWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    SubWindow* pThis = nullptr;

    if (msg == WM_NCCREATE) {
        CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
        pThis = static_cast<SubWindow*>(cs->lpCreateParams); 
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
    }
    else {
        pThis = reinterpret_cast<SubWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    }

    if (pThis)
        return pThis->WndProc(hWnd, msg, wParam, lParam);
    else
        return DefWindowProc(hWnd, msg, wParam, lParam);
}
