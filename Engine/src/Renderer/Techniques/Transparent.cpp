#include "pch.h"
#include "Transparent.h"

namespace sixengine
{
	TransparentTechnique::TransparentTechnique(Shader* shader) : Technique(shader),
		m_Models(40002 * sizeof(glm::mat4), 0), m_Layers(40000 * sizeof(glm::vec4), 1)
	{

	}
	
	void TransparentTechnique::Start(TextureArray* textureArray)
	{
		m_Shader->Bind();

		//textureArray->Bind(0);
		m_Shader->SetVec3("color", glm::vec3(1.0f, .6f, .4f));
		m_Shader->SetFloat("FresnelExponent", 1.f);
		m_Shader->SetInt("OnSurface", 1);
		m_Shader->Unbind();
	}

	void TransparentTechnique::SetOnSurface(bool value) {
		m_Shader->Bind();
		m_Shader->SetInt("OnSurface", (int)value);
		m_Shader->Unbind();
	}
	
	
	void TransparentTechnique::Render(std::vector<RendererCommand*>& commandList, std::vector<glm::mat4>& models, std::vector<glm::vec4> layers)
	{
		m_Shader->Bind();
		glm::vec3 v = Camera::ActiveCamera->m_Transform->GetForward();
		m_Shader->SetVec3("viewDir", v);
		LOG_CORE_INFO("{0} {1} {2}", v.x, v.y, v.z);
		glEnable(GL_BLEND);
		m_Models.Update(models.data(), models.size() * sizeof(models[0]));
		m_Layers.Update(layers.data(), layers.size() * sizeof(layers[0]));
	}
}