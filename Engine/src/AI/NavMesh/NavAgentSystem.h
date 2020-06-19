#pragma once

#include <ECS/SystemManager.h>

#include <AI/NavMesh/NavAgent.h>
#include <Gameplay/Components/Transform.h>
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
			m_DynamicBody->m_Drag = 3.0f;

			if (m_NavAgent->m_ProcedeMoving)
			{
				auto currentPos = m_Transform->GetWorldPosition();
				auto targetPos = m_NavAgent->m_Destination;

				auto direction = glm::normalize(targetPos - currentPos);
				direction.y = 0.0f;

				m_DynamicBody->m_Velocity = direction * maxSpeed;
			}
			else
			{

			}
		}
    };
}
