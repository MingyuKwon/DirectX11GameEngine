#pragma once
#include <windows.h>
#include <QueueCommand.h>

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

#include <atomic>
#include <thread>

#include <mutex>

class KMGRender
{
public:
	KMGRender();
	virtual ~KMGRender();

	void AddRenderCommand(RenderCommand command);

	int StartRenderEngine();
	void StopRenderEngine();
private:
	HWND hMainWnd = nullptr;

	std::atomic<bool> bRunning = false;
	std::thread renderThread;

	std::mutex renderCommandMutex;
	std::queue<RenderCommand> renderCommandQueue;

	ID3D11Device* pMainDevice = nullptr;
	ID3D11DeviceContext* pMainContext = nullptr;
	IDXGISwapChain* pSwapChain = nullptr;
	ID3D11RenderTargetView* mainRenderTargetView = nullptr;

	static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	int InitBaseWindow();
	bool CreateDeviceD3D();
	int InitD3D_IMGUI();

	void RenderLoop(); // ·»´õ ºê·£Ä¡ ·çÇÁ
	int MainLoop(); // ÇÙ½É ·»´õ ·çÇÁ

	void CreateRenderTarget();

	int Render_IMGUI_Windows();
	void Render_SceneWindow();
	void Render_ContentWindow();
	void Render_DetailWindow();

};

