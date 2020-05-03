#pragma once

#include <map>
#include <vector>

namespace sixengine {

	class TextureArray
	{
	public:
		void AddTexture(std::string path);
		void CreateTextureArray();
		unsigned int GetTexture(std::string textureName) { return m_TextureMap[textureName] % m_TexturesPerMaterial; }
		void Bind() const;

		TextureArray(unsigned int width, unsigned int height, unsigned int texturesPerMaterial = 1);
		~TextureArray();

	private:
		unsigned int m_ID;
		unsigned int m_Width, m_Height;
		unsigned int m_TextureLayers;
		unsigned int m_TexturesPerMaterial;

		std::vector<unsigned char*> m_TextureData;
		std::map<std::string, unsigned int> m_TextureMap;	
	};

}