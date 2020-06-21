#pragma once
#include <ECS/SystemManager.h>
#include <Gameplay/Components/LoopedSound.h>


namespace sixengine
{
	SYSTEM(StaticLoopedSoundSystem, LoopedSound)
	{

		GameObject* m_Player;

	public:

		StaticLoopedSoundSystem(GameObject* player) : BaseStaticLoopedSoundSystem()
		{
			m_Player = player;
		}

		void OnStart(EventManager & eventManager) override
		{
			
		}

		void Update(EventManager& eventManager, float dt) override
		{
			if (m_LoopedSound->isPaused())
			{
				//m_StaticLoopedSound->Play();
			}
				
			glm::vec3 playerPos = m_Player->GetComponent<Transform>()->GetWorldPosition();
			LOG_CORE_WARN(glm::distance(playerPos, m_LoopedSound->GetPosition()));
			//LOG_CORE_WARN("{0} {1} {2}", playerPos.x, playerPos.y, playerPos.z);
			//LOG_CORE_WARN("{0} {1} {2}", m_StaticLoopedSound->GetPosition().x, m_StaticLoopedSound->GetPosition().y, m_StaticLoopedSound->GetPosition().z);
			
			if (glm::distance(playerPos, m_LoopedSound->GetPosition()) > m_LoopedSound->GetRad())
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
