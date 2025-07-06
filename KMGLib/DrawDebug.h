#pragma once
#include <KMGDataStructure.h>
#include <DirectXCollision.h> 

namespace DrawDebug
{
	// 여기는 아예 외부에서 그림을 그리고 싶을 때 호출하는 것
	void DrawLine(std::wstring name, DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end, DirectX::XMFLOAT4 color, float time = 0.0f);
	void DrawSphere(std::wstring name, DirectX::XMFLOAT3 origin, float radius, DirectX::XMFLOAT4 color, float time = 0.0f);

	void DrawBoundingBox(std::wstring name, const DirectX::BoundingBox box, DirectX::XMFLOAT4 color);
	void DrawRay(std::wstring name, DirectX::XMFLOAT3 origin, DirectX::XMFLOAT3 direction, float length, DirectX::XMFLOAT4 color);
	void DrawAxes(std::wstring name, DirectX::XMMATRIX worldMatrix, float axisLength = 1.0f);

	// 이건 액터에서 디버그를 그리고 싶을 때 자기가 만들어서 생성하는 것
	KMGDebugMesh MakeDebugLine(DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end, DirectX::XMFLOAT4 color);
	KMGDebugMesh MakeDebugSphere(DirectX::XMFLOAT3 origin, float radius, DirectX::XMFLOAT4 color);

	KMGDebugMesh MakeDebugBoundingBox(const DirectX::BoundingBox box, DirectX::XMFLOAT4 color);
	KMGDebugMesh MakeDebugRay(DirectX::XMFLOAT3 origin, DirectX::XMFLOAT3 direction, float length, DirectX::XMFLOAT4 color);
	KMGDebugMesh MakeDebugAxes(DirectX::XMMATRIX worldMatrix, float axisLength = 1.0f);

}