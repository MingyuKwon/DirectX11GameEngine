#pragma once
#include <KMGDataStructure.h>

class KMGSceneHierarchyWindow
{
public:

	void SelectActor(std::wstring name);

	void DrawHierarchyWindow(
		int mainWindowWidth, int mainWindowHeight
	);
private:
	std::wstring selectActorName;
};