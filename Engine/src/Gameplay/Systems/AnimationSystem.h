#pragma once

#include <ECS/SystemManager.h>

#include <Gameplay/Components/Animation.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace sixengine {

	SYSTEM(SystemAnimation, Animation)
	{
		void Update(EventManager & eventManager, float dt) override
		{
			// does not work

			/*if (Input::IsKeyActive(KeyCode::DOWN)) {  }
			if (Input::IsKeyActive(KeyCode::UP)) {  }
			if (Input::IsKeyActive(KeyCode::RIGHT)) {  }
			if (Input::IsKeyActive(KeyCode::LEFT)) {  }
			if (Input::IsKeyActive(KeyCode::PAGE_UP)) {  }
			if (Input::IsKeyActive(KeyCode::PAGE_DOWN)) {  }*/

			m_Animation.Get()->name = "AAA";

		}
	};
}
