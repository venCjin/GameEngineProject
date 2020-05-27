#include "pch.h"
#include "DepthRender.h"

#include "Renderer/BatchRenderer.h"

namespace sixengine {



	DepthRender::DepthRender(Shader* shader)
		: Technique(shader), m_Models(10000 * sizeof(glm::mat4), 6)
	{
	}

	void DepthRender::Start(TextureArray* textureArray)
	{
	}

	void DepthRender::Render(std::vector<RendererCommand*>& commandList, std::vector<glm::mat4>& models, std::vector<glm::vec4> layers)
	{
		m_Shader->Bind();

		m_Models.Update(models.data(), models.size() * sizeof(models[0]));

		
	}

	void DepthRender::SetLight(Light& light)
	{
		glm::mat4 lightProjection = light.ortho;
		glm::mat4 lightView = glm::lookAt(light.m_DirectionalLightPos,
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));

		glm::mat4 lightSpaceMatrix = lightProjection * lightView;

		m_Shader->SetMat4("lightSpaceMatrix", lightSpaceMatrix);
	}

}