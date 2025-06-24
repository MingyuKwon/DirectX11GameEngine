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

	int StartEngine();
	void StopEngine();

	static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	HWND hMainWnd = nullptr;

	LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	std::atomic<bool> bRunning = false;

	std::thread gameThread;
	std::thread renderThread;
	std::mutex engineMutex;

	std::mutex renderCommandMutex;
	std::queue<RenderCommand> renderCommandQueue;

	ID3D11Device* pMainDevice = nullptr;
	ID3D11DeviceContext* pMainContext = nullptr;
	IDXGISwapChain* pSwapChain = nullptr;
	ID3D11RenderTargetView* mainRenderTargetView = nullptr;

	void AddRenderCommand(RenderCommand command);


	int InitBaseWindow();
	bool CreateDeviceD3D();
	int InitD3D_IMGUI();
	int InitMenuBar();

	void GameLogicLoop(); // 스레드에서 돌아갈 메인 로직
	void RenderLoop(); // 스레드에서 돌아가갈 렌더링 로직
	int MainLoop(); // 앱의 핵심이 되는 루프

	void CreateRenderTarget();

	int Render_IMGUI_Windows();
	void Render_SceneWindow();
	void Render_ContentWindow();
	void Render_DetailWindow();

};

