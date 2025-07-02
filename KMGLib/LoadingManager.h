#pragma once
#include <KMGDataStructure.h>

extern HINSTANCE hWindowInstance;

LRESULT CALLBACK LoadingWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

class LoadingManager
{
public:
	virtual ~LoadingManager();

	bool StartLoading();

	void SetTotalCount(int count) { totalCount = count; }
	void PlusCurrentCount() { ++currentCount; }

	int getTotalCount() { return totalCount; }
	int getCurrentCount() { return currentCount; }

private:
	void LoadingLoop();

	std::atomic<int> totalCount = 100;
	std::atomic<int> currentCount = 0;

	std::thread loadingThread;
};