#include "pch.h"
#include "Water.h"

#include "Renderer/BatchRenderer.h"

namespace sixengine {

	Water::Water(Shader* shader, GameObject* go)
		: Technique(shader), m_Go(go),
		m_Models(12 * sizeof(glm::mat4), 0),
		m_Layers(10 * sizeof(glm::vec4), 1)
	{
	}

	void Water::Start(TextureArray* textureArray)
	{
		m_Shader->Bind();

		textureArray->Bind(0);
		m_Shader->SetInt("textureArray", 0);

		m_Shader->SetInt("reflectTex", 1);
		m_Shader->SetInt("refractTex", 2);
		m_Shader->SetInt("refractDepthTex", 3);

		m_Shader->Unbind();
	}

	void Water::StartFrame(std::vector<RendererCommand*>& commandList, std::vector<DrawElementsCommand> drawCommands, std::vector<glm::mat4>& models, std::vector<glm::vec4> layers)
	{
		m_DrawCommands = drawCommands;

		m_Offset = 0;
		m_Size = 0;

		m_Models.Update(models.data(), models.size() * sizeof(models[0]));
		m_Layers.Update(layers.data(), layers.size() * sizeof(layers[0]));
	}

	void Water::Render(std::vector<RendererCommand*>& commandList)
	{
		m_Shader->Bind();

		m_MoveFactor += WAVE_SPEED * Timer::Instance()->ElapsedTime();
		m_MoveFactor = std::fmod(m_MoveFactor, 1);
		m_Shader->SetFloat("moveFactor", m_MoveFactor);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_FrameBuffers.GetReflectionTexture());

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, m_FrameBuffers.GetRefractionTexture());

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, m_FrameBuffers.GetRefractionDepthTexture());

		m_Models.Bind();
		m_Layers.Bind();
	}

	void Water::FinishFrame()
	{
		m_Models.LockAndMovePointer();
		m_Layers.LockAndMovePointer();
	}

	void Water::SetLight(Light& light)
	{
	}

}