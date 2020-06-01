#include "pch.h"
#include "Transparent.h"

namespace sixengine
{
	TransparentTechnique::TransparentTechnique(Shader* shader) 
		: Technique(shader),
		m_Models(12 * sizeof(glm::mat4), 0),
		m_Layers(10 * sizeof(glm::vec4), 1)
	{
		
	}

	void TransparentTechnique::Start(TextureArray* textureArray)
	{
		m_Shader->Bind();

		//textureArray->Bind(0);
		m_Shader->SetVec3("color", glm::vec3(1.0f, .6f, .4f));
		m_Shader->SetFloat("FresnelExponent", 1.f);

		m_Shader->Unbind();
	}

	void TransparentTechnique::StartFrame(std::vector<RendererCommand*>& commandList, std::vector<DrawElementsCommand> drawCommands, std::vector<glm::mat4>& models, std::vector<glm::vec4> layers)
	{
		m_DrawCommands = drawCommands;

		m_Offset = 0;
		m_Size = 0;

		m_Models.Update(models.data(), models.size() * sizeof(models[0]));
		m_Layers.Update(layers.data(), layers.size() * sizeof(layers[0]));
	}

	
	void TransparentTechnique::Render(std::vector<RendererCommand*>& commandList)
	{
		m_Shader->Bind();
		m_Shader->SetVec3("viewDir", Camera::ActiveCamera->m_Transform->GetForward());

		m_Models.Bind();
		m_Layers.Bind();
	}

	void TransparentTechnique::FinishFrame()
	{
		m_Models.LockAndMovePointer();
		m_Layers.LockAndMovePointer();
	}
}