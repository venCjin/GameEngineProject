#pragma once

#include "Technique.h"

#include "Renderer/Font.h";
#include "Gameplay/Components/Text.h"

namespace sixengine {

	class UI : public Technique
	{
	private:
		struct Vertex2D
		{
			glm::vec2 pos;
			glm::vec2 texCoord;
		};

		StorageBuffer m_Models;
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

}



