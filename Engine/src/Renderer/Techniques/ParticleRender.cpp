#include "pch.h"
#include "ParticleRender.h"

namespace sixengine {

	ParticleRender::ParticleRender(Shader* shader)
		: Technique(shader), m_Models(502 * sizeof(glm::mat4), 0, 1)
	{
		m_Shader->Bind();

		m_Shader->SetInt("textureName", 2);
	}

	void ParticleRender::Start(TextureArray* textureArray)
	{
        float vertices[] = { 
			 0.5f,  0.5f, 0.0f,   1.0f, 1.0f,   // top right
			 0.5f, -0.5f, 0.0f,   1.0f, 0.0f,   // bottom right
			-0.5f, -0.5f, 0.0f,   0.0f, 0.0f,   // bottom left
			-0.5f,  0.5f, 0.0f,   0.0f, 1.0f    // top left 
        };

        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);

        glBindVertexArray(m_VAO);

        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
	}

	void ParticleRender::StartFrame(std::vector<RendererCommand*>& commandList, std::vector<DrawElementsCommand> drawCommands, std::vector<glm::mat4>& models, std::vector<glm::vec4> layers)
	{
	}

	void ParticleRender::Render(std::vector<RendererCommand*>& commandList)
	{
	}

	void ParticleRender::Render(std::vector<ParticleEmitter*> particleEmitters)
	{
		std::vector<glm::mat4> models;
		models.push_back(Camera::ActiveCamera->GetProjectionMatrix());
		models.push_back(Camera::ActiveCamera->GetViewMatrix());

		m_Models.Update(models.data(), 2 * sizeof(glm::mat4));

		m_Shader->Bind();
		glBindVertexArray(m_VAO);

		for (auto p : particleEmitters)
		{
			models = p->GetAllTransforms(); 
			m_Models.Update(models.data(), MAX_PARTICLE_COUNT * sizeof(glm::mat4), 2 * sizeof(glm::mat4));
			m_Models.Bind();

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, p->m_Texture->m_ID);

			glDrawArraysInstanced(GL_TRIANGLES, 0, 4, 500);
		}

		glBindVertexArray(0);
	}

}
