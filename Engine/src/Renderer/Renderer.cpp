#include "pch.h"
#include "Renderer.h"

#include <glad\glad.h>
#include <GLFW\glfw3.h>

namespace sixengine {

	Renderer::Renderer()
	{
		m_VAO = new VertexArray();
		m_VAO->Create();
	}

	Renderer::~Renderer()
	{
	}

	void Renderer::Init()
	{
		VertexBuffer* vbo;
		IndexBuffer* ibo;
		std::array<Vertex, 4> verticies;
		std::array<unsigned int, 6> indicies;

		verticies[0] = { 0.5f,  0.5f, 0.0f };  // top right
		verticies[1] = { 0.5f, -0.5f, 0.0f };  // bottom right
		verticies[2] = { -0.5f, -0.5f, 0.0f };  // bottom left
		verticies[3] = { -0.5f,  0.5f, 0.0f };  // top left 

		indicies[0] = 0;
		indicies[1] = 1;
		indicies[2] = 3;

		indicies[3] = 1;
		indicies[4] = 2;
		indicies[5] = 3;

		
		m_VAO->Bind();

		vbo = new VertexBuffer(verticies.data(), verticies.size());
		BufferData* vbd = new BufferData(sixengine::VertexDataType::VEC3F, "position");
		BufferLayout* vbl = new BufferLayout({ *vbd });
		vbo->SetLayout(*vbl);

		ibo = new IndexBuffer(indicies.data(), indicies.size());

		m_VAO->AddVertexBuffer(*vbo);
		m_VAO->AddIndexBuffer(*ibo);
	}

	void Renderer::Render()
	{
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//m_Shader.Use();

		m_VAO->Bind();
		glDrawElements(GL_TRIANGLES, m_VAO->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, 0);
	}

}