#pragma once
#include <KMGDataStructure.h>
#include <KMGActor.h>

class KMGScene;

struct SceneCommandMessage;
struct SceneCommand_ChangeScene;
struct SceneCommand_AddActor;
struct SceneCommand_RemoveActor;
struct SceneCommand_UpdateActor;


enum class CommandMessageType : int
{
	ERC_NONE,
	ERC_CHANGE_SCENE,
	ERC_ADD_ACTOR,
	ERC_REMOVE_ACTOR,
	ERC_UPDATE_ACTOR,

	ERC_CAMERA_POSITION_UPDATE,
	ERC_CAMERA_FORWARD_UPDATE,

	ERC_MAX,
};

struct SceneCommandMessage
{
	CommandMessageType type = CommandMessageType::ERC_NONE;
	virtual void Execute(KMGScene*& scene) = 0;
	virtual ~SceneCommandMessage() = default;
};

namespace KMGCommand
{
	std::unique_ptr<SceneCommandMessage> ChangeScene(KMGScene* scene);
	std::unique_ptr<SceneCommandMessage> AddActor(const std::wstring& name);
	std::unique_ptr<SceneCommandMessage> RemoveActor(const std::wstring& name);

	std::unique_ptr<SceneCommandMessage> UpdateActor(const std::wstring& name, DirectX::XMMATRIX worldMatrix);

	std::unique_ptr<SceneCommandMessage> UpdateCameraPosition_A(DirectX::XMVECTOR CameraPosition);
	std::unique_ptr<SceneCommandMessage> UpdateCameraPosition_R(DirectX::XMVECTOR CameraPosition);

	std::unique_ptr<SceneCommandMessage> UpdateCameraForwardVector(DirectX::XMMATRIX rotYaw, DirectX::XMMATRIX rotPitch);

}


struct SceneCommand_ChangeScene : public SceneCommandMessage
{
	SceneCommand_ChangeScene(KMGScene* scene) : scene(scene) {
		type = CommandMessageType::ERC_CHANGE_SCENE;
	}

	void Execute(KMGScene*& scene) override;

private:
	KMGScene* scene;
};

struct SceneCommand_AddActor : public SceneCommandMessage
{
	SceneCommand_AddActor(const std::wstring& name) : actorName(name) {
		type = CommandMessageType::ERC_ADD_ACTOR;
	}

	void Execute(KMGScene*& scene) override;

private:
	std::wstring actorName;
};

struct SceneCommand_RemoveActor : public SceneCommandMessage
{
	SceneCommand_RemoveActor(const std::wstring& name) : actorName(name) {
		type = CommandMessageType::ERC_REMOVE_ACTOR;
	}

	void Execute(KMGScene*& scene) override;

private:
	std::wstring actorName;
};


struct SceneCommand_UpdateActor : public SceneCommandMessage
{
	SceneCommand_UpdateActor(const std::wstring& name, DirectX::XMMATRIX worldMatrix, bool bRelative) : actorName(name), worldMatrix(worldMatrix), bRelative(bRelative){
		type = CommandMessageType::ERC_UPDATE_ACTOR;
	}

	void Execute(KMGScene*& scene) override;

private:
	std::wstring actorName;
	DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixIdentity();

	// 이게 true이면 현재 값에 행렬을 추가로 곱하고, false이면 그냥 행렬 값을 그대로 대입한다
	bool bRelative;
};


struct SceneCommand_CameraPositionUpdate : public SceneCommandMessage
{
	SceneCommand_CameraPositionUpdate(const DirectX::XMVECTOR& CameraPosition, bool bRelative) : cameraPosition(CameraPosition), bRelative(bRelative){
		type = CommandMessageType::ERC_CAMERA_POSITION_UPDATE;
	}

	void Execute(KMGScene*& scene) override;

private:
	DirectX::XMVECTOR cameraPosition;

	// 이게 true이면 현재 값에 행렬을 추가로 곱하고, false이면 그냥 행렬 값을 그대로 대입한다
	bool bRelative;

};

struct SceneCommand_CameraForwardVectorUpdate : public SceneCommandMessage
{
	SceneCommand_CameraForwardVectorUpdate(DirectX::XMMATRIX rotYaw, DirectX::XMMATRIX rotPitch) : rotYaw(rotYaw), rotPitch(rotPitch){
		type = CommandMessageType::ERC_CAMERA_FORWARD_UPDATE;
	}

	void Execute(KMGScene*& scene) override;

private:
	DirectX::XMMATRIX rotYaw;
	DirectX::XMMATRIX rotPitch;

};