#include "QueueCommand.h"

RenderCommand RenderCommand::MakeAddActorCommand(KMGActor actor)
{
	RenderCommand command;
	command.type = RenderCommandtype::ERC_ADD_ACTOR;
	command.actor = actor;
	return command;
}

RenderCommandtype RenderCommand::getType()
{
	return type;
}

void RenderCommand::GetActor(KMGActor& outActor)
{
	outActor = actor;
}
