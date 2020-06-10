#include "pch.h"
#include "Technique.h"

namespace sixengine {

	Technique::Technique(Shader* shader)
		: m_Shader(shader), m_Offset(0), m_Size(0), m_Visible(false)
	{
	}

	Technique::~Technique()
	{
		m_Shader = nullptr;
	}

	void Technique::SetLight(Light& light)
	{

	}

	void Technique::FinishFrame()
	{

	}

}