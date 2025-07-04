#pragma once
#include <KMGDataStructure.h>

class KMGActor;

enum class EComponentType
{
	ECT_NONE,
	ECT_LIGHT,
	ECT_STATICMESH,
	ECT_MAX,

};

class KMGComponent {
public:
	KMGComponent() = default;
	inline KMGComponent(EComponentType componentType) : componentType (componentType){}
	EComponentType componentType = EComponentType::ECT_NONE;

	virtual ~KMGComponent() = default; 

	inline void SetOwner(KMGActor* inOwner)
	{
		owner = inOwner;
	}

protected:
	KMGActor* owner = nullptr;

private:
};

class LightComponent : public KMGComponent {
public:
	LightComponent() : KMGComponent(EComponentType::ECT_LIGHT){}
	inline Light& GetLight() { return light; }

private:
	Light light;

};

class StaticMeshComponent : public KMGComponent {
public:
	StaticMeshComponent() : KMGComponent(EComponentType::ECT_STATICMESH) {}

	inline const std::vector<KMGStaticMesh>* GetMeshes()
	{
		if (meshes.size() == 0) return nullptr;

		return &meshes;
	}

	void SetMeshData(std::vector<KMGStaticMesh>&& inMeshes);
	
private:
	std::vector<KMGStaticMesh> meshes;

};