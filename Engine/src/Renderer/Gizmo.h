#pragma once

#include "Renderer/VertexArray.h"
#include "Renderer/Shader.h"

#include "glm/glm.hpp"

namespace sixengine {

	class Gizmo
	{
		VertexArray* m_VAO;
		Shader* m_Shader;
		glm::vec3 m_Color;
	public:
		Gizmo(VertexArray* vao, Shader* shader, glm::vec3 color);
		~Gizmo();

		void Draw(const glm::mat4& model);
	};

}

