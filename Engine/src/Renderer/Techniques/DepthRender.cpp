#include "pch.h"
#include "DepthRender.h"

#include "Renderer/BatchRenderer.h"

namespace sixengine {



	DepthRender::DepthRender(Shader* shader)
		: Technique(shader)
	{
	}

	void DepthRender::Start(TextureArray* textureArray)
	{
	}

	void DepthRender::StartFrame(std::vector<RendererCommand*>& commandList, std::vector<DrawElementsCommand> drawCommands, std::vector<glm::mat4>& models, std::vector<glm::vec4> layers)
	{
	}

	void DepthRender::Render(std::vector<RendererCommand*>& commandList)
	{
		m_Shader->Bind();
	}

	void DepthRender::SetLight(Light& light)
	{
		glm::mat4 lightSpaceMatrix = light.m_Ortho * light.m_LightView;

		m_Shader->Bind();
		m_Shader->SetMat4("lightSpaceMatrix", lightSpaceMatrix);
	}

}