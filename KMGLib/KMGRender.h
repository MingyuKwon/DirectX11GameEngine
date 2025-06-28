#pragma once
#include <EngineData.h>
#include <windows.h>
#include <QueueCommand.h>

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

#include <KMGDataStructure.h>

extern ID3D11Device* g_pMainDevice;
class KMGScene;

class KMGRender
{
public:
	KMGRender(HWND hMainWnd);
	virtual ~KMGRender();

	KMGRender(const KMGRender&) = delete;
	KMGRender& operator=(const KMGRender&) = delete;

	KMGRender(KMGRender&&) = delete;
	KMGRender& operator=(KMGRender&&) = delete;

	void ResizeScreen(int width, int height);

	void StartRenderEngine();
	void StopRenderEngine();

	void RenderScene(KMGScene* scene); // «ŸΩ… ∑ª¥ı ∑Á«¡
private:
	HWND hMainWnd = nullptr;

	std::atomic<bool> bRunning = false;

	std::mutex renderCommandMutex;

	ID3D11DeviceContext* pMainContext = nullptr;
	IDXGISwapChain* pSwapChain = nullptr;

	ID3D11DepthStencilView* pSceneDSV = nullptr;
	ID3D11RenderTargetView* pMainRTV = nullptr;
	ID3D11RenderTargetView* pSceneRTV = nullptr;
	ID3D11ShaderResourceView* pSceneSRV = nullptr;

	ID3D11VertexShader* pVertexShader = nullptr;
	ID3D11PixelShader* pPixelShader = nullptr;
	ID3D11InputLayout* pVertexLayout = nullptr;

	ID3D11Buffer* pCBChangeOnResize = nullptr;


	std::atomic<int> mainWindowWidth = DEFAULT_WINDOW_WIDTH;
	std::atomic<int> mainWindowHeight = DEFAULT_WINDOW_HEIGHT;

	std::atomic<int> sceneWindowWidth = DEFAULT_WINDOW_WIDTH * SCENE_DETAIL_WIDTH_RATIO;
	std::atomic<int> sceneWindowHeight = DEFAULT_WINDOW_HEIGHT * SCENE_CONTENT_HEIGHT_RATIO;

	std::atomic<bool> resizeRequested = false;

	std::vector<ID3D11CommandList*> DX11CommandLists;
	std::mutex dx11CommandMutex;


	bool CreateDeviceD3D();
	HRESULT CreateConstBuffers();

	int InitD3D_IMGUI();
	HRESULT CompileShader(const WCHAR* vertexShaderName, const WCHAR* pixelShaderName);

	void CheckRenderQueue();

	void DrawScene(KMGScene* scene);
	void DrawIMGUI_UI();

	void CreateRenderTarget();
	void CleanupRenderTarget();

	void Render_SceneWindow();
	void Render_ContentWindow();
	void Render_DetailWindow();

	std::unordered_map<std::wstring, DrawResource> drawResources;


};

