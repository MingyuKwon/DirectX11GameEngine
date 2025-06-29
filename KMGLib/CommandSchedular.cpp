#include "CommandSchedular.h"

using namespace std;

void CommandSchedular::ExecuteMessage_InSchedular(KMGScene*& currentScene)
{
	while (true)
	{
		std::unique_ptr<SceneCommandMessage> frontMessage;

		{
			std::lock_guard<std::mutex> lock(schedularMutex);
			if (schedular.empty())
				break;

			frontMessage = std::move(schedular.front());
			schedular.pop();
		}

		frontMessage->Execute(currentScene);
	}
}

void CommandSchedular::PushCommand(std::unique_ptr<SceneCommandMessage> command)
{
	lock_guard<mutex> lock(schedularMutex);
	schedular.push(move(command));
}

