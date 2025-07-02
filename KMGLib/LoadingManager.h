#pragma once
#include <KMGDataStructure.h>

extern HINSTANCE hWindowInstance;

LRESULT CALLBACK LoadingWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

class LoadingManager
{
public:
	LoadingManager();
	virtual ~LoadingManager();

	void SetTotalCount(int count) { totalCount = count; }
	void PlusCurrentCount() { ++currentCount; }

	int getTotalCount() { return totalCount; }
	int getCurrentCount() { return currentCount; }

private:
	void LoadingLoop();
	bool StartLoading();

	std::atomic<int> totalCount = 100;
	std::atomic<int> currentCount = 0;

	std::thread loadingThread;
};