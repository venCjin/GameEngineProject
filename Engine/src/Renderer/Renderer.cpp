#include "pch.h"
#include "Renderer.h"

#include <glad\glad.h>
#include <GLFW\glfw3.h>

#include "PrimitiveUtils.h"
#include "Buffers.h"

namespace sixengine {

	Renderer::Renderer()
	{
	}

	Renderer::~Renderer()
	{
	}

	void Renderer::Render(const VertexArray* vertexArray, const Shader* shader)
	{
		/*std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;

		PrimitiveUtils::GenerateCube(vertices, indices);
		
		VertexBuffer* vb1 = new VertexBuffer(vertices.data(), vertices.size() * sizeof(Vertex));

		//IndexBuffer* ib1 = new IndexBuffer(indices.data(), indices.size());
		//ib1->UnBind();

		VertexArray vao1;
		vao1.Bind();
		//vb1.Bind();
		//ib1.Bind();
		
		unsigned int VBO1, EBO1;
		glCreateBuffers(1, &VBO1);
		glCreateBuffers(1, &EBO1);

		glBindBuffer(GL_ARRAY_BUFFER, VBO1);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO1);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		std::vector<Vertex> vertices1;
		std::vector<unsigned int> indices1;
		PrimitiveUtils::GenerateSphere(vertices1, indices1, 30, 30);

		VertexBuffer* vb2 = new VertexBuffer(vertices1.data(), vertices1.size() * sizeof(Vertex));

		IndexBuffer* ib2 = new IndexBuffer(indices1.data(), indices1.size());

		VertexArray vao2;
		vao2.Bind();
		//vb2.Bind();
		//ib2.Bind();

		unsigned int VBO2, EBO2;
		glCreateBuffers(1, &VBO2);
		glCreateBuffers(1, &EBO2);

		glBindBuffer(GL_ARRAY_BUFFER, VBO2);
		glBufferData(GL_ARRAY_BUFFER, vertices1.size() * sizeof(Vertex), vertices1.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices1.size() * sizeof(unsigned int), indices1.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);*/

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		//shader->Bind();
		vertexArray->Bind();

		glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, 0);

		/*vao1.Bind();
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

		vao2.Bind();
		glDrawElements(GL_TRIANGLES, indices1.size(), GL_UNSIGNED_INT, 0);*/
	}

	void Renderer::Clear(float r, float g, float b)
	{
		glClearColor(r, g, b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}

}