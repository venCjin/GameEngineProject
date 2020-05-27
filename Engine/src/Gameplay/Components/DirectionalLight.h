#pragma once

#include <glm/glm.hpp>

namespace sixengine {

	struct DirectionalLight
	{
		glm::vec3 position;	// needed for shadow mapping
		glm::vec3 direction;
		glm::vec3 color;
	};

}