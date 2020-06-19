#pragma once

#include "Technique.h"

#include "Renderer/LightData.h"

namespace sixengine {

	class QuestionmarkTechnique : public Technique
	{
	private:
		StorageBuffer m_Models;
		StorageBuffer m_Layers;

	public:
		QuestionmarkTechnique(Shader* shader);

		void Start(TextureArray* textureArray) override;

		void StartFrame(std::vector<RendererCommand*>& commandList, std::vector<DrawElementsCommand> drawCommands,
			std::vector<glm::mat4>& models, std::vector<glm::vec4> layers) override;
		void Render(std::vector<RendererCommand*>& commandList) override;
		void SetLight(Light& light) override;
		void FinishFrame() override;
	};

}