#include "pch.h"
#include "Technique.h"

#include "Renderer/BatchRenderer.h"

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

	void StaticPBR::Start()
	{

	}

	void StaticPBR::Render(std::vector<RendererCommand*>& commandList)
	{
		m_Shader->Bind();
		m_Shader->SetMat4("projection", m_Camera->GetProjectionMatrix());
		m_Shader->SetMat4("view", m_Camera->GetViewMatrix());
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

	void AnimationPBR::Start()
	{
	}

	void AnimationPBR::Render(std::vector<RendererCommand*>& commandList)
	{
		m_Shader->Bind();
		m_Shader->SetMat4("projection", m_Camera->GetProjectionMatrix());
		m_Shader->SetMat4("view", m_Camera->GetViewMatrix());

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_BonesSSBO);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_BonesSSBO);

		float time = Timer::Instance()->ElapsedTime();
		unsigned int offsetStep = 100 * sizeof(glm::mat4);
		unsigned int offset = 0;
		for (int i = 0; i < commandList.size(); i++)
		{
			commandList[i]->model->BoneTransform(time + (float)i * 2.0f, m_Transforms[i]);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, m_Transforms[i].size() * sizeof(m_Transforms[i][0]), m_Transforms[i].data());

			offset += offsetStep;
		}

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	

	

}

