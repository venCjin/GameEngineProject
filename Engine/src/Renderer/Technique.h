#pragma once

#include "Renderer/Shader.h"
#include "Core/Camera.h"

namespace sixengine {

	class RendererCommand;

	class Technique
	{
	protected:
		Shader* m_Shader;
		Camera* m_Camera;

	public:
		Technique(Shader* shader, Camera* camera);
		virtual ~Technique();

		virtual void Start() = 0;
		virtual void Render(std::vector<RendererCommand*>& commandList) = 0;

		inline Shader* GetShader() const { return m_Shader; }
	};

	class Debug : public Technique
	{
	private:

	public:

	};

	class StaticPBR : public Technique
	{
	private:

	public:
		StaticPBR(Shader* shader, Camera* camera);

		void Start() override;
		void Render(std::vector<RendererCommand*>& commandList) override;
	};

	struct BonesStruct
	{
		glm::mat4 bones[52];
	};

	class AnimationPBR : public Technique
	{
	private:
		unsigned int m_BonesSSBO;

		std::vector<std::vector<glm::mat4>> m_Transforms;

	public:
		AnimationPBR(Shader* shader, Camera* camera);

		void Start() override;
		void Render(std::vector<RendererCommand*>& commandList) override;
	};

}


