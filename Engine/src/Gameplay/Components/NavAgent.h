#pragma once

#include <glm/glm.hpp>

namespace sixengine {

	struct NavAgent
	{
		glm::vec3 m_LastPlayerKnownPosition = glm::vec3(0.0f);
		bool m_ProcedeMoving = false;

		NavAgent()
		{}

		void SetDestination(glm::vec3 playerPosition)
		{
			m_LastPlayerKnownPosition = playerPosition;
		}

		void Go()
		{
			m_ProcedeMoving = true; 
		}

		void Stop()
		{
			m_ProcedeMoving = false;
		}
	};

}