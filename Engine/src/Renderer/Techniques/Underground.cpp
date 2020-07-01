#include "pch.h"
#include "Underground.h"

namespace sixengine
{

	UndergroundTechnique::UndergroundTechnique(Shader* shader)
		: Technique(shader),
		m_Models(12 * sizeof(glm::mat4), 0)
	{
	}

	void UndergroundTechnique::Start(TextureArray* textureArray)
	{
		m_Shader->Bind();
		m_Shader->SetVec3("color", glm::vec3(255.0f/255.0f, 160.0f/255.0f, 80.0f/255.0f));
		m_Shader->SetFloat("FresnelExponent", 1.0f);
		//m_Shader->Unbind();
	}



	void UndergroundTechnique::StartFrame(std::vector<RendererCommand*>& commandList, std::vector<DrawElementsCommand> drawCommands, std::vector<glm::mat4>& models, std::vector<glm::vec4> layers)
	{
		m_DrawCommands = drawCommands;

		m_Offset = 0;
		m_Size = 0;

		m_Models.Update(models.data(), models.size() * sizeof(models[0]));
	}

	
	void UndergroundTechnique::Render(std::vector<RendererCommand*>& commandList)
	{
		m_Shader->Bind();
		m_Shader->SetVec3("viewDir", Camera::ActiveCamera->m_Transform->GetForward());

		m_Models.Bind();
	}

	void UndergroundTechnique::FinishFrame()
	{
		m_Models.LockAndMovePointer();
	}
}