#pragma once
#include "Technique.h"

namespace sixengine{
	class TransparentTechnique : public Technique
	{
	private:
		StorageBuffer m_Models;
		StorageBuffer m_Layers;
	public:
		TransparentTechnique(Shader* shader);
		void SetOnSurface(bool value);
		void Start(TextureArray* textureArray) override;
		void Render(std::vector<RendererCommand*>& commandList, std::vector<glm::mat4>& models, std::vector<glm::vec4> layers) override;

	};
}


