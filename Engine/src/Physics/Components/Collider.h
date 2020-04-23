#pragma once
#pragma once

#include <glm/glm.hpp>

namespace sixengine {

	struct Collider
	{
	private:
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

		bool isStatic()
		{
			return m_isStatic;
		}
	};
}
