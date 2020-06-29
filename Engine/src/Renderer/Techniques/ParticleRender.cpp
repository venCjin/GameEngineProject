#include "pch.h"

#include <Gameplay/GameObject.h>
#include "ParticleRender.h"

namespace sixengine {

	ParticleRender::ParticleRender(Shader* shader)
		: Technique(shader), m_Models(2 * sizeof(glm::mat4), 0, 1),
		m_ParticleData(50 * 500 * sizeof(ParticleData), 1, 3)
	{
		m_Shader->Bind();

		m_Shader->SetInt("textureName", 2);
	}

	void ParticleRender::Start(TextureArray* textureArray)
	{
		// Positions are clockwise - so initially we render backfaces towards us, but it is needed for LookAt
		float vertices[] = {
			// Left bottom triangle
		-0.25f,  0.25f, 0.0f,   0.0f, 1.0f,
		 0.25f, -0.25f, 0.0f,   1.0f, 0.0f,
		-0.25f, -0.25f, 0.0f,   0.0f, 0.0f,
		// Right top triangle
		 0.25f, -0.25f, 0.0f,   1.0f, 0.0f,
		-0.25f,  0.25f, 0.0f,   0.0f, 1.0f,
		 0.25f,  0.25f, 0.0f,   1.0f, 1.0f

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
		/*std::sort(particleEmitters.begin(), particleEmitters.end(),
			[](GameObject* a, GameObject* b) -> bool
			{
				glm::vec3 posA(a->GetComponent<Transform>()->GetWorldPosition());
				glm::vec3 posB(b->GetComponent<Transform>()->GetWorldPosition());

				glm::vec3 camPosition = Camera::ActiveCamera->m_Transform->GetWorldPosition();

				return glm::distance2(camPosition, posA) > glm::distance2(camPosition, posB);
			});*/

		std::vector<glm::mat4> models;
		std::vector<ParticleData> particleData;

		models.push_back(Camera::ActiveCamera->GetProjectionMatrix());
		models.push_back(Camera::ActiveCamera->GetViewMatrix());

		m_Models.Update(models.data(), 2 * sizeof(glm::mat4));
		m_Models.Bind();
		m_ParticleData.LockAndMovePointer();

		m_Shader->Bind();
		glBindVertexArray(m_VAO);

		for (int i = 0; i < particleEmitters.size(); i++)
		{
			auto p = particleEmitters[i];
			particleData = p->GetComponent<ParticleEmitter>()->GetAllParticleData();

			m_ParticleData.Update(particleData.data(), particleData.size() * sizeof(ParticleData), i * 500 * sizeof(ParticleData));
			glBindBufferRange(GL_SHADER_STORAGE_BUFFER, m_ParticleData.m_Binding, m_ParticleData.m_ID, m_ParticleData.m_Head + i * 500 * sizeof(ParticleData), 500 * sizeof(ParticleData));

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, p->GetComponent<ParticleEmitter>()->m_Texture->m_ID);

			glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 500);
		}

		m_ParticleData.LockAndMovePointer();
		glBindVertexArray(0);
	}

}
