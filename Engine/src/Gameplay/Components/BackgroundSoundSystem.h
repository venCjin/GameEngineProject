#pragma once
#include <ECS/SystemManager.h>
#include <Core/AudioManager.h>
#include <Gameplay/Components/BackgroundSound.h>

namespace sixengine
{
	SYSTEM(BackgroundSoundSystem, BackgroundSound)
	{
		void Update(EventManager & eventManager, float dt) override
		{

			
			if (m_BackgroundSound->m_Sound == nullptr)
			{

				
				if (m_BackgroundSound->timer > m_BackgroundSound->interval)
				{
					m_BackgroundSound->m_Sound = INIT_TRACK(m_BackgroundSound->m_Key);
					m_BackgroundSound->m_Sound->setIsPaused(false);
					m_BackgroundSound->timer = 0.0f;
					float percent = ((rand() % 100) / 100.0f);
					m_BackgroundSound->interval = m_BackgroundSound->minInterval + percent * m_BackgroundSound->maxInterval;
				}
				m_BackgroundSound->timer += dt;
			}
			else
			{
				m_BackgroundSound->timer += 0.0f;
			}

			if (m_BackgroundSound->m_Sound != nullptr && m_BackgroundSound->m_Sound->isFinished())
			{
				m_BackgroundSound->m_Sound->drop();
				m_BackgroundSound->m_Sound = nullptr;
			}
		}
	};
}