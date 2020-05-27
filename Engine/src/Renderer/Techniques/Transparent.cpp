#include "pch.h"
#include "Transparent.h"

namespace sixengine
{
	TransparentTechnique::TransparentTechnique(Shader* shader, Camera* camera) : Technique(shader, camera),
		m_Models(40002 * sizeof(glm::mat4), 0), m_Layers(40000 * sizeof(glm::vec4), 1)
	{

	}
	
	void TransparentTechnique::Start(TextureArray* textureArray)
	{
		m_Shader->Bind();

		//textureArray->Bind(0);
		//m_Shader->SetInt("textureArray", 0);

		m_Shader->Unbind();
	}
	
	
	void TransparentTechnique::Render(std::vector<RendererCommand*>& commandList, std::vector<glm::mat4>& models, std::vector<glm::vec4> layers)
	{
		m_Shader->Bind();
		glEnable(GL_BLEND);
		m_Models.Update(models.data(), models.size() * sizeof(models[0]));
		m_Layers.Update(layers.data(), layers.size() * sizeof(layers[0]));
	}
}