#include "pch.h"
#include "Texture.h"
#include <glad/glad.h>
#include <stb_image.h>

Texture::Texture(std::string path, unsigned int width, unsigned int height) : m_Width(width), m_Height(height)
{
	LoadTexture(path);
}


Texture::~Texture()
{
}

void Texture::LoadTexture(std::string path)
{
	glGenTextures(1, &m_ID);
	glBindTexture(GL_TEXTURE_2D, m_ID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;

	stbi_set_flip_vertically_on_load(false);
	unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

	if (data)
	{
		if (width != m_Width || height != m_Height)
		{
			LOG_WARN("Texture failed to load due to wrong resolution: {0}", path);
			return;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		LOG_WARN("Texture failed to load: {0}", path);
	}
	stbi_image_free(data);

}
