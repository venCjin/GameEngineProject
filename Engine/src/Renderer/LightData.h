#pragma once
#include <glm/glm.hpp>

namespace sixengine {

	struct DirectionalLightData
	{
		glm::vec4 position;	// needed for shadow mapping
		glm::vec4 direction;
		glm::vec4 color;
	};

	struct PointLightData
	{
		glm::vec3 position;
		glm::vec3 color;
	};

	struct SpotLightData
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

		DirectionalLightData dirLight;
	};
}
