#pragma once

#include <Renderer/Techniques/Technique.h>
#include <Gameplay/Components/ParticleEmitter.h>

namespace sixengine {

	class ParticleRender : public Technique
	{
	private:
		StorageBuffer m_Models;
		StorageBuffer m_ParticleData;

		unsigned int m_VAO;
		unsigned int m_VBO;

	public:
		ParticleRender(Shader* shader);

		void Start(TextureArray* textureArray) override;
		void StartFrame(std::vector<RendererCommand*>& commandList, std::vector<DrawElementsCommand> drawCommands,
			std::vector<glm::mat4>& models, std::vector<glm::vec4> layers) override;
		void Render(std::vector<RendererCommand*>& commandList) override;
		void Render(std::vector<ParticleEmitter*> particleEmitter);
	};

}


