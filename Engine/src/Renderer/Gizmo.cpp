#include "pch.h"
#include "Gizmo.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace sixengine {

	Gizmo::Gizmo(VertexArray* vao, Shader* shader, glm::vec3 color)
		: m_VAO(vao), m_Shader(shader), m_Color(color)
	{
	}

	Gizmo::~Gizmo()
	{
	}

	void Gizmo::Draw(const glm::mat4& model)
	{
		//m_Shader->Bind();
		m_VAO->Bind();
		m_Shader->SetMat4("model", model);
		m_Shader->SetVec3("color", m_Color);

		glDrawElements(GL_TRIANGLES, m_VAO->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, 0);

		//m_VAO->UnBind();
		//m_Shader->Unbind();
	}

}