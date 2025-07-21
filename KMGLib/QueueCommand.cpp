#include "QueueCommand.h"
#include "KMGScene.h"
#include <LoadingManager.h>
#include <KMGDataStructure.h>
#include <DrawDebug.h>
#include <CommandSchedular.h>

#include <iostream>

using namespace DirectX;
using namespace std;

extern CommandSchedular* schedular;
extern KMGScene* currentScene;

namespace KMGCommand
{
	void ChangeScene(KMGScene* scene)
	{
		if (schedular)
		{
			schedular->PushCommand(make_unique<SceneCommand_ChangeScene>(scene));
		}
	}

	void AddActor(std::wstring name)
	{
		if (schedular)
		{
			schedular->PushCommand(make_unique<SceneCommand_Add_Remove_Actor>(name, true));
		}
	}

	void RenameActor(const std::wstring& beforeName, const std::wstring& afterName)
	{
		if (schedular)
		{
			schedular->PushCommand(make_unique<SceneCommand_Rename_Actor>(beforeName, afterName));
		}
	}

	void CameraRayTrace_Select(DirectX::XMVECTOR inRayDirection)
	{
		if (schedular)
		{
			schedular->PushCommand(make_unique<SceneCommand_RayTrace>(inRayDirection));
		}
	}

	void AddStaticMeshComponent(const std::wstring& name)
	{
		if (schedular)
		{
			schedular->PushCommand(make_unique<SceneCommand_Add_Remove_StaticMeshComponent>(name, true));
		}
	}

	void RemoveStaticMeshComponent(const std::wstring& name)
	{
		if (schedular)
		{
			schedular->PushCommand(make_unique<SceneCommand_Add_Remove_StaticMeshComponent>(name, false));
		}
	}

	void MakeVisibleActor(const std::wstring& name, bool bVisible)
	{
		if (schedular)
		{
			schedular->PushCommand(make_unique<SceneCommand_MakeVisible_Actor>(name, bVisible));
		}
		
	}

	void CopyActor(const std::wstring& name)
	{
		if (schedular)
		{
			schedular->PushCommand(make_unique<SceneCommand_CopyActor>(name));
		}
	}

	void RemoveActor(const std::wstring& name)
	{
		if (schedular)
		{
			schedular->PushCommand(make_unique<SceneCommand_Add_Remove_Actor>(name, false));
		}
	}

	void UpdateStaticMesh(const std::wstring& name, const std::string& fileName, std::vector<KMGStaticMesh>&& allMeshes)
	{
		if (schedular)
		{
			schedular->PushCommand(make_unique<SceneCommand_UpdateStaticMesh>(name, fileName, std::move(allMeshes)));
		}
	}

	void UpdateTexture(const std::wstring& name, const std::wstring& beforeTextureName, const std::wstring& textureName)
	{
		if (schedular)
		{
			schedular->PushCommand(make_unique<SceneCommand_UpdateTextureNormal>(name, beforeTextureName, textureName, false));
		}
	}

	void UpdateNormalMap(const std::wstring& name, const std::wstring& beforeTextureName, const std::wstring& textureName)
	{
		if (schedular)
		{
			schedular->PushCommand(make_unique<SceneCommand_UpdateTextureNormal>(name, beforeTextureName, textureName, true));
		}
	}

	void AddLightComponent(const std::wstring& name)
	{
		if (schedular)
		{
			schedular->PushCommand(make_unique<SceneCommand_Add_Remove_LightComponent>(name, true));
		}
	}

	void RemoveLightComponent(const std::wstring& name)
	{
		if (schedular)
		{
			schedular->PushCommand(make_unique<SceneCommand_Add_Remove_LightComponent>(name, false));
		}
	}


	void UpdateLightComponent_Type(const std::wstring& name, int type)
	{
		Light light;
		light.type = type;

		if (schedular)
		{
			schedular->PushCommand(make_unique<SceneCommand_Update_LightComponent>(name, light, ECommandMessageType::ERC_UPDATE_LIGHT_TYPE));
		}
	}

	void UpdateLightComponent_Range(const std::wstring& name, float range)
	{
		Light light;
		light.range = range;

		if (schedular)
		{
			schedular->PushCommand(make_unique<SceneCommand_Update_LightComponent>(name, light, ECommandMessageType::ERC_UPDATE_LIGHT_RANGE));
		}
	}

	void UpdateLightComponent_Intensity(const std::wstring& name, float intensity)
	{
		Light light;
		light.intensity = intensity;

		if (schedular)
		{
			schedular->PushCommand(make_unique<SceneCommand_Update_LightComponent>(name, light, ECommandMessageType::ERC_UPDATE_LIGHT_INTENSITY));
		}
	}

	void UpdateLightComponent_Direction(const std::wstring& name, DirectX::XMFLOAT3 direction)
	{
		Light light;
		light.direction = direction;

		if (schedular)
		{
			schedular->PushCommand(make_unique<SceneCommand_Update_LightComponent>(name, light, ECommandMessageType::ERC_UPDATE_LIGHT_DIRECTION));
		}
	}

	void UpdateLightComponent_Color(const std::wstring& name, DirectX::XMFLOAT4 color)
	{
		Light light;
		light.color = color;

		if (schedular)
		{
			schedular->PushCommand(make_unique<SceneCommand_Update_LightComponent>(name, light, ECommandMessageType::ERC_UPDATE_LIGHT_COLOR));
		}
	}

	//////////////////////////////////////////////////////////////////////
	/// 이제 이 작업은 스케줄러가 아니라 물리 스레드에서 진행이 되어야 한다
	//////////////////////////////////////////////////////////////////////

	void TranslateActor(const std::wstring& name, DirectX::XMVECTOR position)
	{
		if (currentScene)
		{
			float dx = XMVectorGetX(position);
			float dy = XMVectorGetY(position);
			float dz = XMVectorGetZ(position);

			KMGActor* findActor = currentScene->GetActor(name);

			if (findActor)
			{
				findActor->Translate_Kinematic(dx, dy, dz);
			}
		}
	}

	void UpdateActorPosition(const std::wstring& name, DirectX::XMVECTOR position)
	{
		if (currentScene)
		{

			KMGActor* findActor = currentScene->GetActor(name);

			if (findActor)
			{
				findActor->SetPosition_Kinematic(position);
			}
		}
	}

	void RotateActor(const std::wstring& name, DirectX::XMVECTOR aixsVector, float radian)
	{
		if (XMVector3Equal(aixsVector, DirectX::XMVectorZero())) return;

		aixsVector = XMVector3Normalize(aixsVector);

		if (schedular)
		{
			schedular->PushCommand(std::make_unique<SceneCommand_UpdateActorRotation>(name, aixsVector, radian));
		}
	}

	void UpdateActorRotation(const std::wstring& name, DirectX::XMVECTOR rotation, bool bEuler)
	{
		if (schedular)
		{
			schedular->PushCommand(std::make_unique<SceneCommand_UpdateActorRotation>(name, rotation, bEuler));
		}
	}


	void UpdateActorScale(const std::wstring& name, DirectX::XMVECTOR scale)
	{
		if (schedular)
		{
			schedular->PushCommand(std::make_unique<SceneCommand_UpdateActorScale>(name, scale));
		}
	}


	//////////////////////////////////////////////////////////////////////
	/// 이제 이 작업은 스케줄러가 아니라 물리 스레드에서 진행이 되어야 한다
	//////////////////////////////////////////////////////////////////////




	void UpdateCameraPosition_A(DirectX::XMVECTOR CameraPosition)
	{
		if (schedular)
		{
			schedular->PushCommand(std::make_unique<SceneCommand_CameraPositionUpdate>(CameraPosition, false));
		}
	}

	void UpdateCameraPosition_R(DirectX::XMVECTOR CameraPosition)
	{
		if (schedular)
		{
			schedular->PushCommand(std::make_unique<SceneCommand_CameraPositionUpdate>(CameraPosition, true));
		}
	}

	void UpdateCameraForwardVector(float yawAngle, float pitchAngle)
	{
		if (schedular)
		{
			schedular->PushCommand(std::make_unique<SceneCommand_CameraForwardVectorUpdate>(yawAngle, pitchAngle));
		}
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

void SceneCommand_Rename_Actor::Execute(KMGScene*& scene)
{
	if (!scene) return;

	scene->RenameActor(beforeName, afterName);
}

void SceneCommand_RayTrace::Execute(KMGScene*& scene)
{
	if (!scene) return;

	KMGCamera& currentCamera = scene->GetCurrentCamera();
	XMVECTOR cameraOrigin = currentCamera.GetCameraPosition();

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
		if (scene->GetHoverMode() == EHoverMode::EHM_NONE)
		{
			scene->SetFocusActor(L"NONE");
		}
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
			findActor->Translate_Kinematic(dx, dy, dz);
		}
		else
		{
			findActor->SetPosition_Kinematic(dx, dy, dz);
		}
		
	}
}

void SceneCommand_UpdateActorRotation::Execute(KMGScene*& scene)
{
	if (!scene) return;

	KMGActor* findActor = scene->GetActor(actorName);

	if (findActor)
	{
		if (bRelative)
		{
			findActor->Rotate_Kinematic(aixsVector, radian);
		}
		else
		{
			if (bEuler)
			{
				findActor->SetRotation_E_Kinematic(rotation);

			}
			else
			{
				findActor->SetRotation_Q_Kinematic(rotation);

			}
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

		findActor->SetScale_Kinematic(dx, dy, dz);
	}

}

void SceneCommand_MakeVisible_Actor::Execute(KMGScene*& scene)
{
	if (!scene) return;

	KMGActor* findActor = scene->GetActor(actorName);

	if (findActor)
	{
		findActor->SetVisibility(bVisible);
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

	KMGActor* findActor = scene->GetActor(actorName);

	if (findActor)
	{
		StaticMeshComponent* staticComp = 
			findActor->GetComponent<StaticMeshComponent>(EComponentType::ECT_STATICMESH);
		if (staticComp)
		{
			staticComp->SetMeshData(std::move(allMeshes), fileName);
		}
	}
	
}

void SceneCommand_UpdateTextureNormal::Execute(KMGScene*& scene)
{
	if (!scene) return;

	KMGActor* findActor = scene->GetActor(actorName);

	if (findActor)
	{
		if (bNormal)
		{
			findActor->UpdateNormalMap(beforeTextureName, textureName);
		}
		else
		{
			findActor->UpdateTexture(beforeTextureName, textureName);
		}
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

void SceneCommand_CopyActor::Execute(KMGScene*& scene)
{
	if (!scene) return;
	KMGActor* findActor = scene->GetActor(actorName);

	if (findActor == nullptr) return;

	scene->CopyActor(actorName);
}
