#pragma once
#include <KMGDataStructure.h>

class DrawResourceManager
{
public:
	inline void ClearShouldDrawActor() { shouldDrawActor.clear(); }
	inline void ClearShouldDrawDebug() { shouldDrawDebug.clear(); }

	void AddShouldDrawActor(
		std::atomic<bool>& bUpdateReource,
		std::wstring actorName, 
		const std::vector<KMGStaticMesh>* actorMeshes,
		ID3D11DeviceContext* pMainContext,
		bool hasLightComp,
		DirectX::XMMATRIX worldMatrix

	);

	void AddShouldDrawDebug(
		std::wstring meshName,
		const KMGDebugMesh& debugMesh,
		ID3D11DeviceContext* pMainContext,
		DirectX::XMMATRIX worldMatrix

	);

	inline std::unordered_map<std::wstring, DrawResource>& GetDrawResources()
	{
		ArrangeResource();
		return drawActorResources;
	}

	inline std::unordered_map<std::wstring, DrawResource>& GetDebugResources()
	{
		ArrangeResource();
		return drawDebugResources;
	}

	void ArrangeResource();

private:
	std::unordered_map<std::wstring, DrawResource> drawActorResources;
	std::unordered_map<std::wstring, DrawResource> drawDebugResources;

	/// <summary>
	/// key는 액터 이름, 뒤에 int는 그 액터에 존재하는 메시의 개수
	/// 만약 있는데 int가 0이라면 그것은 defualt 메시를 그려야 하는것이다
	/// </summary>
	std::unordered_map<std::wstring, int> shouldDrawActor;
	std::unordered_set<std::wstring> shouldDrawDebug;

	KMGStaticMesh defaultMesh = KMGStaticMesh::CreateDefaultSphereMesh();
};