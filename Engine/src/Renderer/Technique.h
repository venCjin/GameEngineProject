#pragma once

#include "Renderer/Shader.h"
#include "Renderer/BufferLockManager.h"
#include "Core/Camera.h"
#include "Font.h";

#include "Gameplay/Components/UIElement.h"
#include "Gameplay/Components/Text.h"

namespace sixengine {

	class RendererCommand;
	class TextureArray;

	struct BufferStorage
	{
		size_t m_Head;
		size_t m_Size;

		unsigned int m_ID;
		void* m_Ptr;

		unsigned int m_Buffering;

		BufferStorage(size_t size, unsigned int buffering = 3)
			: m_Head(0), m_Size(size), m_Buffering(buffering) {}
	};

	class Technique
	{
	protected:
		Shader* m_Shader;
		Camera* m_Camera;

	public:
		Technique(Shader* shader, Camera* camera);
		virtual ~Technique();

		virtual void Start(TextureArray* textureArray) = 0;
		virtual void Render(std::vector<RendererCommand*>& commandList, std::vector<glm::mat4>& models, std::vector<glm::vec4> layers) = 0;

		inline Shader* GetShader() const { return m_Shader; }
		inline Camera* GetCamera() const { return m_Camera; }
	};

	class UI : public Technique
	{
	private:
		struct Vertex2D
		{
			glm::vec2 pos;
			glm::vec2 texCoord;
		};

		BufferStorage m_Models;
		//std::map<std::string, Font*> m_Fonts;
		Font* m_Font;
		unsigned int m_VAO;
		unsigned int m_VBO;

	public:
		UI(Shader* shader, Camera* camera);

		void AddFont(Font* font);
		void RenderText(Text* text, glm::vec2 position);

		void Start(TextureArray* textureArray) override;
		void Render(std::vector<RendererCommand*>& commandList, std::vector<glm::mat4>& models, std::vector<glm::vec4> layers) override;
	};

	class Debug : public Technique
	{
	private:
		BufferStorage m_Models;
		BufferStorage m_Layers;

	public:

	};

	class StaticPBR : public Technique
	{
	private:
		BufferStorage m_Models;
		BufferStorage m_Layers;

		BufferLockManager m_ModelsLockManager;
		BufferLockManager m_LayersLockManager;

	public:
		StaticPBR(Shader* shader, Camera* camera);

		void Start(TextureArray* textureArray) override;
		void Render(std::vector<RendererCommand*>& commandList, std::vector<glm::mat4>& models, std::vector<glm::vec4> layers) override;
	};

	struct BonesStruct
	{
		glm::mat4 bones[100];
	};

	class AnimationPBR : public Technique
	{
	private:
		BufferStorage m_Models;
		BufferStorage m_Layers;
		BufferStorage m_Bones;

		BufferLockManager m_ModelsLockManager;
		BufferLockManager m_LayersLockManager;
		BufferLockManager m_BonesLockManager;

		std::vector<std::vector<glm::mat4>> m_Transforms;

	public:
		AnimationPBR(Shader* shader, Camera* camera);

		void Start(TextureArray* textureArray) override;
		void Render(std::vector<RendererCommand*>& commandList, std::vector<glm::mat4>& models, std::vector<glm::vec4> layers) override;
	};

}


