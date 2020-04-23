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

			if (Input::IsKeyActive(KeyCode::DOWN)) { dir.z += 1.0f; }
			if (Input::IsKeyActive(KeyCode::UP)) { dir.z -= 1.0f; }
			if (Input::IsKeyActive(KeyCode::RIGHT)) { dir.x += 1.0f; }
			if (Input::IsKeyActive(KeyCode::LEFT)) { dir.x -= 1.0f; }
			if (Input::IsKeyActive(KeyCode::PAGE_UP)) { dir.y += 1.0f; }
			if (Input::IsKeyActive(KeyCode::PAGE_DOWN)) { dir.y -= 1.0f; }

			auto w = m_Transform->GetWorld();
			w = glm::translate(w, dir * dt * 4.0f);
			m_Transform->SetWorld(w);
		}
	};
}
