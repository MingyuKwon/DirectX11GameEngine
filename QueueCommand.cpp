#include "QueueCommand.h"
#include "KMGScene.h"
#include <iostream>

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
		findActor->UpdateWorldMatrix(findActor->getWorldMatrix() * worldMatrix);
	}
}


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
		return std::make_unique<SceneCommand_UpdateActor>(name, worldMatrix);
	}
}