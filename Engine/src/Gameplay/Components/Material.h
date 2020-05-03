#pragma once

#include "Renderer/VertexArray.h"
#include "Renderer/Shader.h"
#include <Renderer\Model.h>
#include <stb_image.h>

namespace sixengine {

	struct Material
	{
		Shader* m_Shader;
		unsigned int m_Texture;

		Material(Shader* shader, unsigned int textureLayer)
			: m_Shader(shader), m_Texture(textureLayer)
		{
		}

		inline Shader* GetShader() const { return m_Shader; }
		inline unsigned int GetTexture() const { return m_Texture; }
	};

}