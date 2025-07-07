#include <KMGActor.h>
#include <KMGComponent.h>
#include <KMGActor.h>

using namespace DirectX;

StaticMeshComponent::~StaticMeshComponent()
{
	if (owner)
	{
		owner->bShouldDrawResourceChange = true;
	}
}

void StaticMeshComponent::SetMeshData(std::vector<KMGStaticMesh>&& inMeshes)
{
	meshes = std::move(inMeshes);
	boundingBoxs.clear();

	for (KMGStaticMesh& mesh : meshes)
	{
		boundingBoxs.emplace_back();

		std::vector<KMGVertex> vertices = mesh.vertices;
		if (vertices.empty()) continue;

		BoundingBox::CreateFromPoints(
			boundingBoxs.back(),                  
			vertices.size(),                      
			&vertices[0].Pos,                     
			sizeof(KMGVertex)                     
		);
	}
	
	if (owner)
	{
		owner->bShouldDrawResourceChange = true;
	}
}

void StaticMeshComponent::SetMeshData(KMGStaticMesh&& inMesh)
{
	meshes.clear();
	meshes.push_back(std::move(inMesh));
	boundingBoxs.clear();

	for (KMGStaticMesh& mesh : meshes)
	{
		boundingBoxs.emplace_back();

		std::vector<KMGVertex> vertices = mesh.vertices;
		if (vertices.empty()) continue;

		BoundingBox::CreateFromPoints(
			boundingBoxs.back(),
			vertices.size(),
			&vertices[0].Pos,
			sizeof(KMGVertex)
		);
	}

	if (owner)
	{
		owner->bShouldDrawResourceChange = true;
	}
}

float StaticMeshComponent::CheckHitWithRay(DirectX::XMVECTOR rayOrigin, DirectX::XMVECTOR rayDir, XMVECTOR& hitPosLocal) const
{
	float shortestDistance = -1.0f;
	XMVECTOR bestHitPos = XMVectorZero();

	for (const BoundingBox& box : boundingBoxs)
	{
		float distance;
		bool result = box.Intersects(rayOrigin, rayDir, distance);

		if (result)
		{
			if (shortestDistance < 0 || distance < shortestDistance)
			{
				shortestDistance = distance;

				// 현재 가장 가까운 충돌 지점 계산
				bestHitPos = rayOrigin + rayDir * distance;
			}
		}
	}

	if (shortestDistance >= 0.0f)
	{
		hitPosLocal = bestHitPos;
	}

	return shortestDistance;
}
