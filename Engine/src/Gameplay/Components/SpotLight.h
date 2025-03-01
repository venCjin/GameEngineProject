#pragma once

#include "Core/ISerializable.h"
#include <glm/glm.hpp>

namespace sixengine {

	class GameObject;

	struct SpotLight : public ISerializable
	{
		glm::vec3 position;  //TODO: from Transform
		glm::vec3 direction; //TODO: from Transform
		glm::vec3 color;

		// always pass cut off angles as "glm::cos(glm::radians(angle)" ! angle == cutOff || angle == outerCutOff
		float cutOff;
		float outerCutOff;

		SpotLight(GameObject* go) {}

		virtual void Load(std::iostream& stream) override
		{
			stream >> position.x >> position.y >> position.z;    //TODO: from Transform
			stream >> direction.x >> direction.y >> direction.z; //TODO: from Transform
			stream >> color.x >> color.y >> color.z;
			stream >> cutOff >> outerCutOff;
		}

		virtual void Save(std::iostream& stream) override
		{
			//TODO:
		}
	};

}