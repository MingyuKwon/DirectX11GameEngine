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

void StaticMeshComponent::SetMergeMode(bool mode)
{
	bMergeMode = mode;

	if (bMergeMode)
	{
		for (KMGStaticMesh& mesh : mergeMeshes)
		{
			mesh.bShouldMeshChange = true;
		}
	}
	else
	{
		for (KMGStaticMesh& mesh : meshes)
		{
			mesh.bShouldMeshChange = true;

		}
	}
}

std::vector<KMGStaticMesh>* StaticMeshComponent::GetMergeMeshes()
{
	if (meshes.size() == 0) return nullptr;

	UpdateMergeMeshes();

	return &mergeMeshes;
}

void StaticMeshComponent::UpdateMergeMeshes()
{
	if (meshes.size() == 0) return;

	bool bUpdateMergeMesh = mergeMeshes.size() == 0;
	for (KMGStaticMesh& mesh : meshes)
	{
		if (mesh.bShouldMeshChange)
		{
			bUpdateMergeMesh = true;
		}

		mesh.bShouldMeshChange = false;
	}

	if (!bUpdateMergeMesh && !bMeshUpdated_forMergeMesh) return;
	
	// texture_normal로 이루어진 이름의 쌍으로 각가의 메시를 합칠 것이다.
	std::unordered_map<std::wstring, KMGStaticMesh> temp;
	for (KMGStaticMesh& mesh : meshes)
	{
		std::wstring key;
		key += mesh.textureFilePath;
		key += DEFAULT_NAME_SEPERATOR;
		key += mesh.normalMapFilePath;

		KMGStaticMesh& targetMesh = temp[key];
		targetMesh.textureFilePath = mesh.textureFilePath;
		targetMesh.normalMapFilePath = mesh.normalMapFilePath;

		int vertexOffset = targetMesh.vertices.size();
		targetMesh.vertices.insert(targetMesh.vertices.end(), mesh.vertices.begin(), mesh.vertices.end());

		for (int index : mesh.indices)
		{
			targetMesh.indices.push_back(index + vertexOffset);
		}
	}

	mergeMeshes.clear();
	for (auto& bucket : temp)
	{
		mergeMeshes.push_back(bucket.second);
	}

	bMeshUpdated_forMergeMesh = false;
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

	bMeshUpdated_forMergeMesh = true;

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

	bMeshUpdated_forMergeMesh = true;
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
