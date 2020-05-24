#pragma once

#include "Camera.h"

#include <glm/glm.hpp>
#include "Gameplay/Components/Transform.h"

namespace sixengine
{
	class FollowCamera
	{
	public:
		Transform* m_LookTarget;
		Transform* m_FollowTarget;
		glm::vec3 m_FollowOffset;
	};
}