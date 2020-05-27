#include "pch.h"
#include "Technique.h"

namespace sixengine {

	Technique::Technique(Shader* shader)
		: m_Shader(shader)
	{
		m_Shader = shader;
	}

	Technique::~Technique()
	{
		m_Shader = nullptr;
	}

}