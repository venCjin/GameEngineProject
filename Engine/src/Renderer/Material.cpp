#include "pch.h"
#include "Material.h"

namespace sixengine {

	Material::Material()
	{
	}

	void Material::Bind()
	{
		if (m_Shader == nullptr)
		{
			//warn
			return;
		}

		m_Shader->Bind();
		//set model transform?
		//set texture?

	}

	void Material::Unbind()
	{
		m_Shader->Unbind();
	}

	void Material::SetShader(Shader & shader)
	{
		m_Shader = &shader;
	}

	void Material::SetColor(const std::string & name, float color[])
	{
		m_Shader->SetColor(name, color);
	}


	void Material::SetName(std::string name)
	{
		m_Name = name;
	}

	std::string Material::GetName()
	{
		return m_Name;
	}

	Material::~Material()
	{

	}

}