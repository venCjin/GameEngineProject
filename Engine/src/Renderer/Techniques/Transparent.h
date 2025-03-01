#pragma once
#include "Technique.h"
#include "Renderer/Techniques/AnimationPBR.h"

namespace sixengine{

	class TransparentTechnique : public Technique
	{
	private:
		UniformBuffer m_Models;
		UniformBuffer m_Layers;
		StorageBuffer m_Bones;
		std::vector<std::vector<glm::mat4>> m_Transforms;
	public:
		TransparentTechnique(Shader* shader);

		void Start(TextureArray* textureArray) override;
		void StartFrame(std::vector<RendererCommand*>& commandList, std::vector<DrawElementsCommand> drawCommands,
			std::vector<glm::mat4>& models, std::vector<glm::vec4> layers) override;
		void Render(std::vector<RendererCommand*>& commandList) override;
		void FinishFrame() override;

	};
}


