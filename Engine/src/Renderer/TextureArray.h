#pragma once

#include <map>
#include <vector>

namespace sixengine {

	class TextureArray
	{
	public:
		unsigned int AddTexture(std::string path);
		void CreateTextureArray();
		void Bind(unsigned int slot = 0) const;

		inline unsigned int GetTexture(std::string textureName) { return m_TextureMap[textureName] / m_TexturesPerMaterial; }

		TextureArray(unsigned int width, unsigned int height, unsigned int texturesPerMaterial = 1);
		~TextureArray();

	private:
		unsigned int m_ID;
		unsigned int m_Width, m_Height;
		unsigned int m_TextureLayers;
		unsigned int m_TexturesPerMaterial;
		const unsigned int m_MipmapLevel = 6;

		std::vector<unsigned char*> m_TextureData;
		std::map<std::string, unsigned int> m_TextureMap;	
	};

}