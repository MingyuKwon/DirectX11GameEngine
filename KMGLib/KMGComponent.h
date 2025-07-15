#pragma once
#include <KMGDataStructure.h>
#include <DirectXCollision.h> 

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

	inline void SetLightType(int type)
	{
		light.type = type;
	}

	inline void SetLightRange(float range)
	{
		light.range = range;
	}

	inline void SetLightIntensity(float intensity)
	{
		light.intensity = intensity;
	}

	inline void SetLightDirection(DirectX::XMFLOAT3 direction)
	{
		light.direction = direction;
	}

	void SetLightColor(DirectX::XMFLOAT4 color);

private:
	Light light;

};

class StaticMeshComponent : public KMGComponent {
public:
	StaticMeshComponent() : KMGComponent(EComponentType::ECT_STATICMESH) {}
	StaticMeshComponent(const StaticMeshComponent& comp);

	virtual ~StaticMeshComponent();

	std::string meshFileName = "Default Mesh";

	inline bool GetMergeMode()
	{
		return bMergeMode;
	}

	void SetMergeMode(bool mode);
	

	inline std::vector<KMGStaticMesh>* GetMeshes()
	{
		if (meshes.size() == 0) return nullptr;

		return &meshes;
	}

	std::vector<KMGStaticMesh>* GetMergeMeshes();
	void UpdateMergeMeshes();

	inline const std::vector<DirectX::BoundingBox>* GetBoundingBoxs()
	{
		if (boundingBoxs.size() == 0) return nullptr;

		return &boundingBoxs;
	}

	void SetMeshData(std::vector<KMGStaticMesh>&& inMeshes, std::string fileName = "Default Mesh");
	void SetMeshData(KMGStaticMesh&& inMesh, std::string fileName = "Default Mesh");

	float CheckHitWithRay(DirectX::XMVECTOR rayOrigin, DirectX::XMVECTOR rayDir, DirectX::XMVECTOR& hitPosLocal) const;

	void AxisOnly_CheckHoverAxis(DirectX::XMVECTOR rayOrigin, DirectX::XMVECTOR rayDir, EHoverMode& hoverMode) const;

private:
	// 기본적으로 메시가 업데이트 되면, 일반 메시는 업데이트 되도, 합쳐진 메시는 자동으로 업데이트가 안된다. 
	// 따라서 따로 bool을 둬서 체크하게 한다
	bool bMeshUpdated_forMergeMesh = false;
	bool bMergeMode = false;

	std::vector<KMGStaticMesh> meshes;
	std::vector<KMGStaticMesh> mergeMeshes;

	std::vector<DirectX::BoundingBox> boundingBoxs;
};