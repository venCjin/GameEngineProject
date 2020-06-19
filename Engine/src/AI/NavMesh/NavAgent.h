#pragma once

#include <glm/glm.hpp>

namespace sixengine {

	struct NavAgent
	{
		GameObject* m_GameObject;
		glm::vec3 m_Destination = glm::vec3(0.0f);
		bool m_ProcedeMoving = false;

		NavAgent(GameObject* go)
		{
			m_GameObject = go;
		}

		void SetDestination(glm::vec3 playerPosition)
		{
			m_Destination = playerPosition;
		}

		void Go()
		{
			m_ProcedeMoving = true; 
		}

		void Stop()
		{
			m_ProcedeMoving = false;
		}

		float GetRemainingDistance()
		{
			glm::vec3 my = m_GameObject->GetComponent<Transform>()->GetWorldPosition();
			glm::vec3 offset = (m_Destination - my);
			offset.y = 0.0f;

			return glm::length(offset);
		}
	};

}