#pragma once
#include <glm/glm.hpp>

namespace sixengine {

	struct DirectionalLight
	{
		glm::vec4 position;	// needed for shadow mapping
		glm::vec4 direction;
		glm::vec4 color;
	};

	struct PointLight
	{
		glm::vec3 position;
		glm::vec3 color;
	};

	struct SpotLight
	{
		glm::vec3 position;
		glm::vec3 direction;
		glm::vec3 color;

		float cutOff;
		float outerCutOff;
	};

	struct LightData
	{
		float ao;
		float metallic;
		float roughness;
		float a = 0.0f;

		DirectionalLight dirLight;
	};
}
