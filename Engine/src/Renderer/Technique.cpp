#include "pch.h"
#include "Technique.h"

#include "Renderer/BatchRenderer.h"
#include "Renderer/TextureArray.h"

#include <future>

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
		: Technique(shader, camera)
	{
	}

	void StaticPBR::Start(TextureArray* textureArray)
	{
		m_Shader->Bind();

		textureArray->Bind(0);
		m_Shader->SetInt("textureArray", 0);

		m_Shader->Unbind();

		glGenBuffers(1, &m_ModelsSSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ModelsSSBO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 10002 * sizeof(glm::mat4), 0, GL_DYNAMIC_COPY);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_ModelsSSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		glGenBuffers(1, &m_LayersSSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_LayersSSBO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 10000 * sizeof(glm::vec4), 0, GL_DYNAMIC_COPY);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_LayersSSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	}

	void StaticPBR::Render(std::vector<RendererCommand*>& commandList, std::vector<glm::mat4>& models, std::vector<glm::vec4> layers)
	{
		m_Shader->Bind();

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ModelsSSBO);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, models.size() * sizeof(models[0]), models.data());

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_LayersSSBO);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, layers.size() * sizeof(layers[0]), layers.data());
	}

	AnimationPBR::AnimationPBR(Shader* shader, Camera* camera)
		: Technique(shader, camera)
	{
		glGenBuffers(1, &m_ModelsSSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ModelsSSBO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 102 * sizeof(glm::mat4), 0, GL_DYNAMIC_COPY);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_ModelsSSBO);

		glGenBuffers(1, &m_LayersSSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_LayersSSBO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 100 * sizeof(glm::vec4), 0, GL_DYNAMIC_COPY);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_LayersSSBO);

		glGenBuffers(1, &m_BonesSSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_BonesSSBO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 100 * sizeof(BonesStruct), 0, GL_DYNAMIC_COPY);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, m_BonesSSBO);

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

	void Transform(RendererCommand* command, std::vector<glm::mat4>* transform)
	{
		float time = Timer::Instance()->ElapsedTime();
		command->model->BoneTransform(time, *transform);
	}

	void AnimationPBR::Render(std::vector<RendererCommand*>& commandList, std::vector<glm::mat4>& models, std::vector<glm::vec4> layers)
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ModelsSSBO);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, models.size() * sizeof(models[0]), models.data());

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_LayersSSBO);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, layers.size() * sizeof(layers[0]), layers.data());


		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_BonesSSBO);

		std::vector<std::future<void>> threads;
		threads.reserve(commandList.size());

		for (int i = 0; i < commandList.size(); i++)
			threads.emplace_back(std::async(Transform, commandList[i], &m_Transforms[i]));

		for (int i = 0; i < threads.size(); i++)
			threads[i].wait();

		unsigned int offsetStep = sizeof(BonesStruct);
		unsigned int offset = 0;
		for (int i = 0; i < commandList.size(); i++)
		{
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, m_Transforms[i].size() * sizeof(m_Transforms[i][0]), m_Transforms[i].data());
			offset += offsetStep;
		}

		m_Shader->Bind();
	}

}

