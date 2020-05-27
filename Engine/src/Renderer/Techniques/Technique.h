#pragma once

#include "Renderer/Shader.h"
#include "Renderer/BufferLockManager.h"
#include "Core/CameraSystem/Camera.h"

namespace sixengine {

	class RendererCommand;
	class TextureArray;
	struct Light;

	struct ShaderBuffer
	{
		BufferLockManager m_LockManager;

		size_t m_Head;
		size_t m_Size;

		unsigned int m_ID;
		unsigned int m_Binding;
		void* m_Ptr;

		unsigned int m_Buffering;

		ShaderBuffer(size_t size, unsigned int binding, unsigned int buffering)
			: m_Head(0), m_Binding(binding), m_Buffering(buffering), m_LockManager(true)
		{
		}

		ShaderBuffer(size_t size, unsigned int buffering = 3)
			: m_Size(size), m_Head(0), m_Binding(0), m_Buffering(buffering), m_LockManager(true)
		{
		}
	};

	struct UniformBuffer : public ShaderBuffer
	{
		UniformBuffer(size_t size, unsigned int binding, unsigned int buffering = 3)
			: ShaderBuffer(size, binding, buffering)
		{
			int uniformBufferAlign = 0;
			glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &uniformBufferAlign);

			int maxBufferSize = 0;
			glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &maxBufferSize);
				
			int maxBufferBindings = 0;
			glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &maxBufferBindings);

			if (size % uniformBufferAlign != 0)
				size = size / uniformBufferAlign * uniformBufferAlign + uniformBufferAlign;

			m_Size = size;

			if ((m_Size * m_Buffering) > maxBufferSize)
				LOG_CORE_ERROR("UNIFORM BUFFER SIZE EXCEED");

			GLbitfield mapFlags = GL_MAP_WRITE_BIT
				| GL_MAP_PERSISTENT_BIT
				| GL_MAP_COHERENT_BIT;
			GLbitfield createFlags = mapFlags | GL_DYNAMIC_STORAGE_BIT;

			glGenBuffers(1, &m_ID);
			glBindBuffer(GL_UNIFORM_BUFFER, m_ID);
			glBufferStorage(GL_UNIFORM_BUFFER, m_Buffering * m_Size, 0, createFlags);
			m_Ptr = glMapBufferRange(GL_UNIFORM_BUFFER, 0, m_Buffering * m_Size, mapFlags);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}

		void Update(void* address, size_t size)
		{
			m_LockManager.WaitForLockedRange(m_Head, m_Size);
			glBindBufferRange(GL_SHADER_STORAGE_BUFFER, m_Binding, m_ID, m_Head, m_Size);
			void* ptr = (unsigned char*)m_Ptr + m_Head;
			memcpy(ptr, address, size);

			m_LockManager.LockRange(m_Head, m_Size);
			m_Head = (m_Head + m_Size) % (m_Buffering * m_Size);
		}
	};

	struct StorageBuffer : public ShaderBuffer
	{
		StorageBuffer(size_t size, unsigned int binding, unsigned int buffering = 3)
			: ShaderBuffer(size, binding, buffering)
		{
			m_Size = size;

			GLbitfield mapFlags = GL_MAP_WRITE_BIT
				| GL_MAP_PERSISTENT_BIT
				| GL_MAP_COHERENT_BIT;
			GLbitfield createFlags = mapFlags | GL_DYNAMIC_STORAGE_BIT;

			glGenBuffers(1, &m_ID);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ID);
			glBufferStorage(GL_SHADER_STORAGE_BUFFER, m_Buffering * m_Size, 0, createFlags);
			m_Ptr = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, m_Buffering * m_Size, mapFlags);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		}

		void Update(void* address, size_t size)
		{
			m_LockManager.WaitForLockedRange(m_Head, m_Size);
			glBindBufferRange(GL_SHADER_STORAGE_BUFFER, m_Binding, m_ID, m_Head, m_Size);
			void* ptr = (unsigned char*)m_Ptr + m_Head;
			memcpy(ptr, address, size);

			m_LockManager.LockRange(m_Head, m_Size);
			m_Head = (m_Head + m_Size) % (m_Buffering * m_Size);
		}
	};

	class Technique
	{
	protected:
		Shader* m_Shader;

	public:
		Technique(Shader* shader);
		virtual ~Technique();

		virtual void Start(TextureArray* textureArray) = 0;
		virtual void Render(std::vector<RendererCommand*>& commandList, std::vector<glm::mat4>& models, std::vector<glm::vec4> layers) = 0;
		virtual void SetLight(Light& light);

		inline Shader* GetShader() const { return m_Shader; }
	};

}


