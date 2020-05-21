
#include "pch.h"
#include "Font.h"

namespace sixengine {

	Font::Font(std::string path)
		: m_AtlasWidth(0), m_AtlasHeight(0)
	{
		int error = FT_Init_FreeType(&m_FT);
		if (error)
			LOG_CORE_ERROR("ERROR::FREETYPE: Could not init FreeType Library");

		error = FT_New_Face(m_FT, path.c_str(), 0, &m_Face);
		if (error)
			LOG_CORE_ERROR( "ERROR::FREETYPE: Failed to load font. error = {0}", error);


		FT_Set_Pixel_Sizes(m_Face, 0, 148);

		LoadFont();
	}

	Font::~Font()
	{
		FT_Done_Face(m_Face);
		FT_Done_FreeType(m_FT);
	}

	void Font::Bind(unsigned int slot)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_Font);
	}

	void Font::LoadFont()
	{
		FT_GlyphSlot g = m_Face->glyph;
		int w = 0;
		int offset = 5;

		for (int i = 32; i < 128; i++) 
		{
			FT_Load_Char(m_Face, i, FT_LOAD_RENDER);

			w += g->bitmap.width + offset;
			m_AtlasHeight = max(m_AtlasHeight, g->bitmap.rows);
		}

		m_AtlasWidth = w;

		glGenTextures(1, &m_Font);
		glBindTexture(GL_TEXTURE_2D, m_Font);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_AtlasWidth, m_AtlasHeight, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
		int x = 0;
		for (int i = 32; i < 128; i++) 
		{
			FT_Load_Char(m_Face, i, FT_LOAD_RENDER);

			glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, g->bitmap.width, g->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);
			m_Characters[i - 32].Advance = g->advance.x >> 6;

			m_Characters[i - 32].Size.x = g->bitmap.width;
			m_Characters[i - 32].Size.y = g->bitmap.rows;

			m_Characters[i - 32].Bearing.x = g->bitmap_left;
			m_Characters[i - 32].Bearing.y = g->bitmap_top;

			m_Characters[i - 32].Offset = (float)x / w;

			x += g->bitmap.width + offset;
		}

		//m_Characters[0].Size.x = 60;
	}

}





