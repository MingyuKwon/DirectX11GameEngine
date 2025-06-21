#pragma once
#include <SubWindow.h>
#include <DirectX11Run.h>

class SceneWindow : public SubWindow
{
public:
	SceneWindow(HWND hMainWnd) : SubWindow(hMainWnd){};

	virtual void Tick(float deltaTime) override;

	virtual int InitWindowPanel() override;
	virtual void ResizeWindow() override;

protected:
	virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;

private :
	DirectX11Wrapper* directx11Wraper = nullptr;
};

