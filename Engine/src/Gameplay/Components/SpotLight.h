#pragma once

#include <glm/glm.hpp>

namespace sixengine {

	struct SpotLight
	{
		glm::vec3 position;
		glm::vec3 direction;
		glm::vec3 color;

		// always pass cut off angles as "glm::cos(glm::radians(angle)" ! angle == cutOff || angle == outerCutOff
		float cutOff;
		float outerCutOff;
	};

}