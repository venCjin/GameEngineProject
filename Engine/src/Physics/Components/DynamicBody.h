#pragma once

#include <glm/glm.hpp>

namespace sixengine {

	class DynamicBody
	{
	public:
		glm::vec3 m_Velocity = glm::vec3(0.0f);
		float m_Drag = 1.0f;
	};
}
