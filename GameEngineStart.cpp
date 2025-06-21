#include <EngineData.h>
#include <Window32APIRun.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    hWindowInstance = hInstance;

    KMGEngine engine;

    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg); 
        DispatchMessage(&msg);  
    }

    return (int)msg.wParam;
}