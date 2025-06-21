#pragma once
#include <SubWindow.h>

class ContentWindow : public SubWindow
{
public:
	ContentWindow(HWND hMainWnd);

	virtual void Tick(float deltaTime) override;

	virtual int InitWindowPanel() override;
	virtual void ResizeWindow() override;

protected:
	virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;

private:
};
