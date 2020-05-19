#pragma once
#include <glm/glm.hpp>

namespace sixengine {

	struct DirectionalLight
	{
		glm::vec3 position;	// needed for shadow mapping
		glm::vec3 direction;
		glm::vec3 color;
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
		//float ao;
		//float metallic;
		//float roughness;

		//glm::vec3 position;
		//glm::vec3 direction;
		//glm::vec3 color;
		DirectionalLight dirLight;
	};
}
