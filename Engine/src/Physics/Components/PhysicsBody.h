#pragma once

#include <glm/glm.hpp>

namespace sixengine {

	struct PhysicsBody
	{
	public:
		glm::vec3 velocity = glm::vec3(0.0f);
		float jumpForce = 50.0f;
	};
}
