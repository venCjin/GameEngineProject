#pragma once

#include "Technique.h"
#include "Renderer/Skybox.h"

namespace sixengine {

	class SkyboxRender : public Technique
	{
	private:
		Skybox* m_Skybox;

		unsigned int m_VAO;
		unsigned int m_VBO;

	public:
		SkyboxRender(Shader* shader, Skybox* skybox);

		void Start(TextureArray* textureArray) override;
		void StartFrame(std::vector<RendererCommand*>& commandList, std::vector<DrawElementsCommand> drawCommands,
			std::vector<glm::mat4>& models, std::vector<glm::vec4> layers) override;
		void Render(std::vector<RendererCommand*>& commandList) override;
		void Render();
	};

}


