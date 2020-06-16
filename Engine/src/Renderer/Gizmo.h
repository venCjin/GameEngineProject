#pragma once

#include "Renderer/VertexArray.h"
#include "Renderer/Shader.h"
#include "Core/ISerializable.h"
#include "Renderer/PrimitiveUtils.h"

#include "glm/glm.hpp"

namespace sixengine {

	class Gizmo : public ISerializable
	{
		VertexArray* m_VAO;
		Shader* m_Shader;
		glm::vec3 m_Color;
	public:
		glm::mat4 model;

		Gizmo(VertexArray* vao, Shader* shader, glm::vec3 color);
		~Gizmo();

		Gizmo(Shader* shader);
		virtual void Load(std::iostream& stream) override;
		virtual void Save(std::iostream& stream) override;

		void Draw(const glm::mat4& model);
	};

}

