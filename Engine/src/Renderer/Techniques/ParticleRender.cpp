#include "pch.h"
#include "ParticleRender.h"

namespace sixengine {

	ParticleRender::ParticleRender(Shader* shader)
		: Technique(shader), m_Models(3 * sizeof(glm::mat4), 0, 12), 
		m_ParticleData(500 * sizeof(ParticleData), 1, 12)
	{
		m_Shader->Bind();

		m_Shader->SetInt("textureName", 2);
	}

	void ParticleRender::Start(TextureArray* textureArray)
	{
        float vertices[] = { 
			// Left bottom triangle
		-0.25f,  0.25f, 0.0f,   0.0f, 1.0f,
		-0.25f, -0.25f, 0.0f,   0.0f, 0.0f,
		 0.25f, -0.25f, 0.0f,   1.0f, 0.0f,
		// Right top triangle
		 0.25f, -0.25f, 0.0f,   1.0f, 0.0f,
		 0.25f,  0.25f, 0.0f,   1.0f, 1.0f,
		-0.25f,  0.25f, 0.0f,   0.0f, 1.0f
			 
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

	void ParticleRender::Render(std::vector<GameObject*> particleEmitters)
	{
		std::vector<glm::mat4> models;
		std::vector<ParticleData> particleData;

		models.push_back(Camera::ActiveCamera->GetProjectionMatrix());
		models.push_back(Camera::ActiveCamera->GetViewMatrix());
		models.push_back(glm::mat4());
		
		m_Shader->Bind();
		glBindVertexArray(m_VAO);

		for (auto p : particleEmitters)
		{
			ParticleEmitter *emitter = p->GetComponent<ParticleEmitter>().Get();

			particleData = emitter->GetAllParticleData();

			models[2] = p->GetComponent<Transform>()->GetModelMatrix();
			m_Models.Update(models.data(), 3 * sizeof(glm::mat4));
			m_Models.LockAndMovePointer();
			m_Models.Bind();

			m_ParticleData.Update(particleData.data(), particleData.size() * sizeof(ParticleData));
			m_ParticleData.LockAndMovePointer();
			m_ParticleData.Bind();

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, emitter->m_Texture->m_ID);

			glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 500);
		}

		glBindVertexArray(0);
	}

}
