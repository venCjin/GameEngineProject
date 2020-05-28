#pragma once

#include "Collider.h"

#include <glm/glm.hpp>

namespace sixengine {

	struct SphereCollider : Collider
	{
	public:
		float m_radius;

	public:
		SphereCollider(float radius) : Collider()
		{
			m_radius = radius;
		}

		SphereCollider(float radius, bool isStatic) : Collider(isStatic)
		{
			m_radius = radius;
		}

		SphereCollider(GameObject* go) {}
		virtual void Load(std::iostream& stream)
		{
			float radius;
			bool isStatic;
			stream >> radius >> isStatic;
			m_radius = radius;
			m_isStatic = isStatic;
		}
		virtual void Save(std::iostream& stream)
		{

		}

		virtual ~SphereCollider()
		{

		}
	};
}
