#include "pch.h"
#include "Gizmo.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Core/Timer.h>
#include <Core/Application.h>
#include <Core/Scene.h>

namespace sixengine {

	Gizmo::Gizmo(VertexArray* vao, Shader* shader, glm::vec3 color, bool enableDraw, float timeToLive)
		: m_VAO(vao), m_Shader(shader), m_Color(color), draw(enableDraw), endTime(Timer::Instance()->GetTime(SECOND) + timeToLive)
	{
	}

	Gizmo::~Gizmo()
	{
		m_VAO->~VertexArray();
	}

	Gizmo::Gizmo(Shader* shader) : m_Shader(shader), draw(true), endTime(Timer::Instance()->GetTime(SECOND) + 1000000000.0f)
	{
	}

	void Gizmo::Load(std::iostream& stream)
	{
		std::string s;
		float r, g, b;
		stream >> r >> g >> b;

		std::vector<GizmoVertex> vertices;
		std::vector<unsigned int> indices;
		stream >> s;
		if (s == "Cube") // Directional Light
		{
			float x, y, z;
			stream >> x >> y >> z;
			PrimitiveUtils::GenerateBox(vertices, indices, x, y, z);
		}
		else if (s == "Sphere") // Point Light
		{
			float radius;
			stream >> radius;
			PrimitiveUtils::GenerateSphere(vertices, indices, radius);
		}
		else if (s == "Cone") // Spot Light
		{
			float radius, height;
			stream >> radius >> height;
			PrimitiveUtils::GenerateCone(vertices, indices, radius, height);
		}
		else if (s == "Capsule")
		{
			PrimitiveUtils::GenerateCapsule(vertices, indices);
		}
		else if (s == "Quad")
		{
			PrimitiveUtils::GenerateQuad(vertices, indices);
		}
		else if (s == "Line")
		{
			float x, y, z;
			stream >> x >> y >> z;
			vertices.emplace_back(GizmoVertex{ glm::vec3{ x, y, z } });
			vertices.emplace_back(GizmoVertex{ glm::vec3{ x, y, z } });
			stream >> x >> y >> z;
			vertices.emplace_back(GizmoVertex{ glm::vec3{ x, y, z } });
			indices.push_back(0);
			indices.push_back(1);
			indices.push_back(2);
		}
		m_VAO = new VertexArray();
		VertexBuffer* vbo = new VertexBuffer(&vertices[0], vertices.size());
		vbo->SetLayout({
			{ VertexDataType::VEC3F, "Position" }
		});
		IndexBuffer* ibo = new IndexBuffer(&indices[0], indices.size());
		m_VAO->AddVertexBuffer(*vbo);
		m_VAO->AddIndexBuffer(*ibo);
		m_Color = { r, g, b };
	}

	void Gizmo::Save(std::iostream& stream)
	{
	}

	void Gizmo::DrawLine(glm::vec3 start, glm::vec3 end, float timeToLive, glm::vec3 color)
	{
		std::vector<GizmoVertex> vertices;
		std::vector<unsigned int> indices;
		vertices.emplace_back(GizmoVertex{ glm::vec3{ start.x, start.y, start.z } });
		vertices.emplace_back(GizmoVertex{ glm::vec3{ start.x, start.y, start.z } });
		vertices.emplace_back(GizmoVertex{ glm::vec3{ end.x, end.y, end.z } });
		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(2);
		VertexArray* vao = new VertexArray();
		VertexBuffer* vbo = new VertexBuffer(&vertices[0], vertices.size());
		vbo->SetLayout({
			{ VertexDataType::VEC3F, "Position" }
			});
		IndexBuffer* ibo = new IndexBuffer(&indices[0], indices.size());
		vao->AddVertexBuffer(*vbo);
		vao->AddIndexBuffer(*ibo);
		Application::Get().GetScene()->m_SceneRoot->AddGizmo(new Gizmo(vao, Application::Get().GetScene()->m_ShaderManager->Get("Gizmo"), color, true, timeToLive));
	}

	void Gizmo::Draw(const glm::mat4& model)
	{
		if (draw)
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

}