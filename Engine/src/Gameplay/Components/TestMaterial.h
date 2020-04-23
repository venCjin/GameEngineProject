#pragma once

#include "Renderer/VertexArray.h"
#include "Renderer/Shader.h"
#include <Renderer\Model.h>
#include <stb_image.h>

namespace sixengine {

	struct TestMaterial
	{
		Shader* m_Shader;
		unsigned int m_Texture;

		TestMaterial(Shader* shader, std::string pathToTexture) : m_Shader(shader) 
		{
			LoadTextureFromFile(pathToTexture);
		}

		Shader* GetShader()
		{
			return m_Shader;
		}

		void LoadTextureFromFile(std::string path) 
		{
			unsigned int textureID;
			glGenTextures(1, &textureID);

			int width, height, nrComponents;
			unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
			if (data)
			{
				GLenum format;
				if (nrComponents == 1)
					format = GL_RED;
				else if (nrComponents == 3)
					format = GL_RGB;
				else if (nrComponents == 4)
					format = GL_RGBA;


				glActiveTexture(GL_TEXTURE0 + textureID);
				glBindTexture(GL_TEXTURE_2D, textureID);
				glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				stbi_image_free(data);
			}
			else
			{
				std::string message("Texture failed to load at path: " + std::string(path));
				LOG_WARN(message);
				stbi_image_free(data);
			}

			m_Texture = textureID;
			//return textureID;
		}
		
	};

}