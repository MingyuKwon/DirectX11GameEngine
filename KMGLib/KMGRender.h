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
	KMGRender(HWND hMainWnd);
	virtual ~KMGRender();

	KMGRender(const KMGRender&) = delete;
	KMGRender& operator=(const KMGRender&) = delete;

	KMGRender(KMGRender&&) = delete;
	KMGRender& operator=(KMGRender&&) = delete;

	void AddRenderCommand(RenderCommand command);
	void ResizeScreen(int width, int height);

	void StartRenderEngine();
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

	std::atomic<int> windowWidth = DEFAULT_WINDOW_WIDTH;
	std::atomic<int> windowHeight = DEFAULT_WINDOW_HEIGHT;
	std::atomic<bool> resizeRequested = false;

	bool CreateDeviceD3D();
	int InitD3D_IMGUI();

	void RenderLoop(); // «ŸΩ… ∑ª¥ı ∑Á«¡
	void CheckRenderQueue();

	void DrawIMGUI_UI();

	void CreateRenderTarget();
	void CleanupRenderTarget();

	void Render_SceneWindow();
	void Render_ContentWindow();
	void Render_DetailWindow();

	unordered_map<wstring, DrawResource> drawResources;


};

