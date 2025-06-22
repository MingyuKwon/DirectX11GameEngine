#include "QueueCommand.h"

RenderCommand RenderCommand::MakeResizeViewTargetCommand(int width, int height)
{
	RenderCommand command;
	command.type = RenderCommandtype::ERC_RESIZE_VIEWTARGET;
	command.viewTargetWidth = width;
	command.viewTargetHeight = height;

	return command;
}

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

void RenderCommand::GetViewTargetWidthHeight(int& outWidth, int& outHeight)
{
	outWidth = viewTargetWidth;
	outHeight = viewTargetHeight;
}

void RenderCommand::GetActor(KMGActor& outActor)
{
	outActor = actor;
}
