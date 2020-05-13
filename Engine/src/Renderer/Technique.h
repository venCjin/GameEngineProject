#pragma once

#include "Renderer/Shader.h"
#include "Core/Camera.h"

namespace sixengine {

	class RendererCommand;
	class TextureArray;

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

	class Debug : public Technique
	{
	private:

	public:

	};

	class StaticPBR : public Technique
	{
	private:
		unsigned int m_ModelsSSBO;
		unsigned int m_LayersSSBO;

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
		unsigned int m_ModelsSSBO;
		unsigned int m_LayersSSBO;
		unsigned int m_BonesSSBO;

		std::vector<std::vector<glm::mat4>> m_Transforms;

	public:
		AnimationPBR(Shader* shader, Camera* camera);

		void Start(TextureArray* textureArray) override;
		void Render(std::vector<RendererCommand*>& commandList, std::vector<glm::mat4>& models, std::vector<glm::vec4> layers) override;
	};

}


