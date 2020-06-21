#pragma once
#include <ECS/SystemManager.h>
#include <Gameplay/Components/LoopedSound.h>
#include <Physics/Components/DynamicBody.h>

namespace sixengine
{
	SYSTEM(DynamicSoundSystem, DynamicBody, LoopedSound, Transform)
	{
		float speedFactor = .25f;
		void Update(EventManager & eventManager, float dt) override
		{
			m_LoopedSound->SetPosition(m_Transform->getWorldPosition());

			m_LoopedSound->SetSpeed(glm::length(m_DynamicBody->m_Velocity) * speedFactor);
			if (glm::length(m_DynamicBody->m_Velocity) < .1f)
			{
				m_LoopedSound->Pause();
			}
			else
			{
				m_LoopedSound->Play();
			}
		}
	};
}