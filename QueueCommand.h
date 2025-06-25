#pragma once
#include <KMGDataStructure.h>
#include <KMGActor.h>

enum class RenderCommandtype : int
{
	ERC_NONE,
	ERC_ADD_ACTOR,
	ERC_MAX,
};

struct RenderCommand 
{
public:
	static RenderCommand MakeAddActorCommand(const KMGActor& actor);

	RenderCommandtype getType();
	void GetActor(KMGActor& outActor);

private:
	RenderCommandtype type;
	KMGActor actor;
};