#include <KMGActor.h>
#include <KMGComponent.h>
#include <KMGActor.h>

using namespace DirectX;

StaticMeshComponent::StaticMeshComponent(const StaticMeshComponent& comp) : KMGComponent(EComponentType::ECT_STATICMESH), meshes(comp.meshes), boundingBoxs(comp.boundingBoxs)
{
	for (KMGStaticMesh& mesh : meshes)
	{
		mesh.bShouldMeshChange = true;
	}
}

StaticMeshComponent::~StaticMeshComponent()
{
	if (owner)
	{

	}
}

void StaticMeshComponent::SetMeshData(std::vector<KMGStaticMesh>&& inMeshes, std::string fileName)
{
	meshFileName = fileName;

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

		mesh.bShouldMeshChange = true;
	}
	
}

void StaticMeshComponent::SetMeshData(KMGStaticMesh&& inMesh, std::string fileName)
{
	meshFileName = fileName;

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

		mesh.bShouldMeshChange = true;
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

void StaticMeshComponent::AxisOnly_CheckHoverAxis(DirectX::XMVECTOR rayOrigin, DirectX::XMVECTOR rayDir, EHoverMode& hoverMode) const
{
	EHoverMode hoverModes[3] = {
		EHoverMode::EHM_Y,
		EHoverMode::EHM_Z,
		EHoverMode::EHM_X,
	};

	hoverMode = EHoverMode::EHM_NONE;

	for (int i=0; i<3; i++)
	{
		const BoundingBox& box = boundingBoxs[i];

		float distance;
		bool result = box.Intersects(rayOrigin, rayDir, distance);

		if (result)
		{
			hoverMode = hoverModes[i];
		}
	}

}

void LightComponent::SetLightColor(DirectX::XMFLOAT4 color)
{
	light.color = color;

	if (owner && owner->HasComponent(EComponentType::ECT_STATICMESH))
	{
		std::vector<KMGStaticMesh>* actorMeshes = nullptr;
		StaticMeshComponent* staticComp = owner->GetComponent<StaticMeshComponent>(EComponentType::ECT_STATICMESH);
		actorMeshes = staticComp->GetMeshes();
		
		for (auto& mesh : *actorMeshes)
		{
			for (auto& vertice : mesh.vertices)
			{
				vertice.Color = color;
			}

			mesh.bShouldMeshChange = true;
		}

	}
}
