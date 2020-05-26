#pragma once
#include <string>

namespace sixengine {

	class Animation
	{
	public:
		std::string currentAnimationName = "idle";
		std::string previousAnimationName = "idle";

		float currentAnimationTimer = 0.0f;
		float previousAnimationTimer = 0.0f;
	};

}