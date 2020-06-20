#pragma once

#include <ECS/SystemManager.h>

#include <Gameplay/Components/Gate.h>
#include <Gameplay/Components/Generator.h>
#include <Gameplay/Components/Transform.h>

namespace sixengine {

	SYSTEM(GateSystem, Gate)
	{

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
					entity.Component<Transform>()->Translate(m_Gate->openingSpeed * dt);
					m_Gate->openingDuration -= dt;
				}
			}
		}

	};
}