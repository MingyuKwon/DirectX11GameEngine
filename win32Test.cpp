// Win32 API�� ����� ���� �⺻���� ������ â ����� ����
#include <windows.h>

// ������ ���ν���: â���� �߻��ϴ� �޽����� ó���ϴ� �Լ�
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}

// ���α׷��� ���� ���� (WinMain)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // 1. ������ Ŭ���� ����
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;             // �޽��� ó�� �Լ�
    wc.hInstance = hInstance;           // ���� �ν��Ͻ� �ڵ�
    wc.lpszClassName = L"MyWin32Window";   // ������ Ŭ���� �̸�

    // 2. ������ Ŭ���� ���
    RegisterClass(&wc);

    // 3. ������ ����
    HWND hWnd = CreateWindow(
        wc.lpszClassName,      // ������ Ŭ���� �̸�
        L"Hello, Win32!",      // Ÿ��Ʋ�ٿ� ǥ�õ� �̸�
        WS_OVERLAPPEDWINDOW,   // ������ ��Ÿ��
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, // ��ġ�� ũ��
        nullptr, nullptr, hInstance, nullptr
    );

    // ������ ���� ���� �� ����
    if (!hWnd) return 0;

    // 4. ������ ȭ�鿡 ǥ��
    ShowWindow(hWnd, nCmdShow);

    // 5. �޽��� ����: ������� �Է��̳� �ý��� �̺�Ʈ�� ó��
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
