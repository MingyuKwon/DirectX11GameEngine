#include "QueueCommand.h"
#include "KMGScene.h"
#include <UseAssimp.h>
#include <LoadingManager.h>
#include <KMGDataStructure.h>
#include <DrawDebug.h>

#include <iostream>

using namespace DirectX;
using namespace std;

namespace KMGCommand
{
	std::unique_ptr<SceneCommandMessage> ChangeScene(KMGScene* scene)
	{
		return std::make_unique<SceneCommand_ChangeScene>(scene);
	}

	std::unique_ptr<SceneCommandMessage> AddActor(const std::wstring& name)
	{
		return std::make_unique<SceneCommand_Add_Remove_Actor>(name, true);
	}

	std::unique_ptr<SceneCommandMessage> CameraRayTrace(DirectX::XMVECTOR inRayDirection)
	{
		return std::make_unique<SceneCommand_RayTrace>(inRayDirection);
	}

	std::unique_ptr<SceneCommandMessage> AddStaticMeshComponent(const std::wstring& name)
	{
		return std::make_unique<SceneCommand_Add_Remove_StaticMeshComponent>(name, true);
	}

	std::unique_ptr<SceneCommandMessage> RemoveStaticMeshComponent(const std::wstring& name)
	{
		return std::make_unique<SceneCommand_Add_Remove_StaticMeshComponent>(name, false);
	}

	std::unique_ptr<SceneCommandMessage> RemoveActor(const std::wstring& name)
	{
		return std::make_unique<SceneCommand_Add_Remove_Actor>(name, false);
	}

	std::unique_ptr<SceneCommandMessage> UpdateStaticMesh(const std::wstring& name, const std::string& fileName, const std::wstring& textureName, const std::wstring& normalMapName)
	{
		return std::make_unique<SceneCommand_UpdateStaticMesh>(name, fileName, textureName, normalMapName);
	}


	std::unique_ptr<SceneCommandMessage> AddLightComponent(const std::wstring& name)
	{
		return std::make_unique<SceneCommand_Add_Remove_LightComponent>(name, true);
	}

	std::unique_ptr<SceneCommandMessage> RemoveLightComponent(const std::wstring& name)
	{
		return std::make_unique<SceneCommand_Add_Remove_LightComponent>(name, false);
	}


	std::unique_ptr<SceneCommandMessage> UpdateLightComponent_Type(const std::wstring& name, int type)
	{
		Light light;
		light.type = type;

		return std::make_unique<SceneCommand_Update_LightComponent>(name, light, ECommandMessageType::ERC_UPDATE_LIGHT_TYPE);
	}

	std::unique_ptr<SceneCommandMessage> UpdateLightComponent_Range(const std::wstring& name, float range)
	{
		Light light;
		light.range = range;

		return std::make_unique<SceneCommand_Update_LightComponent>(name, light, ECommandMessageType::ERC_UPDATE_LIGHT_RANGE);
	}

	std::unique_ptr<SceneCommandMessage> UpdateLightComponent_Intensity(const std::wstring& name, float intensity)
	{
		Light light;
		light.intensity = intensity;

		return std::make_unique<SceneCommand_Update_LightComponent>(name, light, ECommandMessageType::ERC_UPDATE_LIGHT_INTENSITY);
	}

	std::unique_ptr<SceneCommandMessage> UpdateLightComponent_Direction(const std::wstring& name, DirectX::XMFLOAT3 direction)
	{
		Light light;
		light.direction = direction;

		return std::make_unique<SceneCommand_Update_LightComponent>(name, light, ECommandMessageType::ERC_UPDATE_LIGHT_DIRECTION);
	}

	std::unique_ptr<SceneCommandMessage> UpdateLightComponent_Color(const std::wstring& name, DirectX::XMFLOAT4 color)
	{
		Light light;
		light.color = color;

		return std::make_unique<SceneCommand_Update_LightComponent>(name, light, ECommandMessageType::ERC_UPDATE_LIGHT_COLOR);
	}



	std::unique_ptr<SceneCommandMessage> TranslateActor(const std::wstring& name, DirectX::XMVECTOR position)
	{
		return std::make_unique<SceneCommand_UpdateActorPosition>(name, position, true);
	}

	std::unique_ptr<SceneCommandMessage> UpdateActorPosition(const std::wstring& name, DirectX::XMVECTOR position)
	{
		return std::make_unique<SceneCommand_UpdateActorPosition>(name, position, false);
	}

	std::unique_ptr<SceneCommandMessage> RotateActor(const std::wstring& name, DirectX::XMVECTOR rotation)
	{
		return std::make_unique<SceneCommand_UpdateActorRotation>(name, rotation, true);
	}

	std::unique_ptr<SceneCommandMessage> UpdateActorRotation(const std::wstring& name, DirectX::XMVECTOR rotation)
	{
		return std::make_unique<SceneCommand_UpdateActorRotation>(name, rotation, false);
	}

	std::unique_ptr<SceneCommandMessage> UpdateActorScale(const std::wstring& name, DirectX::XMVECTOR scale)
	{
		return std::make_unique<SceneCommand_UpdateActorScale>(name, scale);
	}


	std::unique_ptr<SceneCommandMessage> UpdateCameraPosition_A(DirectX::XMVECTOR CameraPosition)
	{
		return std::make_unique<SceneCommand_CameraPositionUpdate>(CameraPosition, false);
	}

	std::unique_ptr<SceneCommandMessage> UpdateCameraPosition_R(DirectX::XMVECTOR CameraPosition)
	{
		return std::make_unique<SceneCommand_CameraPositionUpdate>(CameraPosition, true);
	}

	std::unique_ptr<SceneCommandMessage> UpdateCameraForwardVector(float yawAngle, float pitchAngle)
	{
		return std::make_unique<SceneCommand_CameraForwardVectorUpdate>(yawAngle, pitchAngle);
	}

}

void SceneCommand_ChangeScene::Execute(KMGScene*& scene)
{
	if (scene)
	{
		delete scene;
		scene = nullptr;
	}

	scene = this->scene;
}

void SceneCommand_Add_Remove_Actor::Execute(KMGScene*& scene)
{
	if (!scene) return;

	if (bAdd)
	{
		scene->CreateActor(actorName);
	}
	else
	{
		scene->EraseActor(actorName);
	}

}

void SceneCommand_RayTrace::Execute(KMGScene*& scene)
{
	if (!scene) return;

	KMGCamera& currentCamera = scene->GetCurrentCamera();
	XMVECTOR cameraOrigin = currentCamera.GetCameraPosition();

	/// 테스트 용으로 그리기
	//XMVECTOR endVec = cameraOrigin + cameraRayDirection;

	//XMFLOAT3 start, end;
	//XMStoreFloat3(&start, cameraOrigin);
	//XMStoreFloat3(&end, endVec);

	//float delayTime = 2.f;
	//DRAW_DEBUG_LINE(start, end, XMFLOAT4(0, 0, 0, 1), delayTime);
	//DRAW_DEBUG_SPHERE(start, 0.3, XMFLOAT4(1, 0, 0, 1), delayTime);
	/// 테스트 용으로 그리기

	KMGActor* closestActor = nullptr;
	float closestDistance = -1;

	const unordered_map<wstring, unique_ptr<KMGActor>>& actors = scene->getAllActors();
	for (auto& bucket : actors)
	{
		KMGActor* actor = bucket.second.get();

		float result = actor->RayTraceHit(cameraOrigin, cameraRayDirection);
		if (result >= 0)
		{
			if (closestActor == nullptr)
			{
				closestActor = actor;
				closestDistance = result;
			}
			else
			{
				if (result < closestDistance)
				{
					closestDistance = result;
					closestActor = actor;
				}
			}
		}
	}
	
	if (closestActor)
	{
		scene->SetFocusActor(closestActor->GetName());
	}
	else
	{
		scene->SetFocusActor(L"NONE");
	}
}

void SceneCommand_CameraPositionUpdate::Execute(KMGScene*& scene)
{
	if (!scene) return;

	KMGCamera& currentCamera = scene->GetCurrentCamera();

	if (bRelative)
	{
		XMVECTOR forward = currentCamera.GetForwardVector();
		XMVECTOR up = currentCamera.GetUpVector();
		XMVECTOR right = currentCamera.GetRightVector();

		float dx = XMVectorGetX(cameraPosition);
		float dy = XMVectorGetY(cameraPosition);
		float dz = XMVectorGetZ(cameraPosition);

		XMVECTOR worldMove =
			XMVectorScale(right, dx) +
			XMVectorScale(up, dy) +
			XMVectorScale(forward, dz);

		XMVECTOR newPosition = XMVectorAdd(currentCamera.GetCameraPosition(), (dx == 0 && dz == 0) ? cameraPosition : worldMove);
		currentCamera.SetCameraPosition(newPosition);
	}
	else
	{
		currentCamera.SetCameraPosition(cameraPosition);
	}


}

void SceneCommand_CameraForwardVectorUpdate::Execute(KMGScene*& scene)
{
	if (!scene) return;

	KMGCamera& currentCamera = scene->GetCurrentCamera();

	XMVECTOR forward = currentCamera.GetForwardVector();
	XMVECTOR up = currentCamera.GetUpVector();
	XMVECTOR right = currentCamera.GetRightVector();

	XMMATRIX yawMat = XMMatrixRotationAxis(up, yawAngle);
	forward = XMVector3TransformNormal(forward, yawMat);

	XMMATRIX pitchMat = XMMatrixRotationAxis(right, pitchAngle);
	forward = XMVector3TransformNormal(forward, pitchMat);

	forward = XMVector3Normalize(forward);
	currentCamera.SetForwardVector(forward);
}


void SceneCommand_UpdateActorPosition::Execute(KMGScene*& scene)
{
	if (!scene) return;
	
	KMGActor* findActor = scene->GetActor(actorName);

	if (findActor)
	{
		float dx = XMVectorGetX(position);
		float dy = XMVectorGetY(position);
		float dz = XMVectorGetZ(position);

		if (bRelative)
		{
			findActor->Translate(dx, dy, dz);
		}
		else
		{
			findActor->SetPosition(dx, dy, dz);
		}
	}
}

void SceneCommand_UpdateActorRotation::Execute(KMGScene*& scene)
{
	if (!scene) return;

	KMGActor* findActor = scene->GetActor(actorName);

	if (findActor)
	{
		float dx = XMVectorGetX(rotation);
		float dy = XMVectorGetY(rotation);
		float dz = XMVectorGetZ(rotation);

		if (bRelative)
		{
			findActor->Rotate(dx, dy, dz);
		}
		else
		{
			findActor->SetRotation(dx, dy, dz);
		}
	}
}

void SceneCommand_UpdateActorScale::Execute(KMGScene*& scene)
{
	if (!scene) return;

	KMGActor* findActor = scene->GetActor(actorName);

	if (findActor)
	{
		float dx = XMVectorGetX(scale);
		float dy = XMVectorGetY(scale);
		float dz = XMVectorGetZ(scale);

		findActor->SetScale(dx, dy, dz);
	}

}

void SceneCommand_Add_Remove_StaticMeshComponent::Execute(KMGScene*& scene)
{
	if (!scene) return;

	KMGActor* findActor = scene->GetActor(actorName);

	if (findActor)
	{
		if (bAdd)
		{
			StaticMeshComponent* staticComponent = new StaticMeshComponent();
			bool result = findActor->SetComponent(staticComponent);

			if (!result)
			{
				delete staticComponent;
				staticComponent = nullptr;
			}
		}
		else
		{
			findActor->RemoveComponent(EComponentType::ECT_STATICMESH);
		}
	}
}

void SceneCommand_UpdateStaticMesh::Execute(KMGScene*& scene)
{
	if (!scene) return;

	LoadingManager loading(ELoadingType::ELT_IMPORT_MESH);
	KMGActor* findActor = scene->GetActor(actorName);

	if (findActor)
	{
		LoadModelToActor(fileName, *findActor, loading, textureName, normalMapName);
	}
	else
	{
		loading.StopLoading();
	}

}

void SceneCommand_Add_Remove_LightComponent::Execute(KMGScene*& scene)
{
	if (!scene) return;

	KMGActor* findActor = scene->GetActor(actorName);

	if (findActor)
	{
		if (bAdd)
		{
			LightComponent* lightComponent = new LightComponent();
			
			bool result = findActor->SetComponent(lightComponent);
			lightComponent->SetLightColor(XMFLOAT4(0.8, 0.8, 0, 1));

			if (!result) 
			{
				delete lightComponent;
				lightComponent = nullptr;
			}

		}
		else
		{
			findActor->RemoveComponent(EComponentType::ECT_LIGHT);
		}
	}
}

void SceneCommand_Update_LightComponent::Execute(KMGScene*& scene)
{
	if (!scene) return;

	KMGActor* findActor = scene->GetActor(actorName);

	if (findActor)
	{
		LightComponent* comp = findActor->GetComponent<LightComponent>(EComponentType::ECT_LIGHT);
		if (comp)
		{
			switch (type)
			{
			case ECommandMessageType::ERC_UPDATE_LIGHT_TYPE:
				comp->SetLightType(light.type);

				break;
			case ECommandMessageType::ERC_UPDATE_LIGHT_RANGE:
				comp->SetLightRange(light.range);
				break;
			case ECommandMessageType::ERC_UPDATE_LIGHT_INTENSITY:
				comp->SetLightIntensity(light.intensity);

				break;
			case ECommandMessageType::ERC_UPDATE_LIGHT_DIRECTION:
				comp->SetLightDirection(light.direction);

				break;
			case ECommandMessageType::ERC_UPDATE_LIGHT_COLOR:
				comp->SetLightColor(light.color);

				break;
			default:
				break;
			}

		}
	}
}

void SceneCommand_DrawDebug::Execute(KMGScene*& scene)
{
	if (!scene) return;
	
	scene->AddDebugMesh(debugMeshName, mesh, time);

}
