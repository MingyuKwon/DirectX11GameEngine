#pragma once
#include <windows.h>
#include <QueueCommand.h>

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

#include <atomic>
#include <thread>

#include <mutex>

class KMGEngine
{
public:
	KMGEngine();
	virtual ~KMGEngine();

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

	D3D11Machine* DX11C_Main = nullptr;
	D3D11Machine* DX11C_Scene = nullptr;

	void AddRenderCommand(RenderCommand command);

	int InitBaseWindow();

	int InitD3D_IMGUI();

	int InitMenuBar();

	void GameLogicLoop(); // 스레드에서 돌아갈 메인 로직
	void RenderLoop(); // 스레드에서 돌아가갈 렌더링 로직
	int MainLoop(); // 앱의 핵심이 되는 루프

	int Render_IMGUI_Windows();

	void GameLogicTick(float deltaTime); // 메인 로직을 다루는 Tick
};

