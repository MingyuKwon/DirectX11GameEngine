#pragma once
#include <KMGDataStructure.h>
#include <DirectXCollision.h> 

#define DRAW_DEBUG_LINE(start, end, color, time) \
    DrawDebug::DrawLine(__FILE__, __LINE__, start, end, color, time)

#define DRAW_DEBUG_SPHERE(origin, radius, color, time) \
    DrawDebug::DrawSphere(__FILE__, __LINE__, origin, radius, color, time)

#define DRAW_DEBUG_BOX(box, color) \
    DrawDebug::DrawBoundingBox(__FILE__, __LINE__, box, color)

#define DRAW_DEBUG_RAY(origin, direction, length, color) \
    DrawDebug::DrawRay(__FILE__, __LINE__, origin, direction, length, color)

#define DRAW_DEBUG_AXES(worldMatrix, axisLength) \
    DrawDebug::DrawAxes(__FILE__, __LINE__, worldMatrix, axisLength)


namespace DrawDebug
{
	// 여기는 아예 외부에서 그림을 그리고 싶을 때 호출하는 것
	void DrawLine(const char* file, int line, DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end, DirectX::XMFLOAT4 color, float time = 0.0f);
	void DrawSphere(const char* file, int line, DirectX::XMFLOAT3 origin, float radius, DirectX::XMFLOAT4 color, float time = 0.0f);

	void DrawBoundingBox(const char* file, int line, const DirectX::BoundingBox box, DirectX::XMFLOAT4 color);
	void DrawRay(const char* file, int line, DirectX::XMFLOAT3 origin, DirectX::XMFLOAT3 direction, float length, DirectX::XMFLOAT4 color);
	void DrawAxes(const char* file, int line, DirectX::XMMATRIX worldMatrix, float axisLength = 1.0f);

	void DrawGizmo(int halfGridSize, float gridSpacing, float yLevel, XMFLOAT4 lineColor);

	// 이건 액터에서 디버그를 그리고 싶을 때 자기가 만들어서 생성하는 것
	KMGDebugMesh MakeDebugLine(DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end, DirectX::XMFLOAT4 color);
	KMGDebugMesh MakeDebugSphere(DirectX::XMFLOAT3 origin, float radius, DirectX::XMFLOAT4 color);

	KMGDebugMesh MakeDebugBoundingBox(const DirectX::BoundingBox box, DirectX::XMFLOAT4 color);
	KMGDebugMesh MakeDebugRay(DirectX::XMFLOAT3 origin, DirectX::XMFLOAT3 direction, float length, DirectX::XMFLOAT4 color);
	KMGDebugMesh MakeDebugAxes(DirectX::XMMATRIX worldMatrix, float axisLength = 1.0f);
	KMGDebugMesh MakeDebugGrid(int halfGridSize, float gridSpacing, float yLevel, XMFLOAT4 lineColor);

}