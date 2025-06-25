#pragma once
#include <KMGDataStructure.h>
#include <KMGActor.h>

enum class RenderCommandtype : int
{
	ERC_NONE,
	ERC_ADD_ACTOR,
	ERC_REMOVE_ACTOR,

	ERC_MAX,
};

struct RenderCommand 
{
public:
	static RenderCommand MakeAddActorCommand(const KMGActor& actor);
	static RenderCommand MakeRemoveActorCommand(const std::wstring& name);

	RenderCommandtype getType();
	void GetActor(KMGActor& outActor);

private:
	RenderCommandtype type;
	KMGActor actor;
};