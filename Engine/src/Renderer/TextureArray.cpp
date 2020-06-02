#include "pch.h"
#include "TextureArray.h"

#include <glad/glad.h>
#include <stb_image.h>
#include <string>

#include <Renderer/Shader.h>

unsigned int sixengine::TextureArray::AddTexture(std::string path)
{
	std::size_t slash = path.find_last_of("/\\");
	std::size_t dot = path.find_last_of(".");
	std::string textureName = path.substr(slash + 1, dot - slash - 1);

	if (m_TextureMap.find(textureName) == m_TextureMap.end())
	{
		int width, height, nrChannels;

		stbi_set_flip_vertically_on_load(false);
		unsigned char* textureData = stbi_load(path.c_str(), &width, &height, &nrChannels, 4);

		if (textureData)
		{
			if (width == m_Width && height == m_Height)
				m_TextureData.push_back(textureData);
			else
				LOG_WARN("Texture failed to load due to wrong resolution: {0}", path);
		}
		else
		{
			LOG_WARN("Texture failed to load at path: {0}", path);
		}

		m_TextureMap[textureName] = m_TextureLayers;

		m_TextureLayers++;
	}

	return GetTexture(textureName);
}

void sixengine::TextureArray::CreateTextureArray()
{
	glGenTextures(1, &m_ID);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_ID);

	glTexStorage3D(GL_TEXTURE_2D_ARRAY, m_MipmapLevel, GL_RGBA8, 2048, 2048, m_TextureData.size());

	for (int i = 0; i < m_TextureData.size(); i++)
	{
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_ID);
		//glCompressedTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, m_Width, m_Height, 1, GL_RGBA, GL_UNSIGNED_BYTE, m_TextureData[i]);
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, m_Width, m_Height, 1, GL_RGBA, GL_UNSIGNED_BYTE, m_TextureData[i]);
		stbi_image_free(m_TextureData[i]);
	}

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

	m_TextureData.clear();
}

void sixengine::TextureArray::Bind(unsigned int slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_ID);
}

sixengine::TextureArray::TextureArray(unsigned int width, unsigned int height, unsigned int texturesPerMaterial)
	: m_TextureLayers(0), m_Width(width), m_Height(height), m_TexturesPerMaterial(texturesPerMaterial)
{
}

sixengine::TextureArray::~TextureArray()
{
}
