#pragma once

#include "Engine.h"
#include <ECS/SystemManager.h>
#include "Core/CameraSystem/MixingCamera.h"
#include <Gameplay/Components/Transform.h>

namespace sixengine
{
	SYSTEM(MixingCameraSystem, MixingCamera, Transform)
	{
		void Update(EventManager & eventManager, float dt) override
		{
			auto active = m_MixingCamera->m_ActiveCamera->m_Transform;
			auto target = m_MixingCamera->m_TargetCamera->m_Transform;

			auto positionSmooth = 0.75f;
			auto rotationSmooth = 0.5f;

			active->SetWorldPosition(target->GetWorldPosition() * (1.0f - positionSmooth) + active->GetWorldPosition() * positionSmooth);
			active->SetWorldRotation(glm::slerp(active->GetWorldRotation(), target->GetWorldRotation(), rotationSmooth));

			//active->SetWorld(target->GetWorld());
		}
	};
}
