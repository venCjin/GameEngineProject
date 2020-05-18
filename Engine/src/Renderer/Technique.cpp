#include "pch.h"
#include "Technique.h"

#include "Renderer/BatchRenderer.h"
#include "Renderer/TextureArray.h"

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
		m_ModelsLockManager(true), m_LayersLockManager(true)
	{
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

	}

	void StaticPBR::Render(std::vector<RendererCommand*>& commandList, std::vector<glm::mat4>& models, std::vector<glm::vec4> layers)
	{
		m_Shader->Bind();

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

	void Transform(RendererCommand* command, std::vector<glm::mat4>* transform)
	{
		float time = Timer::Instance()->ElapsedTime();

		std::string animationName = command->gameObject->GetComponent<Animation>().Get()->name;
		command->model->BoneTransform(time, *transform, animationName);
	}

	void AnimationPBR::Render(std::vector<RendererCommand*>& commandList, std::vector<glm::mat4>& models, std::vector<glm::vec4> layers)
	{

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

		m_Shader->Bind();

		m_BonesLockManager.WaitForLockedRange(m_Bones.m_Head, m_Bones.m_Size);
		glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 4, m_Bones.m_ID, m_Bones.m_Head, m_Bones.m_Size);

		std::vector<std::future<void>> threads;
		threads.reserve(commandList.size());

		for (int i = 0; i < commandList.size(); i++)
			threads.emplace_back(std::async(Transform, commandList[i], &m_Transforms[i]));

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
}

