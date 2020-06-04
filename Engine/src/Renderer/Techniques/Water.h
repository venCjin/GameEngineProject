#pragma once

#include "Technique.h"

#include "Renderer/Font.h";
#include "Gameplay/Components/Text.h"

namespace sixengine {

	struct WaterFrameBuffers
	{
	private:
		const unsigned int m_ReflectionWidth = 320;
		const unsigned int m_ReflectionHeight = 180;
		unsigned int m_ReflectionFrameBufferID;
		unsigned int m_ReflectionTextureID;
		unsigned int m_ReflectionDepthBufferID;
		
		const unsigned int m_RefractionWidth = 1280;
		const unsigned int m_RefractionHeight = 720;
		unsigned int m_RefractionFrameBufferID;
		unsigned int m_RefractionTextureID;
		unsigned int m_RefractionDepthTextureID;

	public:
		WaterFrameBuffers()
		{
			InitReflectionFrameBuffer();
			InitRefractionFrameBuffer();
		}

		void BindReflectionFramebuffer() { Bind(m_ReflectionFrameBufferID, m_ReflectionWidth, m_ReflectionHeight); }
		
		void BindRefractionFramebuffer() { Bind(m_RefractionFrameBufferID, m_RefractionWidth, m_RefractionHeight); }

		void Unbind()
		{
			//glDrawBuffer(GL_NONE);
			//glReadBuffer(GL_NONE);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight());
		}

		inline unsigned int GetReflectionTexture() { return m_ReflectionTextureID; }

		inline unsigned int GetRefractionTexture() { return m_RefractionTextureID; }

		inline unsigned int GetRefractionDepthTexture() { return m_RefractionDepthTextureID; }

	private:
		void InitReflectionFrameBuffer()
		{
			CreateFrameBuffer(m_ReflectionFrameBufferID);
			CreateTextureAttachment(m_ReflectionTextureID, m_ReflectionWidth, m_ReflectionHeight);
			CreateDepthBufferAttachment(m_ReflectionDepthBufferID, m_ReflectionWidth, m_ReflectionHeight);
			Unbind();
		}

		void InitRefractionFrameBuffer()
		{
			CreateFrameBuffer(m_RefractionFrameBufferID);
			CreateTextureAttachment(m_RefractionTextureID, m_RefractionWidth, m_RefractionHeight);
			CreateDepthTextureAttachment(m_RefractionDepthTextureID, m_RefractionWidth, m_RefractionHeight);
			Unbind();
		}

		void CreateFrameBuffer(unsigned int& frameBuffer) {
			glGenFramebuffers(1, &frameBuffer);
			//generate name for frame buffer
			glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
			//create the framebuffer
			glDrawBuffer(GL_COLOR_ATTACHMENT0);
			//indicate that we will always render to color attachment 0
		}

		void CreateTextureAttachment(unsigned int& textureID, unsigned int width, unsigned int height)
		{
			glGenTextures(1, &textureID);
			glBindTexture(GL_TEXTURE_2D, textureID);
			//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			//float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);
			//glDrawBuffer(GL_NONE);
			//glReadBuffer(GL_NONE);
		}
		
		void CreateDepthBufferAttachment(unsigned int& depthbufferID, unsigned int width, unsigned int height)
		{
			glGenRenderbuffers(1, &depthbufferID);
			glBindRenderbuffer(GL_RENDERBUFFER, depthbufferID);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthbufferID);
		}

		void CreateDepthTextureAttachment(unsigned int& depthtextureID, unsigned int width, unsigned int height)
		{
			glGenTextures(1, &depthtextureID);
			glBindTexture(GL_TEXTURE_2D, depthtextureID);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthtextureID, 0);
		}
	
		void Bind(unsigned int framebuffer, unsigned int width, unsigned int height)
		{
			glBindTexture(GL_TEXTURE_2D, 0); //To make sure the texture isn't bound
			glViewport(0, 0, width, height);
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
			// //glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			// //glClear(GL_COLOR_BUFFER_BIT);
			//glClear(GL_DEPTH_BUFFER_BIT);
		}
	};

	class Water : public Technique
	{
	private:
		UniformBuffer m_Models;
		UniformBuffer m_Layers;

		WaterFrameBuffers m_FrameBuffers;

	public:
		Water(Shader* shader);

		void Start(TextureArray* textureArray) override;
		void StartFrame(std::vector<RendererCommand*>& commandList, std::vector<DrawElementsCommand> drawCommands,
			std::vector<glm::mat4>& models, std::vector<glm::vec4> layers) override;
		void Render(std::vector<RendererCommand*>& commandList) override;
		void FinishFrame() override;
		void Water::SetLight(Light& light);

		//moje
		inline WaterFrameBuffers& GetFrameBuffers() { return m_FrameBuffers; }
	};

}


