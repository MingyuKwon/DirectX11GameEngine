#pragma once
#include <SubWindow.h>

class SceneWindow : public SubWindow
{
public:
	SceneWindow(HWND hMainWnd);
	virtual ~SceneWindow();

	virtual void Tick(float deltaTime) override;

	virtual int InitWindowPanel() override;
	virtual void ResizeWindow() override;

protected:
	virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
};

