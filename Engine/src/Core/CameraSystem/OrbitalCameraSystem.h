#pragma once

#include <glm/gtx/rotate_vector.hpp>

#include "Engine.h"
#include <ECS/SystemManager.h>
#include "Core/CameraSystem/OrbitalCamera.h"
#include <Gameplay/Components/Transform.h>

namespace sixengine
{
	SYSTEM(OrbitalCameraSystem, OrbitalCamera, Transform)
	{
		void Update(EventManager & eventManager, float dt) override
		{
			m_OrbitalCamera->m_FollowOffset = glm::rotate(m_OrbitalCamera->m_FollowOffset, Input::GetMouseDeltaPosX() * dt, glm::vec3(0.0f, 1.0f, 0.0f));

			if (m_OrbitalCamera->m_FollowTarget)
			{
				auto current = m_Transform->getWorldPosition();
				auto target = m_OrbitalCamera->m_FollowTarget->getWorldPosition() + m_OrbitalCamera->m_FollowOffset;

				auto smooth = 0.75f;
				auto position = target * (1.0f - smooth) + current * smooth;

				m_Transform->SetWorldPosition(position);
			}

			if (m_OrbitalCamera->m_LookTarget)
			{
				m_Transform->LookAt(m_OrbitalCamera->m_LookTarget);
			}
		}
	};
}
