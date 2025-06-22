#pragma once
#include <KMGDataStructure.h>
#include <KMGActor.h>

enum class RenderCommandtype : int
{
	ERC_NONE,
	ERC_RESIZE_VIEWTARGET,
	ERC_ADD_ACTOR,
	ERC_MAX,
};

struct RenderCommand 
{
public:
	static RenderCommand MakeResizeViewTargetCommand(int width, int height);
	static RenderCommand MakeAddActorCommand(KMGActor actor);

	RenderCommandtype getType();
	void GetViewTargetWidthHeight(int& outWidth, int& outHeight);
	void GetActor(KMGActor& outActor);

private:
	RenderCommandtype type;
	int viewTargetWidth = 0;
	int viewTargetHeight = 0;
	KMGActor actor;
};