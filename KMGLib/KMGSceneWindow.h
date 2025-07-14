#pragma once
#include <KMGDataStructure.h>

class KMGScene;


class KMGSceneWindow
{
public:
	void DrawSceneWindow(
		KMGScene* currentScene,
		int mainWindowWidth, int mainWindowHeight,
		std::atomic<int>& sceneWindowWidth, std::atomic<int>& sceneWindowHeight,
		std::atomic<bool>& resizeRequested,
		ID3D11ShaderResourceView* pSceneSRV,
		DirectX::XMMATRIX currentCameraViewMatrix, DirectX::XMMATRIX currentCameraProjectionMatrix
	);

private:
	void ShowSceneSetting(KMGScene* currentScene);
	void ShowFPS_CameraSpeed();
	void CheckSceneClick(
		KMGScene* currentScene,
		int sceneWindowWidth, int sceneWindowHeight,
		DirectX::XMMATRIX currentCameraViewMatrix, DirectX::XMMATRIX currentCameraProjectionMatrix
	);
};