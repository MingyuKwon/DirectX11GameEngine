#include <EngineData.h>
#include <Window32APIRun.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    hWindowInstance = hInstance;

    if (!InitBaseWindow()) return 0;
    if (!InitContentPanel()) return 0;
    if (!InitDetailPanel()) return 0;
    if (!InitScenePanel()) return 0;
    if (!InitMenuBar()) return 0;
    
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg); 
        DispatchMessage(&msg);  
    }

    return (int)msg.wParam;
}