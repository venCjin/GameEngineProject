#pragma once
#pragma once

#include <glm/glm.hpp>

namespace sixengine {

	struct Collider
	{
	protected:
		bool m_isStatic = false;
		
	public:
		Collider()
		{
			m_isStatic = true;
		}

		Collider(bool isStatic)
		{
			m_isStatic = isStatic;
		}

		virtual ~Collider() {}
	};
}
