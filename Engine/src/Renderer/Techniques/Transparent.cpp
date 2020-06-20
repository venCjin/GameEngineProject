#include "pch.h"
#include "Transparent.h"
#include "Renderer/TextureArray.h"
#include "Renderer/BatchRenderer.h"

#include "Gameplay/Components/Animation.h"

#include <future>

namespace sixengine
{

	void TransformBonesTransparent(RendererCommand* command, std::vector<glm::mat4>* transform)
	{
		float time = Timer::Instance()->ElapsedTime();

		std::string currentAnimationName = command->gameObject->GetComponent<Animation>().Get()->currentAnimationName;
		std::string previousAnimationName = command->gameObject->GetComponent<Animation>().Get()->previousAnimationName;

		float currentTimer = command->gameObject->GetComponent<Animation>().Get()->currentAnimationTimer;
		float previousTimer = command->gameObject->GetComponent<Animation>().Get()->previousAnimationTimer;

		command->gameObject->GetComponent<Mesh>()->GetModel()->BoneTransform(currentTimer, previousTimer, *transform, currentAnimationName, previousAnimationName);
	}

	TransparentTechnique::TransparentTechnique(Shader* shader) 
		: Technique(shader),
		m_Models(12 * sizeof(glm::mat4), 0),
		m_Bones(100 * sizeof(BonesStruct), 1),
		m_Layers(10 * sizeof(glm::vec4), 2)
	{
		for (int i = 0; i < 100; i++)
		{
			m_Transforms.push_back(std::vector<glm::mat4>());
		}
	}

	void TransparentTechnique::Start(TextureArray* textureArray)
	{
		m_Shader->Bind();

		//textureArray->Bind(0);
		m_Shader->SetVec3("color", glm::vec3(1.0f, .6f, .4f));
		m_Shader->SetFloat("FresnelExponent", 1.f);
		m_Shader->Unbind();
	}



	void TransparentTechnique::StartFrame(std::vector<RendererCommand*>& commandList, std::vector<DrawElementsCommand> drawCommands, std::vector<glm::mat4>& models, std::vector<glm::vec4> layers)
	{
		m_DrawCommands = drawCommands;

		m_Offset = 0;
		m_Size = 0;

		m_Models.Update(models.data(), models.size() * sizeof(models[0]));
		m_Layers.Update(layers.data(), layers.size() * sizeof(layers[0]));
		m_Bones.m_LockManager.WaitForLockedRange(m_Bones.m_Head, m_Bones.m_Size);
		glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, m_Bones.m_ID, m_Bones.m_Head, m_Bones.m_Size);

		std::vector<std::future<void>> threads;
		threads.reserve(commandList.size());

		for (int i = 0; i < commandList.size(); i++)
			threads.emplace_back(std::async(TransformBonesTransparent, commandList[i], &m_Transforms[i]));

		for (int i = 0; i < threads.size(); i++)
			threads[i].wait();

		void* ptr = (unsigned char*)m_Bones.m_Ptr + m_Bones.m_Head;
		unsigned int offsetStep = sizeof(BonesStruct);
		for (int i = 0; i < commandList.size(); i++)
		{
			memcpy(ptr, m_Transforms[i].data(), m_Transforms[i].size() * sizeof(m_Transforms[i][0]));
			ptr = (unsigned char*)ptr + offsetStep;
		}

	}

	
	void TransparentTechnique::Render(std::vector<RendererCommand*>& commandList)
	{
		m_Shader->Bind();
		m_Shader->SetVec3("viewDir", Camera::ActiveCamera->m_Transform->GetForward());

		m_Models.Bind();
		m_Bones.Bind();
		m_Layers.Bind();
	}

	void TransparentTechnique::FinishFrame()
	{
		m_Models.LockAndMovePointer();
		m_Bones.LockAndMovePointer();
		m_Layers.LockAndMovePointer();
	}
}