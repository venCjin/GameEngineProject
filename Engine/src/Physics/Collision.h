#pragma once

#include <Physics\Components\Collider.h>

namespace sixengine {

	struct Collision
	{
	public: 
		Collision() {}

		Entity self;
		Entity other;

		glm::vec3 offset;
	};
}