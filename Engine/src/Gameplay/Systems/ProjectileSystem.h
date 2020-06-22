#pragma once

#include <ECS/SystemManager.h>

#include <Gameplay/Components/Projectile.h>
#include <Gameplay/Components/Transform.h>

namespace sixengine {

	SYSTEM(ProjectileSystem, Projectile)
	{

		void Update(EventManager & eventManager, float dt) override
		{
			if (entity.Component<Transform>()->GetWorldPosition().y <= 0.0001f) 
			{
				entity.Component<Transform>()->Translate(glm::vec3(0.0f, -100.0f, 0.0f));
			}
			else { entity.Component<Transform>()->Translate(m_Projectile->speedDir * dt); }
		}

	};
}