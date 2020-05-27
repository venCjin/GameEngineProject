#pragma once

#include "Technique.h"

#include "Renderer/Font.h";
#include "Gameplay/Components/Text.h"

namespace sixengine {

	struct DepthFramebuffer
	{
	public:
		const unsigned int m_Width;
		const unsigned int m_Height;

		unsigned int m_FramebufferID;
		unsigned int m_TextureID;

		DepthFramebuffer(unsigned int width, unsigned int height)
			: m_Width(width), m_Height(height)
		{
			glGenTextures(1, &m_TextureID);
			glBindTexture(GL_TEXTURE_2D, m_TextureID);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
				m_Width, m_Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glGenFramebuffers(1, &m_FramebufferID);
			glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_TextureID, 0);
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		void Bind()
		{
			glViewport(0, 0, m_Width, m_Height);
			glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);
			glClear(GL_DEPTH_BUFFER_BIT);
		}

		void Unbind()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight());
		}
	};

	class DepthRender : public Technique
	{
	private:
		StorageBuffer m_Models;

	public:
		DepthRender(Shader* shader);

		void Start(TextureArray* textureArray) override;
		void Render(std::vector<RendererCommand*>& commandList, std::vector<glm::mat4>& models, std::vector<glm::vec4> layers) override;
		void SetLight(Light& light) override;
	};

}


