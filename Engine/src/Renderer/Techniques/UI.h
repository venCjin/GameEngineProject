#pragma once

#include "Technique.h"

#include "Renderer/Font.h";
#include "Renderer/Texture.h";
#include "Gameplay/Components/Text.h"
#include "Gameplay/Components/Image.h"

namespace sixengine {

	class UI : public Technique
	{
	private:
		struct Vertex2D
		{
			glm::vec2 pos;
			glm::vec2 texCoord;
		};

		//std::map<std::string, Font*> m_Fonts;
		Shader* m_ImageShader;
		Font* m_Font;
		unsigned int m_VAO;
		unsigned int m_VBO;

	public:
		UI(Shader* shader, Shader* imageShader);

		void AddFont(Font* font);
		void RenderText(Text* text, glm::vec2 position);
		void RenderImage(Image* image, glm::vec2 position);

		void Start(TextureArray* textureArray) override;
		void StartFrame(std::vector<RendererCommand*>& commandList, std::vector<DrawElementsCommand> drawCommands,
			std::vector<glm::mat4>& models, std::vector<glm::vec4> layers) override;
		void Render(std::vector<RendererCommand*>& commandList) override;
	};

}



