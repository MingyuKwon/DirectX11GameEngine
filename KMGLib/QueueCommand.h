#pragma once
#include <KMGDataStructure.h>
#include <KMGActor.h>

class KMGScene;

struct SceneCommandMessage;
struct SceneCommand_ChangeScene;
struct SceneCommand_Add_Remove_Actor;
struct SceneCommand_RemoveActor;
struct SceneCommand_UpdateActorPosition;

enum class ECommandMessageType : int
{
	ERC_NONE,
	ERC_CHANGE_SCENE,

	ERC_RAY_TRACE,

	ERC_ADD_ACTOR,
	ERC_REMOVE_ACTOR,
	ERC_UPDATE_ACTOR,

	ERC_ADD_COMPONENT,
	ERC_REMOVE_COMPONENT,

	ERC_UPDATE_LIGHT_TYPE,
	ERC_UPDATE_LIGHT_RANGE,
	ERC_UPDATE_LIGHT_INTENSITY,
	ERC_UPDATE_LIGHT_DIRECTION,
	ERC_UPDATE_LIGHT_COLOR,

	ERC_UPDATE_COMPONENT,

	ERC_CAMERA_POSITION_UPDATE,
	ERC_CAMERA_FORWARD_UPDATE,

	ERC_DRAW_DEBUG,


	ERC_MAX,
};

struct SceneCommandMessage
{
	ECommandMessageType type = ECommandMessageType::ERC_NONE;
	virtual void Execute(KMGScene*& scene) = 0;
	virtual ~SceneCommandMessage() = default;
};

namespace KMGCommand
{
	void ChangeScene(KMGScene* scene);
	void AddActor(std::wstring name = DEFAULT_ACTOR_NAME);

	void RenameActor(const std::wstring& beforeName, const std::wstring& afterName);

	void CameraRayTrace_Select(DirectX::XMVECTOR inRayDirection);

	void AddStaticMeshComponent(const std::wstring& name);
	void RemoveStaticMeshComponent(const std::wstring& name);

	void RemoveActor(const std::wstring& name);
	void UpdateStaticMesh(const std::wstring& name, const std::string& fileName, const std::wstring& textureName = DEFAULT_TEXTURE_FILEPATH, const std::wstring& normalMapName = DEFAULT_NORMAL_FILEPATH);

	void UpdateTexture(const std::wstring& name, const std::wstring& beforeTextureName, const std::wstring& textureName);
	void UpdateNormalMap(const std::wstring& name, const std::wstring& beforeTextureName, const std::wstring& textureName);

	void AddLightComponent(const std::wstring& name);
	void RemoveLightComponent(const std::wstring& name);

	void UpdateLightComponent_Type(const std::wstring& name, int type);
	void UpdateLightComponent_Range(const std::wstring& name, float range);
	void UpdateLightComponent_Intensity(const std::wstring& name, float intensity);
	void UpdateLightComponent_Direction(const std::wstring& name, DirectX::XMFLOAT3 direction);
	void UpdateLightComponent_Color(const std::wstring& name, DirectX::XMFLOAT4 color);


	void TranslateActor(const std::wstring& name, DirectX::XMVECTOR position);
	void UpdateActorPosition(const std::wstring& name, DirectX::XMVECTOR position);

	void RotateActor(const std::wstring& name, DirectX::XMVECTOR aixsVector, float radian);
	void UpdateActorRotation(const std::wstring& name, DirectX::XMVECTOR rotation);

	void UpdateActorScale(const std::wstring& name, DirectX::XMVECTOR scale);

	void UpdateCameraPosition_A(DirectX::XMVECTOR CameraPosition);
	void UpdateCameraPosition_R(DirectX::XMVECTOR CameraPosition);

	void UpdateCameraForwardVector(float yawAngle, float pitchAngle);
}


struct SceneCommand_ChangeScene : public SceneCommandMessage
{
	SceneCommand_ChangeScene(KMGScene* scene) : scene(scene) {
		type = ECommandMessageType::ERC_CHANGE_SCENE;
	}

	void Execute(KMGScene*& scene) override;

private:
	KMGScene* scene;
};

struct SceneCommand_RayTrace : public SceneCommandMessage
{
	SceneCommand_RayTrace(DirectX::XMVECTOR inRayDirection) : cameraRayDirection(inRayDirection){
		type = ECommandMessageType::ERC_RAY_TRACE;
	}

	void Execute(KMGScene*& scene) override;

private:
	DirectX::XMVECTOR cameraRayDirection;
};

struct SceneCommand_Rename_Actor : public SceneCommandMessage
{
	SceneCommand_Rename_Actor(const std::wstring& beforeName, const std::wstring& afterName) : beforeName(beforeName), afterName(afterName) {
		type = ECommandMessageType::ERC_UPDATE_ACTOR;
	}

	void Execute(KMGScene*& scene) override;

private:
	std::wstring beforeName;
	std::wstring afterName;

};

struct SceneCommand_Add_Remove_Actor : public SceneCommandMessage
{
	SceneCommand_Add_Remove_Actor(const std::wstring& name, bool bAdd) : actorName(name), bAdd(bAdd){
		type = bAdd ? ECommandMessageType::ERC_ADD_ACTOR : ECommandMessageType::ERC_REMOVE_ACTOR;
	}

	void Execute(KMGScene*& scene) override;

private:
	std::wstring actorName;
	bool bAdd = true;
};

struct SceneCommand_Add_Remove_StaticMeshComponent : public SceneCommandMessage
{
	SceneCommand_Add_Remove_StaticMeshComponent(const std::wstring& name, bool bAdd) : actorName(name), bAdd(bAdd) {
		type = bAdd ? ECommandMessageType::ERC_ADD_COMPONENT : ECommandMessageType::ERC_REMOVE_COMPONENT;
	}

	void Execute(KMGScene*& scene) override;

private:
	std::wstring actorName;
	bool bAdd = true;
};

struct SceneCommand_UpdateStaticMesh : public SceneCommandMessage
{
	SceneCommand_UpdateStaticMesh(const std::wstring& name, const std::string& fileName, const std::wstring& textureName, const std::wstring& normalMapName)
		: actorName(name), fileName(fileName), textureName(textureName), normalMapName(normalMapName)
	{
		type = ECommandMessageType::ERC_UPDATE_ACTOR;
	}

	void Execute(KMGScene*& scene) override;

private:
	std::wstring actorName;
	std::string fileName;
	std::wstring textureName;
	std::wstring normalMapName;
};

struct SceneCommand_UpdateTextureNormal : public SceneCommandMessage
{
	SceneCommand_UpdateTextureNormal(const std::wstring& name, const std::wstring& beforeTextureName, const std::wstring& textureName, bool bNormal)
		: actorName(name), beforeTextureName(beforeTextureName), textureName(textureName), bNormal(bNormal)
	{
		type = ECommandMessageType::ERC_UPDATE_ACTOR;
	}

	void Execute(KMGScene*& scene) override;

private:
	std::wstring actorName;
	std::wstring beforeTextureName;
	std::wstring textureName;

	bool bNormal = false;
};

struct SceneCommand_Add_Remove_LightComponent : public SceneCommandMessage
{
	SceneCommand_Add_Remove_LightComponent(const std::wstring& name, bool bAdd) : actorName(name), bAdd(bAdd) {
		type = bAdd ? ECommandMessageType::ERC_ADD_COMPONENT : ECommandMessageType::ERC_REMOVE_COMPONENT;
	}

	void Execute(KMGScene*& scene) override;

private:
	std::wstring actorName;
	bool bAdd = true;
};


struct SceneCommand_Update_LightComponent : public SceneCommandMessage
{
	SceneCommand_Update_LightComponent(const std::wstring& name, Light light, ECommandMessageType updateType) : actorName(name), light(light){
		type = updateType;
	}

	void Execute(KMGScene*& scene) override;

private:
	std::wstring actorName;
	Light light;
};



struct SceneCommand_UpdateActorPosition : public SceneCommandMessage
{
	SceneCommand_UpdateActorPosition(const std::wstring& name, DirectX::XMVECTOR position, bool bRelative) : actorName(name), position(position), bRelative(bRelative){
		type = ECommandMessageType::ERC_UPDATE_ACTOR;
	}

	void Execute(KMGScene*& scene) override;

private:
	std::wstring actorName;
	DirectX::XMVECTOR position;

	// 이게 true이면 현재 값에 행렬을 추가로 곱하고, false이면 그냥 행렬 값을 그대로 대입한다
	bool bRelative;
};

struct SceneCommand_UpdateActorRotation : public SceneCommandMessage
{
	SceneCommand_UpdateActorRotation(const std::wstring& name, DirectX::XMVECTOR quatRotation, bool bRelative) : actorName(name), quatRotation(quatRotation), bRelative(bRelative) {
		type = ECommandMessageType::ERC_UPDATE_ACTOR;
	}

	void Execute(KMGScene*& scene) override;

private:
	std::wstring actorName;
	DirectX::XMVECTOR quatRotation;

	// 이게 true이면 현재 값에 행렬을 추가로 곱하고, false이면 그냥 행렬 값을 그대로 대입한다
	bool bRelative;
};

struct SceneCommand_UpdateActorScale : public SceneCommandMessage
{
	SceneCommand_UpdateActorScale(const std::wstring& name, DirectX::XMVECTOR scale) : actorName(name), scale(scale) {
		type = ECommandMessageType::ERC_UPDATE_ACTOR;
	}

	void Execute(KMGScene*& scene) override;

private:
	std::wstring actorName;
	DirectX::XMVECTOR scale;
};



struct SceneCommand_CameraPositionUpdate : public SceneCommandMessage
{
	SceneCommand_CameraPositionUpdate(const DirectX::XMVECTOR& CameraPosition, bool bRelative) : cameraPosition(CameraPosition), bRelative(bRelative){
		type = ECommandMessageType::ERC_CAMERA_POSITION_UPDATE;
	}

	void Execute(KMGScene*& scene) override;

private:
	DirectX::XMVECTOR cameraPosition;

	// 이게 true이면 현재 값에 행렬을 추가로 곱하고, false이면 그냥 행렬 값을 그대로 대입한다
	bool bRelative;

};

struct SceneCommand_CameraForwardVectorUpdate : public SceneCommandMessage
{
	SceneCommand_CameraForwardVectorUpdate(float yawAngle, float pitchAngle) : yawAngle(yawAngle), pitchAngle(pitchAngle){
		type = ECommandMessageType::ERC_CAMERA_FORWARD_UPDATE;
	}

	void Execute(KMGScene*& scene) override;

private:
	float yawAngle;
	float pitchAngle;

};

struct SceneCommand_DrawDebug : public SceneCommandMessage
{
	SceneCommand_DrawDebug(std::wstring debugMeshName, KMGDebugMesh mesh, float time = 0) : debugMeshName(debugMeshName), mesh(mesh), time(time){
		type = ECommandMessageType::ERC_DRAW_DEBUG;
	}

	void Execute(KMGScene*& scene) override;

private:
	std::wstring debugMeshName;
	KMGDebugMesh mesh;
	float time;
};