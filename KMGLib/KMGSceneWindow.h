#pragma once
#include <KMGDataStructure.h>

class KMGSceneWindow
{
public:
	void ShowSceneWindow(
		int mainWindowWidth, int mainWindowHeight,
		std::atomic<int>& sceneWindowWidth, std::atomic<int>& sceneWindowHeight,
		std::atomic<bool>& resizeRequested,
		ID3D11ShaderResourceView* pSceneSRV,
		DirectX::XMMATRIX currentCameraViewMatrix, DirectX::XMMATRIX currentCameraProjectionMatrix
	);

private:
	void ShowFPS_CameraSpeed();
	void CheckSceneClick(int sceneWindowWidth, int sceneWindowHeight,
		DirectX::XMMATRIX currentCameraViewMatrix, DirectX::XMMATRIX currentCameraProjectionMatrix
	);
};