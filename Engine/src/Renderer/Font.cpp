
#include "pch.h"
#include "Font.h"



namespace sixengine {
	Font::Font(std::string path)
	{
		int error = FT_Init_FreeType(&m_Ft);
		if (error)
			LOG_CORE_ERROR("ERROR::FREETYPE: Could not init FreeType Library");

		error = FT_New_Face(m_Ft, path.c_str(), 0, &m_Face);
		if (error)
			LOG_CORE_ERROR( "ERROR::FREETYPE: Failed to load font. error = {0}", error);


		FT_Set_Pixel_Sizes(m_Face, 0, 48);

	}

	Font::~Font()
	{
		FT_Done_Face(m_Face);
		FT_Done_FreeType(m_Ft);
	}


	Font::Character Font::getChar(char c)
	{
		int error = FT_Load_Char(m_Face, c, FT_LOAD_RENDER);
		if (error) 
		{
			LOG_CORE_ERROR("ERROR::FREETYTPE: Failed to load Glyph, error = {0}", error);
			Character ch{
				0,
				glm::ivec2(m_Face->glyph->bitmap.width, m_Face->glyph->bitmap.rows),
				glm::ivec2(m_Face->glyph->bitmap_left, m_Face->glyph->bitmap_top),
				m_Face->glyph->advance.x
			};
			return ch;
		}
		//is not working fix
		unsigned int texture;
		glGenTextures(1, &texture);
		LOG_CORE_INFO("{0}", texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGB,
			m_Face->glyph->bitmap.width,
			m_Face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			m_Face->glyph->bitmap.buffer
		);

		// set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		Character ch{
			texture,
			glm::ivec2(m_Face->glyph->bitmap.width, m_Face->glyph->bitmap.rows),
			glm::ivec2(m_Face->glyph->bitmap_left, m_Face->glyph->bitmap_top),
			m_Face->glyph->advance.x
		};
		return ch;
	}
}





