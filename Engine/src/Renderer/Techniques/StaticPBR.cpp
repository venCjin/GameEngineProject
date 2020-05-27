#include "pch.h"
#include "StaticPBR.h"

#include "Renderer/TextureArray.h"

namespace sixengine {

	StaticPBR::StaticPBR(Shader* shader)
		: Technique(shader), m_Models(1002 * sizeof(glm::mat4), 0), m_Layers(1000 * sizeof(glm::vec4), 1),
		m_Lights(sizeof(LightData), 2)
	{

		m_DirectionalLightPos = glm::vec3(-2.0f, 24.0f, -1.0f);

		m_LightData.ao = 0.4f;
		m_LightData.metallic = 0.4f;
		m_LightData.roughness = 0.6f;

		m_LightData.dirLight.position = glm::vec4(Camera::ActiveCamera->GetViewMatrix() * glm::vec4(m_DirectionalLightPos, 1.0f));
		m_LightData.dirLight.direction = glm::vec4(Camera::ActiveCamera->GetViewMatrix() * glm::vec4(-m_DirectionalLightPos, 0.0f));
		m_LightData.dirLight.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
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

		m_LightData.dirLight.position = glm::vec4(Camera::ActiveCamera->GetViewMatrix() * glm::vec4(m_DirectionalLightPos, 1.0f));
		m_LightData.dirLight.direction = glm::vec4(Camera::ActiveCamera->GetViewMatrix() * glm::vec4(-m_DirectionalLightPos, 0.0f));

		m_Models.Update(models.data(), models.size() * sizeof(models[0]));
		m_Layers.Update(layers.data(), layers.size() * sizeof(layers[0]));
		m_Lights.Update(&m_LightData, sizeof(m_LightData));
	}

}