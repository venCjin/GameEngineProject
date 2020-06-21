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
			float currentTimeInTicks = m_Animation->GetCurrentAnimation()->timer * m_Animation->GetCurrentAnimation()->ticksPerSecond;
			float previousTimeInTicks = m_Animation->GetPreviousAnimation()->timer * m_Animation->GetPreviousAnimation()->ticksPerSecond;
			
			if (m_Animation->m_CurrentAnimationName != "idle" && currentTimeInTicks < m_Animation->GetCurrentAnimation()->duration)
				m_Animation->GetCurrentAnimation()->timer += dt;
			else
				m_Animation->ChangeAnimation("idle");

			if (!m_Animation->GetPreviousAnimation()->singleCycle || (m_Animation->GetPreviousAnimation()->singleCycle && previousTimeInTicks < m_Animation->GetPreviousAnimation()->duration))
				m_Animation->GetPreviousAnimation()->timer += dt;
 
			/*m_Animation->currentAnimationTimer += dt;
			m_Animation->previousAnimationTimer += dt;*/
					   
			/*if (Input::IsKeyActive(KeyCode::DOWN) || Input::IsKeyActive(KeyCode::UP) ||
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
			}*/

			

		}

		/*void ChangeAnimation(std::string newAnimationName)
		{
			if (m_Animation.Get()->currentAnimationName == newAnimationName)
				return;

			m_Animation.Get()->previousAnimationName = m_Animation.Get()->currentAnimationName;
			m_Animation.Get()->currentAnimationName = newAnimationName;
			m_Animation.Get()->previousAnimationTimer = m_Animation.Get()->currentAnimationTimer;
			m_Animation.Get()->currentAnimationTimer = 0.0f;


		}*/
	};
}
