#include "pch.h"
#include "Material.h"

namespace sixengine {

	Material::Material(Shader* shader, glm::vec4 texture, std::string name)
		: m_Shader(shader), m_Texture(texture), m_Name(name)
	{
	}

	Material::~Material()
	{

	}

	void Material::SetShader(Shader* shader)
	{
		m_Shader = shader;
	}

	void Material::SetTexture(glm::vec4 texture)
	{
		m_Texture = texture;
	}


	void Material::SetName(const std::string& name)
	{
		m_Name = name;
	}
}