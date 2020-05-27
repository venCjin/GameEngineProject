#pragma once

#include "Technique.h"

#include "Renderer/LightData.h"

namespace sixengine {

	class StaticPBR : public Technique
	{
	private:
		StorageBuffer m_Models;
		StorageBuffer m_Layers;
		UniformBuffer m_Lights;

		LightData m_LightData;
		glm::vec3 m_DirectionalLightPos;

	public:
		StaticPBR(Shader* shader);

		void Start(TextureArray* textureArray) override;
		void Render(std::vector<RendererCommand*>& commandList, std::vector<glm::mat4>& models, std::vector<glm::vec4> layers) override;
	};

}