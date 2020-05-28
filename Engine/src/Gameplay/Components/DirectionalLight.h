#pragma once

#include "Core/ISerializable.h"
#include <glm/glm.hpp>

namespace sixengine {

	class GameObject;
	
	struct DirectionalLight : public ISerializable
	{
		glm::vec3 position;  //TODO: from Transform; //needed for shadow mapping 
		glm::vec3 direction; //TODO: from Transform
		glm::vec3 color;

		DirectionalLight(GameObject* go) {}

		virtual void Load(std::iostream& stream) override
		{
			stream >> position.x >> position.y >> position.z;    //TODO: from Transform
			stream >> direction.x >> direction.y >> direction.z; //TODO: from Transform
			stream >> color.x >> color.y >> color.z;
		}

		virtual void Save(std::iostream& stream) override
		{
			//TODO:
		}
	};

}