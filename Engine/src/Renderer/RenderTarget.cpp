#include "pch.h"
#include "RenderTarget.h"
#include "Core/Application.h"

namespace sixengine {

	RenderTarget::RenderTarget(int width, int height, GLenum textureTarget, GLfloat filter, GLenum internalFormat, GLenum format, bool clamp, GLenum attachment, bool depth)
		: m_Width(width), m_Height(height)
	{
		glGenTextures(1, &m_TextureID);
		glBindTexture(textureTarget, m_TextureID);

		glTexParameteri(textureTarget, GL_TEXTURE_MIN_FILTER, filter);
		glTexParameteri(textureTarget, GL_TEXTURE_MAG_FILTER, filter);

		if (clamp)
		{
			glTexParameteri(textureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(textureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		}
		else
		{
			glTexParameteri(textureTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(textureTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}

		glTexImage2D(textureTarget, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, 0);

		glBindTexture(textureTarget, 0);

		glGenFramebuffers(1, &m_FramebufferID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);

		if (depth)
		{
			GLuint depthrenderbuffer;
			glGenRenderbuffers(1, &depthrenderbuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_Width, m_Height);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, m_TextureID, 0);

		GLenum DrawBuffers[1] = { attachment };
		glDrawBuffers(1, DrawBuffers);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			LOG_CORE_ERROR("Framebuffer error");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void RenderTarget::BindTarget()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);
		glViewport(0, 0, m_Width, m_Height);
	}

	void RenderTarget::BindTexture()
	{
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
	}

	void RenderTarget::BindDefaultTarget()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight());
	}

}

