#include "pch.h"
#include "Renderer.h"

#include <glad\glad.h>
#include <GLFW\glfw3.h>

namespace sixengine {

	Renderer::Renderer()
	{
	}

	Renderer::~Renderer()
	{
	}

	void Renderer::Render(const VertexArray* vertexArray, const Shader* shader)
	{
		shader->Bind();
		vertexArray->Bind();
		//glBindVertexArray(VAO);

		glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, 0);
	}

	void Renderer::Clear(float r, float g, float b)
	{
		glClearColor(r, g, b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}

}