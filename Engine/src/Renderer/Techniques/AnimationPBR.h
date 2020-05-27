#pragma once

#include "Technique.h"

namespace sixengine {

	struct BonesStruct
	{
		glm::mat4 bones[100];
	};

	class AnimationPBR : public Technique
	{
	private:
		UniformBuffer m_Models;
		UniformBuffer m_Layers;
		StorageBuffer m_Bones;
		UniformBuffer m_Lights;

		std::vector<std::vector<glm::mat4>> m_Transforms;

	public:
		AnimationPBR(Shader* shader);

		void Start(TextureArray* textureArray) override;
		void Render(std::vector<RendererCommand*>& commandList, std::vector<glm::mat4>& models, std::vector<glm::vec4> layers) override;
		void SetLight(Light& light) override;
	};

}