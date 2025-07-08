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

	void ShowName();
	void ShowTransform();
	void ShowStaticMesh();
	void ShowLight();


	void ShowTexture_Normal(std::vector<KMGStaticMesh>* meshes);
	std::wstring GetTextureFromFileExplorer();

};