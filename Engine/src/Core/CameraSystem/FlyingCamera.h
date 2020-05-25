#pragma once

#include "Camera.h"

#include <glm/glm.hpp>
#include "Gameplay/Components/Transform.h"

namespace sixengine
{
	class FlyingCamera
	{
	public:
		float m_Yaw = 0.0f;
		float m_Pitch = 0.0f;
	};
}