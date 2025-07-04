#include <KMGActor.h>
#include <KMGComponent.h>
#include <KMGActor.h>

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
	if (owner)
	{
		owner->bShouldDrawResourceChange = true;
	}
}
