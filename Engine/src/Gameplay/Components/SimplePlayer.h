#pragma once

#include <glm/glm.hpp>

namespace sixengine {

	struct SimplePlayer : public ISerializable
	{
		SimplePlayer() {}
        SimplePlayer(GameObject* go) {}
        virtual void Load(std::iostream& stream) {}
		virtual void Save(std::iostream& stream) {}
	};

}