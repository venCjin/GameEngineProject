#pragma once

#include <ECS/SystemManager.h>

#include <Gameplay/Components/Projectile.h>
#include <Gameplay/Components/Transform.h>

namespace sixengine {

	SYSTEM(ProjectileSystem, Projectile)
	{

		void Update(EventManager & eventManager, float dt) override
		{
			if (m_Projectile->endTime > Timer::Instance()->GetTime(SECOND))
			{
				if (entity.Component<Transform>()->GetWorldPosition().y <= 0.0001f) {}
				else { entity.Component<Transform>()->Translate(m_Projectile->speedDir * dt); }
			}
			else
			{
				// Destroy
			}
		}

	};
}