#pragma once

#include "glad/glad.h"

namespace sixengine {

	class RenderTarget
	{
	private:
		unsigned int m_TextureID;
		unsigned int m_FramebufferID;

		unsigned int m_Width;
		unsigned int m_Height;

	public:
		RenderTarget(int width, int height, GLenum textureTarget, GLfloat filter, 
			GLenum internalFormat, GLenum format, bool clamp, GLenum attachment, bool depth);

		void BindTarget();
		void BindTexture();

		static void BindDefaultTarget();
	};

}

