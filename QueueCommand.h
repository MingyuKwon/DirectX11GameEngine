#pragma once
#include <KMGDataStructure.h>
#include <KMGActor.h>

class KMGScene;

struct SceneCommandMessage;
struct SceneCommand_ChangeScene;
struct SceneCommand_AddActor;
struct SceneCommand_RemoveActor;
struct SceneCommand_UpdateActorPosition;


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


struct SceneCommand_UpdateActorPosition : public SceneCommandMessage
{
	SceneCommand_UpdateActorPosition(const std::wstring& name, DirectX::XMVECTOR position, bool bRelative) : actorName(name), position(position), bRelative(bRelative){
		type = CommandMessageType::ERC_UPDATE_ACTOR;
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
		type = CommandMessageType::ERC_UPDATE_ACTOR;
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
		type = CommandMessageType::ERC_UPDATE_ACTOR;
	}

	void Execute(KMGScene*& scene) override;

private:
	std::wstring actorName;
	DirectX::XMVECTOR scale;
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
	SceneCommand_CameraForwardVectorUpdate(float yawAngle, float pitchAngle) : yawAngle(yawAngle), pitchAngle(pitchAngle){
		type = CommandMessageType::ERC_CAMERA_FORWARD_UPDATE;
	}

	void Execute(KMGScene*& scene) override;

private:
	float yawAngle;
	float pitchAngle;

};