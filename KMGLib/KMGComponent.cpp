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

float StaticMeshComponent::CheckHitWithRay(DirectX::XMVECTOR rayOrigin, DirectX::XMVECTOR rayDir) const
{
	float shortestdistance = -1;

    for (const BoundingBox& box : boundingBoxs)
    {
        float distance;
        bool result = box.Intersects(rayOrigin, rayDir, distance);

        XMFLOAT3 minPoint = {
            box.Center.x - box.Extents.x,
            box.Center.y - box.Extents.y,
            box.Center.z - box.Extents.z
        };

        XMFLOAT3 maxPoint = {
            box.Center.x + box.Extents.x,
            box.Center.y + box.Extents.y,
            box.Center.z + box.Extents.z
        };

        /*
        std::wcout << L"[" << owner->GetName() << L"]\n";
        std::wcout << L"  Box Min: (" << minPoint.x << L", " << minPoint.y << L", " << minPoint.z << L")\n";
        std::wcout << L"  Box Max: (" << maxPoint.x << L", " << maxPoint.y << L", " << maxPoint.z << L")\n";

        std::wcout << L"  Ray Origin: ("
            << XMVectorGetX(rayOrigin) << L", "
            << XMVectorGetY(rayOrigin) << L", "
            << XMVectorGetZ(rayOrigin) << L")\n";

        std::wcout << L"  Ray Direction: ("
            << XMVectorGetX(rayDir) << L", "
            << XMVectorGetY(rayDir) << L", "
            << XMVectorGetZ(rayDir) << L")\n";

        std::wcout << L"  Intersects? " << (result ? L"YES" : L"NO")
            << (result ? (L", Distance: " + std::to_wstring(distance)) : L"") << L"\n";
        
        */
        if (result)
        {
            if (shortestdistance < 0)
            {
                shortestdistance = distance;
            }
            else
            {
                shortestdistance = min(shortestdistance, distance);
            }
        }
    }
	
	return shortestdistance;
}
