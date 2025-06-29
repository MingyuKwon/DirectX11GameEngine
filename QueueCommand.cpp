#include "QueueCommand.h"
#include "KMGScene.h"
#include <iostream>

using namespace DirectX;

namespace KMGCommand
{
	std::unique_ptr<SceneCommandMessage> ChangeScene(KMGScene* scene)
	{
		return std::make_unique<SceneCommand_ChangeScene>(scene);
	}

	std::unique_ptr<SceneCommandMessage> AddActor(const std::wstring& name)
	{
		return std::make_unique<SceneCommand_AddActor>(name);
	}

	std::unique_ptr<SceneCommandMessage> RemoveActor(const std::wstring& name)
	{
		return std::make_unique<SceneCommand_RemoveActor>(name);
	}

	std::unique_ptr<SceneCommandMessage> UpdateActor(const std::wstring& name, DirectX::XMMATRIX worldMatrix)
	{
		return std::make_unique<SceneCommand_UpdateActor>(name, worldMatrix, true);
	}


	std::unique_ptr<SceneCommandMessage> UpdateCameraPosition_A(DirectX::XMVECTOR CameraPosition)
	{
		return std::make_unique<SceneCommand_CameraPositionUpdate>(CameraPosition, false);
	}

	std::unique_ptr<SceneCommandMessage> UpdateCameraPosition_R(DirectX::XMVECTOR CameraPosition)
	{
		return std::make_unique<SceneCommand_CameraPositionUpdate>(CameraPosition, true);
	}

	std::unique_ptr<SceneCommandMessage> UpdateCameraForwardVector(DirectX::XMVECTOR forwardVector)
	{
		return std::make_unique<SceneCommand_CameraForwardVectorUpdate>(forwardVector);
	}

}

void SceneCommand_AddActor::Execute(KMGScene*& scene)
{
	if (!scene) return;

	scene->CreateActor(actorName);
}

void SceneCommand_RemoveActor::Execute(KMGScene*& scene)
{
	if (!scene) return;

	scene->EraseActor(actorName);

}
void SceneCommand_ChangeScene::Execute(KMGScene*& scene)
{
	if (scene)
	{
		delete scene;
		scene = nullptr;
	}

	scene = this->scene;
}

void SceneCommand_UpdateActor::Execute(KMGScene*& scene)
{
	if (!scene) return;
	
	KMGActor* findActor = scene->GetActor(actorName);

	if (findActor)
	{
		findActor->UpdateWorldMatrix(bRelative ? findActor->getWorldMatrix() * worldMatrix : worldMatrix);
	}
}

void SceneCommand_CameraPositionUpdate::Execute(KMGScene*& scene)
{
	if (!scene) return;

	KMGCamera& currentCamera = scene->GetCurrentCamera();

	currentCamera.SetCameraPosition(bRelative ? currentCamera.GetCameraPosition() + cameraPosition : cameraPosition);
}

void SceneCommand_CameraForwardVectorUpdate::Execute(KMGScene*& scene)
{
	if (!scene) return;

	KMGCamera& currentCamera = scene->GetCurrentCamera();

	currentCamera.SetForwardVector(forwardVector);
}

