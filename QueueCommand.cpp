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
