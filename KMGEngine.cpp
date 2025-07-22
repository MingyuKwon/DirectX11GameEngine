#include <EngineData.h>
#include <KMGRender.h>
#include <iostream>
#include <string>
#include <KMGScene.h>
#include <CommandSchedular.h>
#include <algorithm>
#include <DrawDebug.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
int InitBaseWindow();
void GetDeltaTime();
float GetPhysicsDeltaTime();
void MoveCameraRealtime();
void RotateCameraRealtime();

const double targetFrameTime = 1.0 / DEFAULT_MAIN_FRAME_COUNT; // 기준을 60fps로 맞춤
std::atomic<float> deltaTime = 0;

std::atomic<bool> bRunning = true;

std::atomic<bool> bSceneFocused = false;
std::atomic<bool> bHierarchyFocused = false;
std::atomic<bool> bDetailFocused = false;

float g_cameraMoveSpeed = SCENE_EDIT_CAMERAMOVESPEED;
float g_cameraRotateSpeed = SCENE_EDIT_CAMERAROTATESPEED;

HINSTANCE hWindowInstance = nullptr;
HWND hMainWnd = nullptr;
HMENU hMenu = nullptr;
HMENU hFileMenu = nullptr;

KMGRender* renderEngine = nullptr;
DrawResourceManager resourceManager;
MeshLoader meshLoader;

CommandSchedular* schedular = nullptr;

KMGScene* currentScene = nullptr;

using namespace std;
using namespace DirectX;

void MoveCameraRealtime();
void RotateCameraRealtime();

void TextureImportLoop();
void MeshImportLoop();

void PhysicsLoop();

void GlobalTick(float deltaTime);


int main(int, char**)
{
    InitBaseWindow();

    renderEngine = new KMGRender(hMainWnd);
    renderEngine->StartRenderEngine();

    schedular = new CommandSchedular();
    KMGCommand::ChangeScene(new KMGScene());
    currentScene->InitializeScene();

    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);

    resourceManager.AddTextureSRVs(DEFAULT_TEXTURE_FILEPATH);
    resourceManager.AddTextureSRVs(DEFAULT_NORMAL_FILEPATH);

    thread textureImportThread(TextureImportLoop);
    thread meshImportThread(MeshImportLoop);
    thread physicsThread(PhysicsLoop);

    MSG msg = {};

    while (bRunning)
    {
        LARGE_INTEGER frameStart;
        QueryPerformanceCounter(&frameStart);

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
        // 그리기 직전에 들어온 모든 명령 동기적으로 처리
        schedular->ExecuteMessage_InSchedular(currentScene);

        GlobalTick(deltaTime);

        // 마지막으로 그리기
        renderEngine->RenderScene(currentScene);

        LARGE_INTEGER frameEnd;
        QueryPerformanceCounter(&frameEnd);
        double frameDuration = static_cast<double>(frameEnd.QuadPart - frameStart.QuadPart) / frequency.QuadPart;

        double remainingTime = targetFrameTime - frameDuration;
        if (remainingTime > 0.0) {
            this_thread::sleep_for(chrono::duration<double>(remainingTime));
        }
    }

    textureImportThread.join();
    meshImportThread.join();
    physicsThread.join();

    renderEngine->StopRenderEngine();
    delete renderEngine;
    delete schedular;
    return 0;

}


void PhysicsLoop()
{
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);

    while (bRunning) {
        LARGE_INTEGER frameStart;
        QueryPerformanceCounter(&frameStart);

        float physicsDeltaTime = GetPhysicsDeltaTime();

        if (currentScene)
        {
            currentScene->SetAxisActorPosToFocus();

            currentScene->GetAxisActor()->ExecutePhysics(physicsDeltaTime);
            const std::unordered_map<std::wstring, std::unique_ptr<KMGActor>>& actors = currentScene->getAllActors();
            for (auto& bucket : actors)
            {
                bucket.second->ExecutePhysics(physicsDeltaTime);
            }

        }

        LARGE_INTEGER frameEnd;
        QueryPerformanceCounter(&frameEnd);
        double frameDuration = static_cast<double>(frameEnd.QuadPart - frameStart.QuadPart) / frequency.QuadPart;

        double remainingTime = 1 / DEFAULT_PHYSICS_FRAME_COUNT - frameDuration;
        if (remainingTime > 0.0) {
            this_thread::sleep_for(chrono::duration<double>(remainingTime));
        }
    }
}


void TextureImportLoop()
{
    while (bRunning) {
        // 텍스처 로딩
        resourceManager.MakeRequestTextures();
        std::this_thread::sleep_for(std::chrono::milliseconds(DEFAULT_IMPORT_FRAME_DURATION));
    }
}

void MeshImportLoop()
{
    while (bRunning) {

        // 여기서 해야 하는 것은 액터 로딩 
        meshLoader.MakeMeshOnRequest();

        std::this_thread::sleep_for(std::chrono::milliseconds(DEFAULT_IMPORT_FRAME_DURATION));
    }
}



void GlobalTick(float deltaTime)
{
    if (bSceneFocused)
    {
        MoveCameraRealtime();
        RotateCameraRealtime();
    }

    DRAW_DEBUG_LINE(XMFLOAT3(0,0,0), XMFLOAT3(0, 10, 0), XMFLOAT4(1,0,0,1), 1);

    if (currentScene)
    {
        currentScene->SwapAllActorTransformBuffer();
        currentScene->Tick(deltaTime);
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) {}

    bool isRightMouseDown = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;


    switch (msg)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        EndPaint(hWnd, &ps);
        break;
    }

    case WM_MOUSEWHEEL:
    {
        if (!bSceneFocused) break;
        if ((GetAsyncKeyState(VK_RBUTTON) & 0x8000) == 0) break;

        short delta = GET_WHEEL_DELTA_WPARAM(wParam);
        if (delta > 0)
            g_cameraMoveSpeed += 0.01f;
        else
            g_cameraMoveSpeed += -0.01f;

        if (g_cameraMoveSpeed <= 0) g_cameraMoveSpeed = 0.01f;
        if (g_cameraMoveSpeed >= 0.2) g_cameraMoveSpeed = 0.2f;

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
            case 'Q':
            {
                if (currentScene && !isRightMouseDown)
                {
                    currentScene->SetSceneEditMode(ESceneEditMode::ESEM_SELECT);
                }

                break;
            }
            case 'W':
            {
                if (currentScene && !isRightMouseDown)
                {
                    currentScene->SetSceneEditMode(ESceneEditMode::ESEM_MOVE);
                }

                break;
            }
            case 'E':
            {
                if (currentScene && !isRightMouseDown)
                {
                    currentScene->SetSceneEditMode(ESceneEditMode::ESEM_ROTATE);
                }
                break;
            }
            case 'R':
            {
                if (currentScene && !isRightMouseDown)
                {
                    currentScene->SetSceneEditMode(ESceneEditMode::ESEM_SCALE);
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

void MoveCameraRealtime()
{
    if ((GetAsyncKeyState(VK_RBUTTON) & 0x8000) == 0) return;
    if (!currentScene) return;

    XMVECTOR moveVector = XMVectorZero();

    if (GetAsyncKeyState('A') & 0x8000)
        moveVector = XMVectorAdd(moveVector, XMVectorSet(-1, 0.0f, 0.0f, 0.0f));
    if (GetAsyncKeyState('D') & 0x8000)
        moveVector = XMVectorAdd(moveVector, XMVectorSet(+1, 0.0f, 0.0f, 0.0f));
    if (GetAsyncKeyState('W') & 0x8000)
        moveVector = XMVectorAdd(moveVector, XMVectorSet(0.0f, 0.0f, +1, 0.0f));
    if (GetAsyncKeyState('S') & 0x8000)
        moveVector = XMVectorAdd(moveVector, XMVectorSet(0.0f, 0.0f, -1, 0.0f));
    if (GetAsyncKeyState('Q') & 0x8000)
        moveVector = XMVectorAdd(moveVector, XMVectorSet(0.0f, -1, 0.0f, 0.0f));
    if (GetAsyncKeyState('E') & 0x8000)
        moveVector = XMVectorAdd(moveVector, XMVectorSet(0.0f, +1, 0.0f, 0.0f));

    if (!XMVector3Equal(moveVector, XMVectorZero()))
    {
        XMVECTOR moveDir = XMVector3Normalize(moveVector);
        KMGCommand::UpdateCameraPosition_R(
            XMVectorScale(moveDir, g_cameraMoveSpeed)
        );
    }
}

void RotateCameraRealtime()
{
    if (!currentScene) return;

    static POINT prevMousePos = {0,0};

    POINT currMousePos;
    GetCursorPos(&currMousePos);

    if ((GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0)
    {
        int deltaX = currMousePos.x - prevMousePos.x;
        int deltaY = currMousePos.y - prevMousePos.y;

        KMGCommand::UpdateCameraForwardVector(
            deltaX * SCENE_EDIT_CAMERAROTATESPEED, deltaY * SCENE_EDIT_CAMERAROTATESPEED
        );

    }

    prevMousePos = currMousePos;
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

float GetPhysicsDeltaTime()
{
    static ULONGLONG prevTime = 0;
    ULONGLONG timeCur = GetTickCount64();

    if (prevTime == 0)
        prevTime = timeCur;

    ULONGLONG elapsedTimeMs = timeCur - prevTime;
    float physicsDeltaTime = elapsedTimeMs / 1000.0f;

    prevTime = timeCur;


    return physicsDeltaTime;
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