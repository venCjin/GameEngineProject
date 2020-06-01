#pragma once

#include "Technique.h"

#include "Renderer/Font.h";
#include "Gameplay/Components/Text.h"

namespace sixengine {

	struct DepthFramebuffer
	{
	public:
		const unsigned int m_Size;

		unsigned int m_FramebufferID;
		unsigned int m_TextureID;

		DepthFramebuffer(unsigned int size)
			: m_Size(size)
		{
			glGenTextures(1, &m_TextureID);
			glBindTexture(GL_TEXTURE_2D, m_TextureID);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
				m_Size, m_Size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER); 
			float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

			glGenFramebuffers(1, &m_FramebufferID);
			glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_TextureID, 0);
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		DepthFramebuffer()
			: m_Size(4096)
		{
			glGenTextures(1, &m_TextureID);
			glBindTexture(GL_TEXTURE_2D, m_TextureID);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F,
				m_Size, m_Size, 0, GL_RGBA, GL_FLOAT, NULL);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

			glGenFramebuffers(1, &m_FramebufferID);
			glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureID, 0);
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		void Bind()
		{
			glViewport(0, 0, m_Size, m_Size);
			glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);
			//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			//glClear(GL_COLOR_BUFFER_BIT);
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
	public:
		DepthRender(Shader* shader);

		void Start(TextureArray* textureArray) override;
		void StartFrame(std::vector<RendererCommand*>& commandList, std::vector<DrawElementsCommand> drawCommands,
			std::vector<glm::mat4>& models, std::vector<glm::vec4> layers) override;
		void Render(std::vector<RendererCommand*>& commandList) override;
		void DepthRender::SetLight(Light& light);
	};

}


