#include "pch.h"
#include "Technique.h"

#include "Renderer/BatchRenderer.h"
#include "Renderer/TextureArray.h"

#include "Gameplay/Components/Transform.h"

#include <future>

#include "Gameplay/Components/Animation.h"

namespace sixengine {

	Technique::Technique(Shader* shader, Camera* camera)
		: m_Shader(shader), m_Camera(camera)
	{
		m_Shader = shader;
	}

	Technique::~Technique()
	{
		m_Shader = nullptr;
		m_Camera = nullptr;
	}

	StaticPBR::StaticPBR(Shader* shader, Camera* camera)
		: Technique(shader, camera), m_Models(40002 * sizeof(glm::mat4)), m_Layers(40000 * sizeof(glm::vec4)),
		m_Lights(sizeof(LightData)), m_ModelsLockManager(true), m_LayersLockManager(true), m_LightsLockManager(true)
	{

		m_DirectionalLightPos = glm::vec3(-2.0f, 4.0f, -1.0f);

		m_LightData.ao = 0.4f;
		m_LightData.roughness = 0.6f;
		m_LightData.metallic = 0.4f;

		m_LightData.dirLight.position = glm::vec4(m_Camera->GetViewMatrix() * glm::vec4(m_DirectionalLightPos, 1.0f));
		m_LightData.dirLight.direction = glm::vec4(m_Camera->GetViewMatrix()* glm::vec4(-m_DirectionalLightPos, 0.0f));
		m_LightData.dirLight.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	void StaticPBR::Start(TextureArray* textureArray)
	{
		m_Shader->Bind();

		textureArray->Bind(0);
		m_Shader->SetInt("textureArray", 0);

		m_Shader->Unbind();

		GLbitfield mapFlags = GL_MAP_WRITE_BIT
			| GL_MAP_PERSISTENT_BIT
			| GL_MAP_COHERENT_BIT;
		GLbitfield createFlags = mapFlags | GL_DYNAMIC_STORAGE_BIT;

		glGenBuffers(1, &m_Models.m_ID);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_Models.m_ID);
		glBufferStorage(GL_SHADER_STORAGE_BUFFER, m_Models.m_Buffering * m_Models.m_Size, 0, createFlags);
		m_Models.m_Ptr = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, m_Models.m_Buffering * m_Models.m_Size, mapFlags);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		glGenBuffers(1, &m_Layers.m_ID);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_Layers.m_ID);
		glBufferStorage(GL_SHADER_STORAGE_BUFFER, m_Layers.m_Buffering * m_Layers.m_Size, 0, createFlags);
		m_Layers.m_Ptr = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, m_Layers.m_Buffering * m_Layers.m_Size, mapFlags);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		glGenBuffers(1, &m_Lights.m_ID);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_Lights.m_ID);
		glBufferStorage(GL_SHADER_STORAGE_BUFFER, m_Lights.m_Buffering * m_Lights.m_Size, 0, createFlags);
		m_Lights.m_Ptr = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, m_Lights.m_Buffering * m_Lights.m_Size, mapFlags);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	}

	void StaticPBR::Render(std::vector<RendererCommand*>& commandList, std::vector<glm::mat4>& models, std::vector<glm::vec4> layers)
	{
		m_Shader->Bind();

		// update position and direction of light - move it to the main loop or somewhere else
		m_LightData.dirLight.position = glm::vec4(m_Camera->GetViewMatrix() * glm::vec4(m_DirectionalLightPos, 1.0f));
		m_LightData.dirLight.direction = glm::vec4(m_Camera->GetViewMatrix()* glm::vec4(-m_DirectionalLightPos, 0.0f));

		m_ModelsLockManager.WaitForLockedRange(m_Models.m_Head, m_Models.m_Size);
		glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, m_Models.m_ID, m_Models.m_Head, m_Models.m_Size);
		void* ptr = (unsigned char*)m_Models.m_Ptr + m_Models.m_Head;
		memcpy(ptr, models.data(), models.size() * sizeof(models[0]));

		m_ModelsLockManager.LockRange(m_Models.m_Head, m_Models.m_Size);
		m_Models.m_Head = (m_Models.m_Head + m_Models.m_Size) % (m_Models.m_Buffering * m_Models.m_Size);

		m_LayersLockManager.WaitForLockedRange(m_Layers.m_Head, m_Layers.m_Size);
		glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, m_Layers.m_ID, m_Layers.m_Head, m_Layers.m_Size);
		ptr = (unsigned char*)m_Layers.m_Ptr + m_Layers.m_Head;
		memcpy(ptr, layers.data(), layers.size() * sizeof(layers[0]));

		m_LayersLockManager.LockRange(m_Layers.m_Head, m_Layers.m_Size);
		m_Layers.m_Head = (m_Layers.m_Head + m_Layers.m_Size) % (m_Layers.m_Buffering * m_Layers.m_Size);

		m_LightsLockManager.WaitForLockedRange(m_Lights.m_Head, m_Lights.m_Size);
		glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 5, m_Lights.m_ID, m_Lights.m_Head, m_Lights.m_Size);
		ptr = (unsigned char*)m_Lights.m_Ptr + m_Lights.m_Head;
		memcpy(ptr, &m_LightData, sizeof(m_LightData));


		m_LightsLockManager.LockRange(m_Lights.m_Head, m_Lights.m_Size);
		m_Lights.m_Head = (m_Lights.m_Head + m_Lights.m_Size) % (m_Lights.m_Buffering * m_Lights.m_Size);
	}

	AnimationPBR::AnimationPBR(Shader* shader, Camera* camera)
		: Technique(shader, camera), m_Models(102 * sizeof(glm::mat4)), m_Layers(100 * sizeof(glm::vec4)),
		m_Bones(100 * sizeof(BonesStruct)),
		m_ModelsLockManager(true), m_LayersLockManager(true), m_BonesLockManager(true)
	{
		GLbitfield mapFlags = GL_MAP_WRITE_BIT
			| GL_MAP_PERSISTENT_BIT
			| GL_MAP_COHERENT_BIT;
		GLbitfield createFlags = mapFlags | GL_DYNAMIC_STORAGE_BIT;

		glGenBuffers(1, &m_Models.m_ID);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_Models.m_ID);
		glBufferStorage(GL_SHADER_STORAGE_BUFFER, m_Models.m_Buffering * m_Models.m_Size, 0, createFlags);
		m_Models.m_Ptr = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, m_Models.m_Buffering * m_Models.m_Size, mapFlags);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		glGenBuffers(1, &m_Layers.m_ID);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_Layers.m_ID);
		glBufferStorage(GL_SHADER_STORAGE_BUFFER, m_Layers.m_Buffering * m_Layers.m_Size, 0, createFlags);
		m_Layers.m_Ptr = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, m_Layers.m_Buffering * m_Layers.m_Size, mapFlags);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		glGenBuffers(1, &m_Bones.m_ID);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_Bones.m_ID);
		glBufferStorage(GL_SHADER_STORAGE_BUFFER, m_Bones.m_Buffering * m_Bones.m_Size, 0, createFlags);
		m_Bones.m_Ptr = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, m_Bones.m_Buffering * m_Bones.m_Size, mapFlags);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		for (int i = 0; i < 100; i++)
		{
			m_Transforms.push_back(std::vector<glm::mat4>());
		}
	}

	void AnimationPBR::Start(TextureArray* textureArray)
	{
		m_Shader->Bind();

		textureArray->Bind(0);
		m_Shader->SetInt("textureArray", 0);

		m_Shader->Unbind();
	}

	void TransformBones(RendererCommand* command, std::vector<glm::mat4>* transform)
	{
		float time = Timer::Instance()->ElapsedTime();

		std::string animationName = command->gameObject->GetComponent<Animation>().Get()->name;
		command->model->BoneTransform(time, *transform, animationName);
	}

	void AnimationPBR::Render(std::vector<RendererCommand*>& commandList, std::vector<glm::mat4>& models, std::vector<glm::vec4> layers)
	{
		m_Shader->Bind();

		m_ModelsLockManager.WaitForLockedRange(m_Models.m_Head, m_Models.m_Size);
		glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 2, m_Models.m_ID, m_Models.m_Head, m_Models.m_Size);
		void* ptr = (unsigned char*)m_Models.m_Ptr + m_Models.m_Head;
		memcpy(ptr, models.data(), models.size() * sizeof(models[0]));

		m_ModelsLockManager.LockRange(m_Models.m_Head, m_Models.m_Size);
		m_Models.m_Head = (m_Models.m_Head + m_Models.m_Size) % (m_Models.m_Buffering * m_Models.m_Size);

		m_LayersLockManager.WaitForLockedRange(m_Layers.m_Head, m_Layers.m_Size);
		glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 3, m_Layers.m_ID, m_Layers.m_Head, m_Layers.m_Size);
		ptr = (unsigned char*)m_Layers.m_Ptr + m_Layers.m_Head;
		memcpy(ptr, layers.data(), layers.size() * sizeof(layers[0]));

		m_LayersLockManager.LockRange(m_Layers.m_Head, m_Layers.m_Size);
		m_Layers.m_Head = (m_Layers.m_Head + m_Layers.m_Size) % (m_Layers.m_Buffering * m_Layers.m_Size);

		m_BonesLockManager.WaitForLockedRange(m_Bones.m_Head, m_Bones.m_Size);
		glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 4, m_Bones.m_ID, m_Bones.m_Head, m_Bones.m_Size);

		std::vector<std::future<void>> threads;
		threads.reserve(commandList.size());

		for (int i = 0; i < commandList.size(); i++)
			threads.emplace_back(std::async(TransformBones, commandList[i], &m_Transforms[i]));

		for (int i = 0; i < threads.size(); i++)
			threads[i].wait();

		ptr = (unsigned char*)m_Bones.m_Ptr + m_Bones.m_Head;
		unsigned int offsetStep = 100 * sizeof(glm::mat4);
		for (int i = 0; i < commandList.size(); i++)
		{
			memcpy(ptr, m_Transforms[i].data(), m_Transforms[i].size() * sizeof(m_Transforms[i][0]));
			ptr = (unsigned char*)ptr + offsetStep;
		}

		m_BonesLockManager.LockRange(m_Bones.m_Head, m_Bones.m_Size);
		m_Bones.m_Head = (m_Bones.m_Head + m_Bones.m_Size) % (m_Bones.m_Buffering * m_Bones.m_Size);
	}

	UI::UI(Shader* shader, Camera* camera)
		: Technique(shader, camera), m_Models(2 * sizeof(glm::mat4) + sizeof(glm::vec4))
	{
	}

	void UI::AddFont(Font* font)
	{
		//m_Fonts.emplace(font->GetName(), font);
		m_Font = font;
	}

	void UI::RenderText(Text* textR, glm::vec2 position)
	{
		std::vector<Vertex2D> vertices;

		float scale = textR->size / 2.f;

		std::string text = textR->text;
		
		float lineLength = 0;
		/*for (int i = 0; i < text.size(); i++)
		{
			Font::Character ch = m_Font->GetChar(text[i]);
			lineLength += ch.Advance * scale;
		}
		lineLength /= 2;*/

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
		m_Shader->SetVec3("color", textR->color);

		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex2D), vertices.data());

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glDrawArrays(GL_TRIANGLES, 0, vertices.size());

		glBindVertexArray(0);

		glDisable(GL_BLEND);
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
		m_Font->Bind(0);
		m_Shader->SetInt("fontTexture", 0);
		m_Shader->SetMat4("projection", m_Camera->GetProjectionMatrix());
		m_Shader->SetMat4("view", m_Camera->GetViewMatrix());

		GLbitfield mapFlags = GL_MAP_WRITE_BIT
			| GL_MAP_PERSISTENT_BIT
			| GL_MAP_COHERENT_BIT;
		GLbitfield createFlags = mapFlags | GL_DYNAMIC_STORAGE_BIT;

		glGenBuffers(1, &m_Models.m_ID);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_Models.m_ID);
		glBufferStorage(GL_SHADER_STORAGE_BUFFER, m_Models.m_Buffering * m_Models.m_Size, 0, createFlags);
		m_Models.m_Ptr = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, m_Models.m_Buffering * m_Models.m_Size, mapFlags);
	}

	void UI::Render(std::vector<RendererCommand*>& commandList, std::vector<glm::mat4>& models, std::vector<glm::vec4> layers)
	{
		m_Shader->Bind();

		//glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 5, m_Models.m_ID, m_Models.m_Head, m_Models.m_Size);
		//void* ptr = (unsigned char*)m_Models.m_Ptr + m_Models.m_Head;
		//memcpy(ptr, models.data(), 2 * sizeof(glm::mat4));

		for (auto i : commandList)
		{
			//if (i->gameObject->HasComponent<Image>())

			if (i->gameObject->HasComponent<Text>())
			{
				auto text = i->gameObject->GetComponent<Text>();
				auto t = i->gameObject->GetComponent<Transform>();

				//void* ptr = (unsigned char*)m_Models.m_Ptr + m_Models.m_Head;
				//memcpy(ptr, &text->color, sizeof(glm::vec4));

				RenderText(text.Get(), t->getWorldPosition());
			}
		}

		m_Models.m_Head = (m_Models.m_Head + m_Models.m_Size) % (m_Models.m_Buffering * m_Models.m_Size);
	}
}

