#include "QueueCommand.h"

RenderCommand RenderCommand::MakeAddActorCommand(const KMGActor& actor)
{
	RenderCommand command;
	command.type = RenderCommandtype::ERC_ADD_ACTOR;
	command.actor = actor;

	return command;
}

RenderCommand RenderCommand::MakeRemoveActorCommand(const std::wstring& name)
{
	RenderCommand command;
	command.type = RenderCommandtype::ERC_REMOVE_ACTOR;
	command.actor.name = name;
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
