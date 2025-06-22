#include <EngineData.h>
#include <Window32APIRun.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    hWindowInstance = hInstance;
    KMGEngine engine;
    return engine.StartEngine();
}