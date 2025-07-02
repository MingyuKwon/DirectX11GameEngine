#include <LoadingManager.h>
#include <iostream>

std::atomic<bool> g_loadingFinished = false;
HWND g_loadingWindow = nullptr;

LRESULT CALLBACK LoadingWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_NCCREATE:
	{
		CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		LoadingManager* pThis = reinterpret_cast<LoadingManager*>(pCreate->lpCreateParams);

		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
		break;
	}
	case WM_PAINT:
	{
		LoadingManager* pThis = reinterpret_cast<LoadingManager*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		// 1. 배경 지우기 (흰색 또는 원하는 색)
		RECT rect;
		GetClientRect(hWnd, &rect);
		HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255)); // 흰색
		FillRect(hdc, &rect, hBrush);
		DeleteObject(hBrush);

		// 2. 텍스트 출력
		wchar_t buffer[100];
		swprintf(buffer, 100, L"로딩 중... (%d / %d)", pThis->getCurrentCount(), pThis->getTotalCount());
		DrawTextW(hdc, buffer, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		EndPaint(hWnd, &ps);
		break;
	}
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LoadingManager::LoadingManager()
{
	StartLoading();
}

LoadingManager::~LoadingManager()
{
	loadingThread.join();
}

bool LoadingManager::StartLoading()
{
	if (loadingThread.joinable()) return false;

	loadingThread = std::thread([this]()
		{
			LoadingLoop();
		});

	return true;
}


void LoadingManager::LoadingLoop()
{
	const wchar_t* className = L"LoadingWindowClass";

	WNDCLASS wc = {};
	wc.lpfnWndProc = LoadingWndProc;
	wc.hInstance = GetModuleHandle(nullptr);
	wc.lpszClassName = className;
	RegisterClass(&wc);

	int width = 300;
	int height = 100;

	// 화면 중앙 계산
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	int x = (screenWidth - width) / 2;
	int y = (screenHeight - height) / 2;

	// 창 생성
	HWND hwnd = CreateWindowEx(
		0, className, L"로딩 중입니다...",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
		x, y, width, height,
		nullptr, nullptr, GetModuleHandle(nullptr), this
	);

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	MSG msg;

	while (currentCount < totalCount)
	{
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// 텍스트 업데이트 강제
		InvalidateRect(hwnd, nullptr, TRUE);

		std::this_thread::sleep_for(std::chrono::milliseconds(50));

	}

	DestroyWindow(hwnd);

}

