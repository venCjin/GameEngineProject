#pragma once

#include <ECS/SystemManager.h>

#include <Gameplay/Components/Transform.h>
#include <Gameplay/Components/TestRotation.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace sixengine {

	SYSTEM(SimplePlayerSystem, Transform, SimplePlayer)
	{
		void Update(EventManager & eventManager, float dt) override
		{
			glm::vec3 dir = glm::vec3();

			if (Input::IsKeyActive(KeyCode::S)) { dir.z += 1.0f; }
			if (Input::IsKeyActive(KeyCode::W)) { dir.z -= 1.0f; }
			if (Input::IsKeyActive(KeyCode::D)) { dir.x += 1.0f; }
			if (Input::IsKeyActive(KeyCode::A)) { dir.x -= 1.0f; }
			if (Input::IsKeyActive(KeyCode::PAGE_UP)) { dir.y += 1.0f; }
			if (Input::IsKeyActive(KeyCode::PAGE_DOWN)) { dir.y -= 1.0f; }

			glm::vec3 forward = m_SimplePlayer->camera->m_Front;
			forward.y = 0.0f;
			forward = -glm::normalize(forward);

			glm::vec3 right = m_SimplePlayer->camera->m_Right;
			right.y = 0.0f;
			right = glm::normalize(right);

			glm::vec3 final = dir.z * forward + dir.x * right;

			if (glm::length(final) > 0.0f)
			{
				final = glm::normalize(final);
			}

			auto w = m_Transform->GetWorld();
			w = glm::translate(w, (final) * dt * 4.0f);
			m_Transform->SetWorld(w);
		}
	};
}
