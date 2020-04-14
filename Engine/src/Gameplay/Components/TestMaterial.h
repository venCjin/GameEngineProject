#pragma once

#include "Renderer/VertexArray.h"
#include "Renderer/Shader.h"

namespace sixengine {

	struct TestMaterial
	{
		Shader* m_Shader;

		TestMaterial(Shader* shader) : m_Shader(shader) {}

		Shader* GetShader()
		{
			return m_Shader;
		}
	};

}