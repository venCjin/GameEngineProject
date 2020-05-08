#include "pch.h"
#include "Renderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "PrimitiveUtils.h"
#include "Buffers.h"

namespace sixengine {

	Renderer::Renderer()
	{
	}

	Renderer::~Renderer()
	{
	}

	void Renderer::Render(const VertexArray* vertexArray, const Shader* shader, unsigned int texture)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		shader->Bind();
		vertexArray->Bind();
		//LOG_CORE_INFO(texture);
		shader->SetInt("texture1", texture);

		glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, 0);
	}

	void Renderer::Clear(float r, float g, float b)
	{
		glClearColor(r, g, b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

}