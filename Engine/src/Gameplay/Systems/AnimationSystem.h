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
			LOG_ERROR(m_Animation->m_CurrentAnimationName);
			float currentTimeInTicks = m_Animation->GetCurrentAnimation()->timer * m_Animation->GetCurrentAnimation()->ticksPerSecond;
			float previousTimeInTicks = m_Animation->GetPreviousAnimation()->timer * m_Animation->GetPreviousAnimation()->ticksPerSecond;
			
			if (m_Animation->m_CurrentAnimationName != "idle" && currentTimeInTicks < m_Animation->GetCurrentAnimation()->duration)
				m_Animation->GetCurrentAnimation()->timer += dt;
			else
				m_Animation->ChangeAnimation("idle");

			if (!m_Animation->GetPreviousAnimation()->singleCycle || (m_Animation->GetPreviousAnimation()->singleCycle && previousTimeInTicks < m_Animation->GetPreviousAnimation()->duration))
				m_Animation->GetPreviousAnimation()->timer += dt;		
		}
		
	};
}
