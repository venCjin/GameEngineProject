#include "pch.h"
#include "Water.h"

#include "Renderer/BatchRenderer.h"

namespace sixengine {

	Water::Water(Shader* shader)
		: Technique(shader),
		m_Models(12 * sizeof(glm::mat4), 0),
		m_Layers(10 * sizeof(glm::vec4), 1)
	{
	}

	void Water::Start(TextureArray* textureArray)
	{
	}

	void Water::StartFrame(std::vector<RendererCommand*>& commandList, std::vector<DrawElementsCommand> drawCommands, std::vector<glm::mat4>& models, std::vector<glm::vec4> layers)
	{
		m_DrawCommands = drawCommands;

		m_Offset = 0;
		m_Size = 0;

		LOG_CORE_INFO("m_DrawCommands.empty() : {0}", m_DrawCommands.empty());

		m_Models.Update(models.data(), models.size() * sizeof(models[0]));
		//m_Layers.Update(layers.data(), layers.size() * sizeof(layers[0]));
	}

	void Water::Render(std::vector<RendererCommand*>& commandList)
	{
		LOG_CORE_INFO("Water::Render");
		m_Shader->Bind();

		m_Models.Bind();
		//m_Layers.Bind();
	}

	void Water::FinishFrame()
	{
		m_Models.LockAndMovePointer();
		//m_Layers.LockAndMovePointer();
	}

	void Water::SetLight(Light& light)
	{
	}

}