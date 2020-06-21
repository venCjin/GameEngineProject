#pragma once
#include <ECS/SystemManager.h>
#include <Gameplay/Components/LoopedSound.h>


namespace sixengine
{

	SYSTEM(LoopedSoundSystem, LoopedSound) 
	{
		void Update(EventManager & eventManager, float dt) override
		{
			if (m_LoopedSound->IsPaused()) m_LoopedSound->Play();
		}

	};
	SYSTEM(StaticLoopedSound3DSystem, LoopedSound3D)
	{

		GameObject* m_Player;

	public:

		StaticLoopedSound3DSystem(GameObject* player) : BaseStaticLoopedSound3DSystem()
		{
			m_Player = player;
		}

		void OnStart(EventManager & eventManager) override
		{
			
		}

		void Update(EventManager& eventManager, float dt) override
		{
			if (m_LoopedSound3D->isPaused())
			{
				//m_StaticLoopedSound->Play();
			}
				
			glm::vec3 playerPos = m_Player->GetComponent<Transform>()->GetWorldPosition();
			//LOG_CORE_WARN(glm::distance(playerPos, m_LoopedSound3D->GetPosition()));
			//LOG_CORE_WARN("{0} {1} {2}", playerPos.x, playerPos.y, playerPos.z);
			//LOG_CORE_WARN("{0} {1} {2}", m_StaticLoopedSound->GetPosition().x, m_StaticLoopedSound->GetPosition().y, m_StaticLoopedSound->GetPosition().z);
			
			if (glm::distance(playerPos, m_LoopedSound3D->GetPosition()) > m_LoopedSound3D->GetRad())
			{
				m_LoopedSound3D->Pause();
			}
			else
			{
				m_LoopedSound3D->Play();
			}
				

		}
	};
}
