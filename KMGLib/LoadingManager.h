#pragma once
#include <KMGDataStructure.h>

extern HINSTANCE hWindowInstance;

LRESULT CALLBACK LoadingWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

enum class ELoadingType
{
	ELT_NONE,
	ELT_IMPORT_MESH,
	ELT_MAKE_GPU_DATA,
	ELT_MAX,

};

class LoadingManager
{
public:
	ELoadingType loadingType = ELoadingType::ELT_NONE;

	LoadingManager(ELoadingType loadingType);
	virtual ~LoadingManager();

	bool StopLoading();

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