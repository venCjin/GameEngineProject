#pragma once

#include <Core/ISerializable.h>
#include "Collider.h"

#include <glm/glm.hpp>

namespace sixengine {

	struct BoxCollider : Collider, ISerializable
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

		BoxCollider(GameObject* go) {}
		virtual void Load(std::iostream& stream)
		{
			float x, y, z;
			bool isStatic;
			stream >> x >> y >> z >> isStatic;
			m_Size = { x, y, z};
			m_isStatic = isStatic;
		}
		virtual void Save(std::iostream& stream)
		{

		}

		virtual ~BoxCollider()
		{

		}
	};
}
