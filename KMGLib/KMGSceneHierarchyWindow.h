#pragma once
#include <KMGDataStructure.h>

class KMGActor;
class KMGScene;

class KMGSceneHierarchyWindow
{
public:

	void SelectActor(std::wstring name);

	void DrawHierarchyWindow(
		int mainWindowWidth, int mainWindowHeight
	);
private:
	std::wstring selectActorName;

	bool ShowContextItem(const char* str_id, KMGScene* currentScene, KMGActor* focusActor);
	void ShowContextWindow(KMGScene* currentScene, bool bShow);

};