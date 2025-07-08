#pragma once
#include <KMGDataStructure.h>

class KMGActor;

class KMGDetailWindow
{
public:
	void DrawDetailWindow(
		KMGActor* focusActor,
		int mainWindowWidth, int mainWindowHeight

	);
private:
	KMGActor* currenFocusActor = nullptr;

	void ShowTransform();
	void ShowStaticMesh();
	void ShowLight();


};