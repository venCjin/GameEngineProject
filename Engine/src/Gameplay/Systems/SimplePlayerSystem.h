#pragma once

#include <ECS/SystemManager.h>

#include <Gameplay/Components/Transform.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
			if (Input::IsKeyActive(KeyCode::PAGE_UP)) { dir.y += 1.0f; }
			if (Input::IsKeyActive(KeyCode::PAGE_DOWN)) { dir.y -= 1.0f; }

			auto pos = m_Transform->getWorldPosition();
			pos += dir * dt * 25.0f;
			m_Transform->SetWorldPosition(pos);
		}
	};
}
