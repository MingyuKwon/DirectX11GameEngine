// Win32 API를 사용한 가장 기본적인 윈도우 창 만들기 예제
#include <windows.h>

// 윈도우 프로시저: 창에서 발생하는 메시지를 처리하는 함수
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}

// 프로그램의 시작 지점 (WinMain)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // 1. 윈도우 클래스 정의
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;             // 메시지 처리 함수
    wc.hInstance = hInstance;           // 현재 인스턴스 핸들
    wc.lpszClassName = L"MyWin32Window";   // 윈도우 클래스 이름

    // 2. 윈도우 클래스 등록
    RegisterClass(&wc);

    // 3. 윈도우 생성
    HWND hWnd = CreateWindow(
        wc.lpszClassName,      // 윈도우 클래스 이름
        L"Hello, Win32!",      // 타이틀바에 표시될 이름
        WS_OVERLAPPEDWINDOW,   // 윈도우 스타일
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, // 위치와 크기
        nullptr, nullptr, hInstance, nullptr
    );

    // 윈도우 생성 실패 시 종료
    if (!hWnd) return 0;

    // 4. 윈도우 화면에 표시
    ShowWindow(hWnd, nCmdShow);

    // 5. 메시지 루프: 사용자의 입력이나 시스템 이벤트를 처리
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
