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

		void Update(EventManager & eventManager, float dt) override
		{
					   
			if (Input::IsKeyActive(KeyCode::DOWN) || Input::IsKeyActive(KeyCode::UP) ||
				Input::IsKeyActive(KeyCode::RIGHT) || Input::IsKeyActive(KeyCode::LEFT))
			{ 
				ChangeAnimation("walk");
			}
			else if (Input::IsMouseButtonActive(0))
			{
				ChangeAnimation("punch");
			}
			else
			{
				ChangeAnimation("idle");
			}

			m_Animation.Get()->currentAnimationTimer += dt;
			m_Animation.Get()->previousAnimationTimer += dt;

		}

		void ChangeAnimation(std::string newAnimationName)
		{
			if (m_Animation.Get()->currentAnimationName == newAnimationName)
				return;

			m_Animation.Get()->previousAnimationName = m_Animation.Get()->currentAnimationName;
			m_Animation.Get()->currentAnimationName = newAnimationName;
			m_Animation.Get()->previousAnimationTimer = m_Animation.Get()->currentAnimationTimer;
			m_Animation.Get()->currentAnimationTimer = 0.0f;


		}
	};
}
