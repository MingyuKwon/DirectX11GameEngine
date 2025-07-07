#pragma once
#include <EngineData.h>
#include <windows.h>
#include <QueueCommand.h>

#include <KMGSceneWindow.h>
#include <KMGSceneHierarchyWindow.h>
#include <KMGDetailWindow.h>

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

#include <KMGDataStructure.h>
#include <DrawResourceManager.h>

extern ID3D11Device* g_pMainDevice;
class KMGScene;

class KMGRender
{
public:
	KMGSceneWindow sceneWindow;
	KMGSceneHierarchyWindow hierarchyWindow;
	KMGDetailWindow detailWindow;


	KMGRender(HWND hMainWnd);
	virtual ~KMGRender();

	KMGRender(const KMGRender&) = delete;
	KMGRender& operator=(const KMGRender&) = delete;

	KMGRender(KMGRender&&) = delete;
	KMGRender& operator=(KMGRender&&) = delete;

	void ResizeScreen(int width, int height);

	void StartRenderEngine();
	void StopRenderEngine();

	void RenderScene(KMGScene* scene); // 핵심 렌더 루프
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

	ID3D11VertexShader* pVertexShader_Default = nullptr;

	ID3D11PixelShader* pPixelShader_Default = nullptr;
	ID3D11PixelShader* pPixelShader_NoNormalMap = nullptr;
	ID3D11PixelShader* pPixelShader_OnlyColor = nullptr;

	ID3D11InputLayout* pVertexLayout = nullptr;

	ID3D11Buffer* pCBChangeOnResize = nullptr;
	ID3D11Buffer* pCBChangeOnPlayer = nullptr;
	ID3D11Buffer* pCBLightArray = nullptr;

	CBLightArray lightArray;
	Light testLight;

	ID3D11SamplerState* pSamplerState = nullptr;

	DirectX::XMMATRIX currentCameraViewMatrix;
	DirectX::XMMATRIX currentCameraProjectionMatrix;


	std::atomic<int> mainWindowWidth = DEFAULT_WINDOW_WIDTH;
	std::atomic<int> mainWindowHeight = DEFAULT_WINDOW_HEIGHT;

	std::atomic<int> sceneWindowWidth = DEFAULT_WINDOW_WIDTH * SCENE_DETAIL_WIDTH_RATIO;
	std::atomic<int> sceneWindowHeight = DEFAULT_WINDOW_HEIGHT * HIERARCHY_DETAIL_HEIGHT_RATIO;

	std::atomic<bool> resizeRequested = false;

	std::vector<ID3D11CommandList*> DX11CommandLists;
	std::mutex dx11CommandMutex;


	bool CreateDeviceD3D();
	HRESULT CreateConstBuffers();

	int InitD3D_IMGUI();
	HRESULT CompileVertexShader(const WCHAR* vertexShaderName, ID3D11VertexShader*& pVertexShader);
	HRESULT CompilePixelShader(const WCHAR* pixelShaderName, ID3D11PixelShader*& pPixelShader);

	ID3D11PixelShader* SelectPixelShader(DrawResource& resource);

	void DrawScene(KMGScene* scene);
	void DrawIMGUI_UI();

	void CreateRenderTarget();
	void CleanupRenderTarget();

	// 여기엔 메시 별로 DrawResource를 생성한다. 하나의 액터에 여러 메시가 있을 수 있고, 각각 actorName_0, actorName_1 이렇게 이름이 부여되고 저장될 예정
	DrawResourceManager resourceManager;

};

