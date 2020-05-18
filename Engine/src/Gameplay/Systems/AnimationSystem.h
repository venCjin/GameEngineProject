#pragma once

#include <ECS/SystemManager.h>

#include <Gameplay/Components/Animation.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Core/Input.h>

namespace sixengine {

	SYSTEM(AnimationSystem, Animation)
	{
		std::string previousAnim;
		void Update(EventManager & eventManager, float dt) override
		{
					   
			if (Input::IsKeyActive(KeyCode::DOWN) || Input::IsKeyActive(KeyCode::UP) ||
				Input::IsKeyActive(KeyCode::RIGHT) || Input::IsKeyActive(KeyCode::LEFT)|| Input::IsKeyActive(KeyCode::Q))
			{ 
				m_Animation.Get()->name = "walk";
			}
			else
			{
				m_Animation.Get()->name = "samba";

			}
			m_Animation.Get()->timer += dt;

			if (m_Animation.Get()->name != previousAnim)
			{
				m_Animation.Get()->timer = 0.0f;
				previousAnim = m_Animation.Get()->name;
			}

		}
	};
}
