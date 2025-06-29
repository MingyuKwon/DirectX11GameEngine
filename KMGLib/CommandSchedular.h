#pragma once
#include "QueueCommand.h"
#include <mutex>
#include <queue>

class CommandSchedular
{
public:
	void ExecuteMessage_InSchedular(KMGScene*& currentScene);
	void PushCommand(std::unique_ptr<SceneCommandMessage> command);
private:
	std::mutex schedularMutex;
	std::queue<std::unique_ptr<SceneCommandMessage>> schedular;

	
};