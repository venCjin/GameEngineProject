#include "pch.h"
#include "StaticPBR.h"

#include "Renderer/TextureArray.h"
#include "Renderer/BatchRenderer.h"

namespace sixengine {

	StaticPBR::StaticPBR(Shader* shader)
		: Technique(shader), 
		m_Models(1002 * sizeof(glm::mat4), 0), 
		m_Layers(1000 * sizeof(glm::vec4), 1),
		m_Lights(sizeof(LightData), 2)
	{

		
	}

	void StaticPBR::Start(TextureArray* textureArray)
	{
		m_Shader->Bind();

		textureArray->Bind(0);
		m_Shader->SetInt("textureArray", 0);
		m_Shader->SetInt("shadowMap1", 3);

		m_Shader->Unbind();
	}

	void StaticPBR::StartFrame(std::vector<RendererCommand*>& commandList, std::vector<DrawElementsCommand> drawCommands, std::vector<glm::mat4>& models, std::vector<glm::vec4> layers)
	{
		m_DrawCommands = drawCommands;

		m_Offset = 0;
		m_Size = 0;

		m_Models.Update(models.data(), models.size() * sizeof(models[0]));
		m_Layers.Update(layers.data(), layers.size() * sizeof(layers[0]));
	}

	void StaticPBR::Render(std::vector<RendererCommand*>& commandList)
	{
		m_Shader->Bind();

		m_Shader->SetVec3("cameraPos", Camera::ActiveCamera->m_Transform->GetWorldPosition());

		m_Models.Bind();
		m_Layers.Bind();
		m_Lights.Bind();
	}

	void StaticPBR::SetLight(Light& light)
	{
		m_Shader->Bind();

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, light.m_DepthFramebuffer.m_TextureID);

		glm::mat4 lightSpaceMatrix = light.m_Ortho * light.m_LightView;

		m_Shader->SetMat4("lightSpaceMatrix1", lightSpaceMatrix);

		Light tmp = light;
		tmp.m_LightData.dirLight.position = glm::vec4(Camera::ActiveCamera->GetViewMatrix() * glm::vec4(light.m_DirectionalLightPos, 1.0f));
		tmp.m_LightData.dirLight.direction = glm::vec4(Camera::ActiveCamera->GetViewMatrix() * glm::vec4(-light.m_DirectionalLightPos, 0.0f));

		m_Lights.Update(&tmp.m_LightData, sizeof(tmp.m_LightData));
	}

	void StaticPBR::FinishFrame()
	{
		m_Models.LockAndMovePointer();
		m_Layers.LockAndMovePointer();
		m_Lights.LockAndMovePointer();
	}

}