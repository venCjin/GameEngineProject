#include "pch.h"
#include "StaticPBR.h"

#include "Renderer/TextureArray.h"
#include "Renderer/BatchRenderer.h"

namespace sixengine {

	StaticPBR::StaticPBR(Shader* shader)
		: Technique(shader), m_Models(10002 * sizeof(glm::mat4), 0), m_Layers(10000 * sizeof(glm::vec4), 1),
		m_Lights(sizeof(LightData), 2)
	{

		
	}

	void StaticPBR::Start(TextureArray* textureArray)
	{
		m_Shader->Bind();

		textureArray->Bind(0);
		m_Shader->SetInt("textureArray", 0);

		m_Shader->Unbind();
	}

	void StaticPBR::Render(std::vector<RendererCommand*>& commandList, std::vector<glm::mat4>& models, std::vector<glm::vec4> layers)
	{
		m_Shader->Bind();

		
		m_Models.Update(models.data(), models.size() * sizeof(models[0]));
		m_Layers.Update(layers.data(), layers.size() * sizeof(layers[0]));
	}

	void StaticPBR::SetLight(Light& light)
	{
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, light.m_DepthFramebuffer.m_TextureID);
		m_Shader->SetInt("shadowMap1", light.m_DepthFramebuffer.m_TextureID);

		glm::mat4 lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, 1.0f, 50.0f);
		glm::mat4 lightView = glm::lookAt(glm::vec3(-2.0f, 24.0f, -1.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));

		glm::mat4 lightSpaceMatrix = lightProjection * lightView;

		m_Shader->SetMat4("lightSpaceMatrix1", lightSpaceMatrix);

		Light tmp = light;
		tmp.m_LightData.dirLight.position = glm::vec4(Camera::ActiveCamera->GetViewMatrix() * glm::vec4(light.m_DirectionalLightPos, 1.0f));
		tmp.m_LightData.dirLight.direction = glm::vec4(Camera::ActiveCamera->GetViewMatrix() * glm::vec4(-light.m_DirectionalLightPos, 0.0f));

		m_Lights.Update(&tmp, sizeof(tmp));
	}

}