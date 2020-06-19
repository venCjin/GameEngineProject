#pragma once

namespace sixengine {

	class Texture
	{
	public:
		Texture(std::string path);
		~Texture();

		unsigned int m_ID;


	private:
		unsigned int m_Width, m_Height;

		void LoadTexture(std::string path);
	};

}


