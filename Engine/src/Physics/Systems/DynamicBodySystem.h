#pragma once

#include "pch.h"

#include <ECS/SystemManager.h>

#include <Physics/Components/DynamicBody.h>
#include <Gameplay/Components/Transform.h>

namespace sixengine {

	SYSTEM(DynamicBodySystem, DynamicBody, Transform)
	{
	public:
		
		void Update(EventManager & eventManager, float dt) override
		{
			m_Transform->Translate(m_DynamicBody->m_Velocity * dt);
			m_DynamicBody->m_Velocity = m_DynamicBody->m_Velocity * (1.0f - dt * m_DynamicBody->m_Drag);
		}
	};
}
