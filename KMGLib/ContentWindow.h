#pragma once
#include <SubWindow.h>

class ContentWindow : public SubWindow
{
public:
	ContentWindow(HWND hMainWnd);

	virtual int InitWindowPanel() override;
	virtual void ResizeWindow() override;

protected:
	virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;

private:
};
