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
