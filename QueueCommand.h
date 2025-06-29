#pragma once
#include <KMGDataStructure.h>
#include <KMGActor.h>

class KMGScene;

enum class CommandMessageType : int
{
	ERC_NONE,
	ERC_CHANGE_SCENE,
	ERC_ADD_ACTOR,
	ERC_REMOVE_ACTOR,
	ERC_UPDATE_ACTOR,

	ERC_MAX,
};

struct SceneCommandMessage
{
	CommandMessageType type = CommandMessageType::ERC_NONE;
	virtual void Execute(KMGScene*& scene) = 0;
	virtual ~SceneCommandMessage() = default;
};

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
	SceneCommand_UpdateActor(const std::wstring& name, DirectX::XMMATRIX worldMatrix) : actorName(name), worldMatrix(worldMatrix){
		type = CommandMessageType::ERC_UPDATE_ACTOR;
	}

	void Execute(KMGScene*& scene) override;

private:
	std::wstring actorName;
	DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixIdentity();
};
