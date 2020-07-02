#pragma once

#include <ECS/SystemManager.h>

#include <Gameplay/Components/Gate.h>
#include <Gameplay/Components/Generator.h>
#include <Gameplay/Components/Transform.h>

namespace sixengine {

	SYSTEM(GateSystem, Gate, Transform)
	{
		irrklang::ISound* openingSound = nullptr;
		void Update(EventManager & eventManager, float dt) override
		{
			
			if (m_Gate->closed)
			{
				bool open = true;
				for (auto i : m_Gate->generators)
					open &= i->destroyed;

				m_Gate->closed = !open;
			}
			else
			{
				

				if (m_Gate->openingDuration > 0.0f)
				{
					/*if (openingSound == nullptr) 
					{
						openingSound = INIT_TRACK_3D("gate", m_Transform->GetWorldPosition());
						openingSound->setIsPaused(false);
					}*/
					entity.Component<Transform>()->Translate(m_Gate->openingSpeed * dt);
					m_Gate->openingDuration -= dt;
				}
				else
				{
					//openingSound->stop();
					//openingSound->drop();
					openingSound = nullptr;
				}
			}
		}

	};
}