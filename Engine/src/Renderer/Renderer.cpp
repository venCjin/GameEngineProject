#include "pch.h"
#include "Renderer.h"

#include <glad\glad.h>
#include <GLFW\glfw3.h>

#include "PrimitiveUtils.h"
#include "Buffers.h"
#include "Core/Profile.h"

namespace sixengine {

	Renderer::Renderer()
	{
	}

	Renderer::~Renderer()
	{
	}

	void Renderer::Render(const VertexArray* vertexArray, const Shader* shader, std::vector<glm::mat4> models)
	{

		struct DrawElementsCommand
		{
			unsigned int vertexCount;
			unsigned int instanceCount;
			unsigned int firstIndex;
			unsigned int baseVertex;
			unsigned int baseInstance;
		};

		DrawElementsCommand indirectData[] {
			{   36, 2,   0,   0, 0 },
			{  540, 1,  36,   8, 2 },
			{ 5520, 2, 576, 129, 3 }
		};

		/*std::vector<glm::mat4> models;
		glm::mat4 m1 = glm::translate(glm::mat4(1.0f), glm::vec3( 0.0f,  0.0f, 0.0f));
		models.push_back(m1);
		glm::mat4 m2 = glm::translate(glm::mat4(1.0f), glm::vec3( 1.25f, 0.0f, 0.0f));
		models.push_back(m2);
		glm::mat4 m3 = glm::translate(glm::mat4(1.0f), glm::vec3(-1.25f, 0.0f, 0.0f));
		models.push_back(m3);*/

		/*for (int i = 0; i < 100)
		{
		
		}*/
		
		// set attribute pointers for matrix (4 times vec4)
		/*glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

		glVertexAttribDivisor(1, 1);
		glVertexAttribDivisor(2, 1);
		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);

		vertexArray->Bind();

		unsigned int buffer;
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, models.size() * sizeof(glm::mat4), models.data(), GL_STATIC_DRAW);*/

		unsigned int ssbo;
		glGenBuffers(1, &ssbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, models.size() * sizeof(glm::mat4), models.data(), GL_DYNAMIC_COPY);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

		vertexArray->Bind();

		unsigned int idbo;
		glGenBuffers(1, &idbo);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, idbo);
		glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(indirectData), indirectData, GL_DYNAMIC_DRAW);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glEnable(GL_DEPTH_TEST);

		//shader->Bind();
		//glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, 0);
		glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (void*)0, 3, 0);
	}

	void Renderer::Clear(float r, float g, float b)
	{
		glClearColor(r, g, b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

}