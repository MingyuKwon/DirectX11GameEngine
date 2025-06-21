#include <EngineData.h>
#include <Window32APIRun.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    hWindowInstance = hInstance;

    KMGEngine engine;

    MSG msg = {};

    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency); 
    LARGE_INTEGER prevTime;
    QueryPerformanceCounter(&prevTime); 

    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                break;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            LARGE_INTEGER currentTime;
            QueryPerformanceCounter(&currentTime);

            double deltaTime = static_cast<double>(currentTime.QuadPart - prevTime.QuadPart) / frequency.QuadPart;
            prevTime = currentTime;

            //engine.Tick(static_cast<float>(deltaTime));
        }
    }

    return static_cast<int>(msg.wParam);
}