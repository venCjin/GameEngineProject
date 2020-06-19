#pragma once
#include <Gameplay\GameObject.h>


namespace sixengine
{
	class Questionmark
	{
	public:
		GameObject* enemy;
		glm::vec4 color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		float fill = 0.5f;
		bool visable = true;
	};
}