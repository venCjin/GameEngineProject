#pragma once
#include <Engine.h>
// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H

namespace sixengine 
{
	class Font
	{
	public:
		struct Character {
			unsigned int TextureID;  // ID handle of the glyph texture
			glm::ivec2   Size;       // Size of glyph
			glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
			unsigned int Advance;    // Offset to advance to next glyph
		};


		Font(std::string path);
		~Font();
		 Font::Character getChar(char c);
	private:
		FT_Library m_Ft;
		FT_Face m_Face;
	};

}

