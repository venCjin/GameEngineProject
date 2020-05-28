#pragma once

#include <ECS/SystemManager.h>

#include <Gameplay/Components/Transform.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Core/CameraSystem/Camera.h"

namespace sixengine {

	SYSTEM(SimplePlayerSystem, Transform, SimplePlayer)
	{
		void Update(EventManager & eventManager, float dt) override
		{
			glm::vec3 dir = glm::vec3();

			if (Input::IsKeyActive(KeyCode::DOWN)) { dir.z += 1.0f; }
			if (Input::IsKeyActive(KeyCode::UP)) { dir.z -= 1.0f; }
			if (Input::IsKeyActive(KeyCode::RIGHT)) { dir.x += 1.0f; }
			if (Input::IsKeyActive(KeyCode::LEFT)) { dir.x -= 1.0f; }
			//if (Input::IsKeyActive(KeyCode::PAGE_UP)) { dir.y += 1.0f; }
			//if (Input::IsKeyActive(KeyCode::PAGE_DOWN)) { dir.y -= 1.0f; }

			auto originalPos = m_Transform->GetWorldPosition();

			glm::vec3 finalDir = m_Transform->getWorldPosition();
			finalDir -= dir.z * Camera::ActiveCamera->m_Transform->GetForward() * dt * 5.0f;
			finalDir += dir.x * Camera::ActiveCamera->m_Transform->GetRight() * dt * 5.0f;
			finalDir.y = m_Transform->getWorldPosition().y;

			m_Transform->SetWorldPosition(finalDir);

			if (glm::dot(finalDir - originalPos, finalDir - originalPos) > 0)
			{
				m_Transform->LookAt(m_Transform->GetWorldPosition() + (originalPos - finalDir));
			}
		}
	};
}
