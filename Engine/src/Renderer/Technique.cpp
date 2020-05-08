#include "pch.h"
#include "Technique.h"

#include "Renderer/BatchRenderer.h"
#include "Renderer/TextureArray.h"

#include <future>

namespace sixengine {

	Technique::Technique(Shader* shader, Camera* camera)
		: m_Shader(shader), m_Camera(camera)
	{
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
	}

	void StaticPBR::Render(std::vector<RendererCommand*>& commandList)
	{
		m_Shader->Bind(); 
	}

	AnimationPBR::AnimationPBR(Shader* shader, Camera* camera)
		: Technique(shader, camera)
	{
		glGenBuffers(1, &m_BonesSSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_BonesSSBO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 100 * sizeof(BonesStruct), 0, GL_DYNAMIC_COPY);
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

	void Transform(RendererCommand* command, std::vector<glm::mat4>* transform)
	{
		float time = Timer::Instance()->ElapsedTime();
		command->model->BoneTransform(time, *transform);
	}

	void AnimationPBR::Render(std::vector<RendererCommand*>& commandList)
	{		
		m_Shader->Bind();

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_BonesSSBO);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_BonesSSBO);
		
		std::vector<std::future<void>> threads;
		threads.reserve(commandList.size());

		for (int i = 0; i < commandList.size(); i++)
			threads.emplace_back(std::async(Transform, commandList[i], &m_Transforms[i]));

		for (int i = 0; i < threads.size(); i++)
			threads[i].wait();

		unsigned int offsetStep = 100 * sizeof(glm::mat4);
		unsigned int offset = 0;
		for (int i = 0; i < commandList.size(); i++)
		{
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, m_Transforms[i].size() * sizeof(m_Transforms[i][0]), m_Transforms[i].data());
			offset += offsetStep;
		}

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	

	

}

