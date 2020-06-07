#pragma once
class Texture
{
public:
	Texture(std::string path, unsigned int width, unsigned int height);
	~Texture();
	
	unsigned int m_ID;


private:
	unsigned int m_Width, m_Height;
	unsigned int m_TextureLayers;
	unsigned int m_TexturesPerMaterial;
	const unsigned int m_MipmapLevel = 6;

	void LoadTexture(std::string path);
};

