#include <EngineData.h>
#include <KMGRender.h>
#include <iostream>
#include <string>
#include <KMGScene.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
int InitBaseWindow();
void GetDeltaTime();

std::atomic<float> deltaTime = 0;

HINSTANCE hWindowInstance = nullptr;
HWND hMainWnd = nullptr;
HMENU hMenu = nullptr;
HMENU hFileMenu = nullptr;

KMGRender* renderEngine = nullptr;
KMGScene* currentScene = nullptr;

using namespace std;
using namespace DirectX;


int main(int, char**)
{
    InitBaseWindow();

    renderEngine = new KMGRender(hMainWnd);
    renderEngine->StartRenderEngine();

    currentScene = new KMGScene();

    MSG msg = {};

    bool bRunning = true;

    while (bRunning)
    {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                bRunning = false;
            }
                
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        GetDeltaTime();
        XMMATRIX RotateMatrix = XMMatrixRotationY(deltaTime);


        renderEngine->RenderScene(currentScene);
    }

    renderEngine->StopRenderEngine();
    delete renderEngine;
    return 0;

}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        EndPaint(hWnd, &ps);
        break;
    }

    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_ESCAPE:
        {
            PostQuitMessage(0);
            break;
        }
        case VK_DELETE:
        {
            if (renderEngine)
            {
                //renderEngine->AddRenderCommand(RenderCommand::MakeRemoveActorCommand(L"actor1"));
            }
            break;
        }
        case VK_SPACE:
        {
            if (currentScene)
            {
                KMGActor* actor = currentScene->CreateActor(L"Actor1");
                if (!actor)  std::cout << "Failed to Create Actor\n";
            }

            if (renderEngine)
            {
                //renderEngine->AddRenderCommand(RenderCommand::MakeAddActorCommand(actor2));
            }

            break;
        }

        }
        break;

    case WM_SIZE:
    {
        int width = LOWORD(lParam);
        int height = HIWORD(lParam);

        if (renderEngine) renderEngine->ResizeScreen(width, height);
        break;

    }
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case 1:
            MessageBox(hWnd, L"Open clicked", L"Info", MB_OK);
            break;

        }
        break;

    case WM_DESTROY:
        break;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

void GetDeltaTime()
{
    static ULONGLONG prevTime = 0;
    ULONGLONG timeCur = GetTickCount64();

    if (prevTime == 0)
        prevTime = timeCur;

    ULONGLONG elapsedTimeMs = timeCur - prevTime;
    deltaTime.store(elapsedTimeMs / 1000.0f);

    prevTime = timeCur;

}

int InitBaseWindow()
{
    hWindowInstance = GetModuleHandle(nullptr);

    const wchar_t CLASS_NAME[] = ENGINE_NAME;

    WNDCLASSEX wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hWindowInstance;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszClassName = CLASS_NAME;

    RegisterClassEx(&wcex);

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    int midXPos = (screenWidth - DEFAULT_WINDOW_WIDTH) / 2;
    int midYPos = (screenHeight - DEFAULT_WINDOW_HEIGHT) / 2;

    hMainWnd = CreateWindowEx(
        0,
        CLASS_NAME,
        ENGINE_NAME,
        WS_OVERLAPPEDWINDOW,
        midXPos, midYPos,
        DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT,
        nullptr, nullptr, hWindowInstance, nullptr);


    hMenu = CreateMenu();
    hFileMenu = CreatePopupMenu();

    AppendMenu(hFileMenu, MF_STRING, 1, L"Open");
    AppendMenu(hFileMenu, MF_STRING, 2, L"Exit");

    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"File");

    SetMenu(hMainWnd, hMenu);


    if (!hMainWnd) return 1;
    ShowWindow(hMainWnd, 1);
    UpdateWindow(hMainWnd);


    return 0;
}

// Tutorial에서 가져온 코드
//--------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DCompile
//
// With VS 11, we could load up prebuilt .cso files instead...
//--------------------------------------------------------------------------------------
HRESULT CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows
    // the shaders to be optimized and to run exactly the way they will run in
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;

    // Disable optimizations to further improve shader debugging
    dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    ID3DBlob* pErrorBlob = nullptr;
    hr = D3DCompileFromFile(szFileName, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, szEntryPoint, szShaderModel,
        dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
    if (FAILED(hr))
    {
        if (pErrorBlob)
        {
            OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
            MessageBoxA(nullptr, (char*)pErrorBlob->GetBufferPointer(), "Shader Compile Error", MB_OK);
            pErrorBlob->Release();
        }
        return hr;
    }
    if (pErrorBlob) pErrorBlob->Release();

    return S_OK;
}