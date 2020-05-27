#include "pch.h"
#include "AnimationPBR.h"

#include "Renderer/TextureArray.h"
#include "Renderer/BatchRenderer.h"

#include "Gameplay/Components/Animation.h"
#include <Gameplay/Components/Mesh.h>

#include <future>

namespace sixengine {

	AnimationPBR::AnimationPBR(Shader* shader)
		: Technique(shader), m_Models(102 * sizeof(glm::mat4), 3), m_Layers(100 * sizeof(glm::vec4), 4),
		m_Bones(100 * sizeof(BonesStruct), 5)
	{
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

		std::string currentAnimationName = command->gameObject->GetComponent<Animation>().Get()->currentAnimationName;
		std::string previousAnimationName = command->gameObject->GetComponent<Animation>().Get()->previousAnimationName;

		float currentTimer = command->gameObject->GetComponent<Animation>().Get()->currentAnimationTimer;
		float previousTimer = command->gameObject->GetComponent<Animation>().Get()->previousAnimationTimer;

		command->gameObject->GetComponent<Mesh>()->GetModel()->BoneTransform(currentTimer, previousTimer, *transform, currentAnimationName, previousAnimationName);
	}

	void AnimationPBR::Render(std::vector<RendererCommand*>& commandList, std::vector<glm::mat4>& models, std::vector<glm::vec4> layers)
	{
		m_Shader->Bind();

		m_Models.Update(models.data(), models.size() * sizeof(models[0]));
		m_Layers.Update(layers.data(), layers.size() * sizeof(layers[0]));

		m_Bones.m_LockManager.WaitForLockedRange(m_Bones.m_Head, m_Bones.m_Size);
		glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 5, m_Bones.m_ID, m_Bones.m_Head, m_Bones.m_Size);

		std::vector<std::future<void>> threads;
		threads.reserve(commandList.size());

		for (int i = 0; i < commandList.size(); i++)
			threads.emplace_back(std::async(TransformBones, commandList[i], &m_Transforms[i]));

		for (int i = 0; i < threads.size(); i++)
			threads[i].wait();

		void* ptr = (unsigned char*)m_Bones.m_Ptr + m_Bones.m_Head;
		unsigned int offsetStep = 100 * sizeof(glm::mat4);
		for (int i = 0; i < commandList.size(); i++)
		{
			memcpy(ptr, m_Transforms[i].data(), m_Transforms[i].size() * sizeof(m_Transforms[i][0]));
			ptr = (unsigned char*)ptr + offsetStep;
		}

		m_Bones.m_LockManager.LockRange(m_Bones.m_Head, m_Bones.m_Size);
		m_Bones.m_Head = (m_Bones.m_Head + m_Bones.m_Size) % (m_Bones.m_Buffering * m_Bones.m_Size);
	}

}