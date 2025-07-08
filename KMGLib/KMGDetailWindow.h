#pragma once
#include <KMGDataStructure.h>

class KMGDetailWindow
{
public:
	void DrawDetailWindow(
		int mainWindowWidth, int mainWindowHeight

	);
private:

	void ShowTransform();
	void ShowStaticMesh();

};