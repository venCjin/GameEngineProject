#pragma once

#include "Core/ISerializable.h"
#include <glm/glm.hpp>

namespace sixengine {

	class GameObject;

	struct PointLight : public ISerializable
	{
		glm::vec3 position; //TODO: from Transform
		glm::vec3 color;    //TODO: from Transform

		PointLight(GameObject* go) {}

		virtual void Load(std::iostream& stream) override
		{
			stream >> position.x >> position.y >> position.z;
			stream >> color.x >> color.y >> color.z;
		}

		virtual void Save(std::iostream& stream) override
		{
			//TODO:
		}
	};

}