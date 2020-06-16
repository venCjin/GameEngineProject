#pragma once

#include <ECS/SystemManager.h>

#include <Gameplay/Components/Transform.h>
#include <Gameplay/Components/NavAgent.h>
#include <Physics/Components/DynamicBody.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace sixengine {

	SYSTEM(NavAgentSystem, Transform, NavAgent, DynamicBody)
    {
		float speed = 0.25f;
		float maxSpeed = 4.0f;

		void Update(EventManager& eventManager, float dt) override
		{
			if (m_NavAgent->m_ProcedeMoving)
			{
				m_DynamicBody->m_Drag = 10.0f;
				glm::vec3 dir = m_NavAgent->m_LastPlayerKnownPosition - m_Transform->GetWorldPosition();
				glm::normalize(dir);
				
				if (glm::length(m_DynamicBody->m_Velocity) < maxSpeed)
				{
					m_DynamicBody->m_Velocity += dir * speed;
				}
			}
			else
			{
				m_DynamicBody->m_Velocity = glm::vec3(0.0f);
			}
			
		}
    };
}
