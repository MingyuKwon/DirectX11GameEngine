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

	ERC_CLICK_SCENE,

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
	std::unique_ptr<SceneCommandMessage> ChangeScene(KMGScene* scene);
	std::unique_ptr<SceneCommandMessage> AddActor(const std::wstring& name);

	std::unique_ptr<SceneCommandMessage> ClickScene(float clickX, float clickY, float screenWidth, float screenHeight);

	std::unique_ptr<SceneCommandMessage> AddStaticMeshComponent(const std::wstring& name);
	std::unique_ptr<SceneCommandMessage> RemoveStaticMeshComponent(const std::wstring& name);

	std::unique_ptr<SceneCommandMessage> RemoveActor(const std::wstring& name);
	std::unique_ptr<SceneCommandMessage> UpdateStaticMesh(const std::wstring& name, const std::string& fileName);

	std::unique_ptr<SceneCommandMessage> AddLightComponent(const std::wstring& name);
	std::unique_ptr<SceneCommandMessage> RemoveLightComponent(const std::wstring& name);

	std::unique_ptr<SceneCommandMessage> UpdateLightComponent_Type(const std::wstring& name, int type);
	std::unique_ptr<SceneCommandMessage> UpdateLightComponent_Range(const std::wstring& name, float range);
	std::unique_ptr<SceneCommandMessage> UpdateLightComponent_Intensity(const std::wstring& name, float intensity);
	std::unique_ptr<SceneCommandMessage> UpdateLightComponent_Direction(const std::wstring& name, DirectX::XMFLOAT3 direction);
	std::unique_ptr<SceneCommandMessage> UpdateLightComponent_Color(const std::wstring& name, DirectX::XMFLOAT4 color);


	std::unique_ptr<SceneCommandMessage> TranslateActor(const std::wstring& name, DirectX::XMVECTOR position);
	std::unique_ptr<SceneCommandMessage> UpdateActorPosition(const std::wstring& name, DirectX::XMVECTOR position);

	std::unique_ptr<SceneCommandMessage> RotateActor(const std::wstring& name, DirectX::XMVECTOR rotation);
	std::unique_ptr<SceneCommandMessage> UpdateActorRotation(const std::wstring& name, DirectX::XMVECTOR rotation);

	std::unique_ptr<SceneCommandMessage> UpdateActorScale(const std::wstring& name, DirectX::XMVECTOR scale);

	std::unique_ptr<SceneCommandMessage> UpdateCameraPosition_A(DirectX::XMVECTOR CameraPosition);
	std::unique_ptr<SceneCommandMessage> UpdateCameraPosition_R(DirectX::XMVECTOR CameraPosition);

	std::unique_ptr<SceneCommandMessage> UpdateCameraForwardVector(float yawAngle, float pitchAngle);
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

struct SceneCommand_ClickScene : public SceneCommandMessage
{
	SceneCommand_ClickScene(float clickX, float clickY, float screenWidth, float screenHeight) : clickX(clickX), clickY(clickY), screenWidth(screenWidth), screenHeight(screenHeight) {
		type = ECommandMessageType::ERC_CLICK_SCENE;
	}

	void Execute(KMGScene*& scene) override;

private:
	float clickX;
	float clickY;
	float screenWidth;
	float screenHeight;

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
	SceneCommand_UpdateStaticMesh(const std::wstring& name, const std::string& fileName) : actorName(name), fileName(fileName){
		type = ECommandMessageType::ERC_UPDATE_ACTOR;
	}

	void Execute(KMGScene*& scene) override;

private:
	std::wstring actorName;
	std::string fileName;
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
	SceneCommand_UpdateActorRotation(const std::wstring& name, DirectX::XMVECTOR rotation, bool bRelative) : actorName(name), rotation(rotation), bRelative(bRelative) {
		type = ECommandMessageType::ERC_UPDATE_ACTOR;
	}

	void Execute(KMGScene*& scene) override;

private:
	std::wstring actorName;
	DirectX::XMVECTOR rotation;

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