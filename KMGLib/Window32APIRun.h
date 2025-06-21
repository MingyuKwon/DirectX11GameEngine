#pragma once
#include <windows.h>
#include <SceneWindow.h>
#include <ContentWindow.h>
#include <DetailWindow.h>

class KMGEngine
{
public:
	KMGEngine();
	virtual ~KMGEngine();

	static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


private:
	HWND hMainWnd = nullptr;

	HWND hSceneWnd = nullptr;
	HWND hContentWnd = nullptr;
	HWND hDetailWnd = nullptr;

	SceneWindow* sceneWindow = nullptr;
	ContentWindow* contentWindow = nullptr;
	DetailWindow* detailWindow = nullptr;

	LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	int InitBaseWindow();
	int InitSubWindow();
	int InitMenuBar();

	void CloseEngine();
};

