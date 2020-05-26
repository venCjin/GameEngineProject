#pragma once

#include <ECS/SystemManager.h>

#include <Core/Input.h>
#include "Core/CameraSystem/FlyingCamera.h"
#include <Gameplay/Components/Transform.h>

#include <glm/gtx/string_cast.hpp>

namespace sixengine
{
	SYSTEM(FlyingCameraSystem, FlyingCamera, Transform)
	{
		void UpdatePosition(float dt)
		{
			glm::vec3 direction = glm::vec3(0.0f);

			direction += m_Transform->GetForward() * Input::GetAxis(Axis::KEYBOARD_VERTICAL);
			direction += m_Transform->GetRight() * Input::GetAxis(Axis::KEYBOARD_HORIZONTAL);

			if (Input::IsKeyActive(KeyCode::E))
			{
				direction += glm::vec3(0.0f, 1.0f, 0.0f);
			}

			if (Input::IsKeyActive(KeyCode::Q))
			{
				direction -= glm::vec3(0.0f, 1.0f, 0.0f);
			}

			if (glm::dot(direction, direction) > 0)
			{
				direction = glm::normalize(direction);
			}

			m_Transform->Translate(direction * dt * 50.0f);
		}

		void UpdateRotation(float dt)
		{
			m_FlyingCamera->m_Pitch -= Input::GetMouseDeltaPosY() * dt * 10.0f;
			m_FlyingCamera->m_Yaw -= Input::GetMouseDeltaPosX() * dt * 10.0f;

			m_FlyingCamera->m_Pitch = std::clamp(m_FlyingCamera->m_Pitch, -90.0f, 90.0f);

			m_Transform->SetLocalOrientation(m_FlyingCamera->m_Yaw, m_FlyingCamera->m_Pitch, 0.0f);
		}

		void Update(EventManager & eventManager, float dt) override
		{
			UpdatePosition(dt);
			UpdateRotation(dt);
		}
	};
}
