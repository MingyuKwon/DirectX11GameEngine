#include <KMGActor.h>
#include <KMGComponent.h>
#include <KMGActor.h>

void StaticMeshComponent::SetMeshData(std::vector<KMGStaticMesh>&& inMeshes)
{
	meshes = std::move(inMeshes);
	if (owner)
	{
		owner->bShouldDrawResourceChange = true;
	}
}
