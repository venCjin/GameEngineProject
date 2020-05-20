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
		struct Character
		{
			float			Offset;
			glm::ivec2		Size;       // Size of glyph
			glm::ivec2		Bearing;    // Offset from baseline to left/top of glyph
			unsigned int	Advance;    // Offset to advance to next glyph
		};

	public:
		Font(std::string path);
		~Font();

		void Bind(unsigned int slot = 0);

		inline Character GetChar(char index) const { return m_Characters[index - 32]; }
		inline std::string GetName() const { return m_FontName; }
		inline unsigned int GetFontID() const { return m_Font; }

		inline unsigned int GetAtlasWidth() const { return m_AtlasWidth; }
		inline unsigned int GetAtlasHeight() const { return m_AtlasHeight; }

	private:
		void LoadFont();

	private:
		FT_Library m_FT;
		FT_Face m_Face;

		unsigned int m_Font;
		unsigned int m_AtlasWidth;
		unsigned int m_AtlasHeight;
		std::string m_FontName;
		Character m_Characters[96];
	};

}

