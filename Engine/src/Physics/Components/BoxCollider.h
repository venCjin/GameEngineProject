#pragma once

#include "Collider.h"

#include <glm/glm.hpp>

namespace sixengine {

	struct BoxCollider : Collider
	{
	public:
		glm::vec3 m_Size;

	public:
		BoxCollider(glm::vec3 size) : Collider()
		{
			m_Size = size;
		}

		BoxCollider(glm::vec3 size, bool isStatic) : Collider(isStatic)
		{
			m_Size = size;
		}

		virtual ~BoxCollider()
		{

		}
	};
}
