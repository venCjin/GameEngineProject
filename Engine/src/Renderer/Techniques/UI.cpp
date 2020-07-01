#include "pch.h"
#include "UI.h"

#include "Renderer/BatchRenderer.h"

#include <glm/glm.hpp>

namespace sixengine {

	UI::UI(Shader* shader, Shader* imageShader)
		: Technique(shader), m_ImageShader(imageShader)
	{
	}

	void UI::AddFont(Font* font)
	{
		m_Font = font;
	}

	void UI::RenderText(Text* textR, glm::vec2 position)
	{
		std::vector<Vertex2D> vertices;

		float scale = textR->size / 2.f;

		std::string text = textR->text;

		float lineLength = 0;

		glm::vec2 atlasSize(m_Font->GetAtlasWidth(), m_Font->GetAtlasHeight());

		for (int i = 0; i < text.size(); i++)
		{
			Font::Character ch = m_Font->GetChar(text[i]);
			float x = position.x - lineLength + ch.Bearing.x * scale;
			float y = -position.y - ch.Bearing.y * scale;
			float w = ch.Size.x * scale;
			float h = ch.Size.y * scale;

			//Skip glyphs that have no pixels
			if (!w || !h)
			{
				lineLength -= ch.Advance * scale;
				continue;
			}

			vertices.push_back({ { x + w, -y }, { ch.Offset + ch.Size.x / atlasSize.x, 0.0f } }); //prawy g�rny
			vertices.push_back({ { x, -y }, { ch.Offset, 0.0f } }); //lewy g�rny
			vertices.push_back({ { x, -y - h}, { ch.Offset, ch.Size.y / atlasSize.y } }); //lewy dolny

			vertices.push_back({ { x + w, -y }, { ch.Offset + ch.Size.x / atlasSize.x, 0 } }); //prawy g�rny
			vertices.push_back({ { x, -y - h }, { ch.Offset, ch.Size.y / atlasSize.y } }); //lewy dolny
			vertices.push_back({ { x + w, -y - h }, { ch.Offset + ch.Size.x / atlasSize.x, ch.Size.y / atlasSize.y } }); //prawy dolny

			lineLength -= ch.Advance * scale;
		}

		m_Shader->Bind();
		m_Font->Bind(0);
		m_Shader->SetVec3("color", textR->color);

		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex2D), vertices.data());

		glDrawArrays(GL_TRIANGLES, 0, vertices.size());

		glBindVertexArray(0);
	}

	void UI::RenderImage(Image* image, glm::vec2 position)
	{
		std::vector<Vertex2D> vertices;

		glm::vec2 size = image->size;
		float xLeft = position.x - size.x / 2.0f;
		float xRight = position.x + size.x / 2.0f;
		float yDown = position.y - size.y / 2.0f;
		float yUp = position.y + size.y / 2.0f;

		vertices.push_back({ { xRight,	 yUp }, { 1.0f, 1.0f } }); //prawy gorny
		vertices.push_back({ {  xLeft,	 yUp }, { 0.0f, 1.0f } }); //lewy gorny
		vertices.push_back({ {  xLeft, yDown }, { 0.0f, 0.0f } }); //lewy dolny

		vertices.push_back({ { xRight,	 yUp }, { 1.0f, 1.0f } }); //prawy gorny
		vertices.push_back({ {  xLeft, yDown }, { 0.0f, 0.0f } }); //lewy dolny
		vertices.push_back({ { xRight, yDown }, { 1.0f, 0.0f } }); //prawy dolny

		m_ImageShader->Bind();
		image->texture->Bind(0);

		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex2D), vertices.data());

		glDrawArrays(GL_TRIANGLES, 0, vertices.size());

		glBindVertexArray(0);
	}

	void UI::Start(TextureArray* textureArray)
	{
		glGenVertexArrays(1, &m_VAO);
		glGenBuffers(1, &m_VBO);

		glBindVertexArray(m_VAO);

		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, 600 * sizeof(Vertex2D), 0, GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		m_Shader->Bind();
		m_Shader->SetInt("fontTexture", 0);
		m_Shader->SetMat4("projection", glm::ortho(0.f, (float)Application::Get().GetWindow().GetWidth(), 0.f, (float)Application::Get().GetWindow().GetHeight(), 0.01f, 1000.0f));
	
		m_ImageShader->Bind();
		m_ImageShader->SetInt("fontTexture", 0);
		m_ImageShader->SetMat4("projection", glm::ortho(0.f, (float)Application::Get().GetWindow().GetWidth(), 0.f, (float)Application::Get().GetWindow().GetHeight(), 0.01f, 1000.0f));

	}

	void UI::StartFrame(std::vector<RendererCommand*>& commandList, std::vector<DrawElementsCommand> drawCommands, std::vector<glm::mat4>& models, std::vector<glm::vec4> layers)
	{
		m_DrawCommands = drawCommands;

		m_Offset = 0;
		m_Size = 0;
	}

	void UI::Render(std::vector<RendererCommand*>& commandList)
	{
		for (auto i : commandList)
		{
			if (i->gameObject->HasComponent<Image>())
			{
				auto image = i->gameObject->GetComponent<Image>();
				auto t = i->gameObject->GetComponent<Transform>();

				RenderImage(image.Get(), t->getWorldPosition());
			}

			if (i->gameObject->HasComponent<Text>())
			{
				auto text = i->gameObject->GetComponent<Text>();
				auto t = i->gameObject->GetComponent<Transform>();

				RenderText(text.Get(), t->getWorldPosition());
			}
		}
	}
}