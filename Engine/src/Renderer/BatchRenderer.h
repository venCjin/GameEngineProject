#pragma once

#include <Gameplay/GameObject.h>
#include <Renderer/TextureArray.h>
#include <Renderer/ModelManager.h>
#include <Renderer/Technique.h>
#include <Renderer/BufferLockManager.h>


#include <ECS/ComponentManager.h>

namespace sixengine {

	struct SSBO
	{
		glm::mat4 model;
		glm::vec4 textureLayer;
	};

	struct RendererCommand
	{
		//float distance;
		class Shader* shader;
		class Model* model;
		//bool isTranslucent;
		GameObject* gameObject;

		SSBO data;
	};

	struct DrawElementsCommand
	{
		unsigned int vertexCount;
		unsigned int instanceCount;
		unsigned int firstIndex;
		unsigned int baseVertex;
		unsigned int baseInstance;
	};

	class BatchRenderer
	{
	private:
		FrameAllocator<RendererCommand> m_FrameAllocator;
		static BatchRenderer* m_BatchRendererInstance;

		std::vector<Technique*> m_TechniqueList;

		std::vector<RendererCommand*> m_CommandList;
		std::vector<DrawElementsCommand> m_RenderCommandList;

		ModelManager* m_ModelManager;
		TextureArray* m_TextureArray;

		BufferStorage m_IDBO;
		BufferLockManager m_LockManager;

	public:
		void SubmitCommand(GameObject* gameObject, glm::mat4 model);
		void Render();

		void AddTechnique(Technique* technique);
		void Configure();

		static BatchRenderer* Instance() { return m_BatchRendererInstance; }
		static void Initialize(ModelManager* modelManager, TextureArray* textureArray);

	private:
		BatchRenderer(ModelManager* modelManager, TextureArray* textureArray);
		~BatchRenderer();
		float Distance(glm::vec3* x, glm::vec3* y);
	};

}
