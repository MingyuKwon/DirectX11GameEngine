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

	std::unique_ptr<SceneCommandMessage> UpdateCameraForwardVector(float yawAngle, float pitchAngle)
	{
		return std::make_unique<SceneCommand_CameraForwardVectorUpdate>(yawAngle, pitchAngle);
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

	if (bRelative)
	{
		XMVECTOR forward = currentCamera.GetForwardVector();
		XMVECTOR up = currentCamera.GetUpVector();
		XMVECTOR right = currentCamera.GetRightVector();

		float dx = XMVectorGetX(cameraPosition);
		float dy = XMVectorGetY(cameraPosition);
		float dz = XMVectorGetZ(cameraPosition);

		XMVECTOR worldMove =
			XMVectorScale(right, dx) +
			XMVectorScale(up, dy) +
			XMVectorScale(forward, dz);

		XMVECTOR newPosition = XMVectorAdd(currentCamera.GetCameraPosition(), (dx == 0 && dz == 0) ? cameraPosition : worldMove);
		currentCamera.SetCameraPosition(newPosition);
	}
	else
	{
		currentCamera.SetCameraPosition(cameraPosition);
	}

	
}

void SceneCommand_CameraForwardVectorUpdate::Execute(KMGScene*& scene)
{
	if (!scene) return;

	KMGCamera& currentCamera = scene->GetCurrentCamera();

	XMVECTOR forward = currentCamera.GetForwardVector();
	XMVECTOR up = currentCamera.GetUpVector();
	XMVECTOR right = currentCamera.GetRightVector();

	XMMATRIX yawMat = XMMatrixRotationAxis(up, yawAngle);
	forward = XMVector3TransformNormal(forward, yawMat);

	XMMATRIX pitchMat = XMMatrixRotationAxis(right, pitchAngle);
	forward = XMVector3TransformNormal(forward, pitchMat);

	forward = XMVector3Normalize(forward);
	currentCamera.SetForwardVector(forward);
}

