#pragma once
#include <string>
#include <vector>
#include <queue>

enum class RenderCommandtype : int
{
	ERC_NONE,
	ERC_RESIZE_VIEWTARGET,
	ERC_MAX,
};

struct RenderCommand 
{
	RenderCommandtype type;
	int viewTargetWidth = 0;
	int viewTargetHeight = 0;
};