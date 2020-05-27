#pragma once

#include <Renderer/TextureArray.h>
#include <Renderer/ModelManager.h>
#include <Renderer/Techniques/DepthRender.h>
#include <Renderer/Techniques/Technique.h>
#include <Renderer/BufferLockManager.h>

#include <Gameplay/GameObject.h>

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
		class Shader* shader = nullptr;
		int ModelID = -1;
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

		DepthRender* m_Depth;
		DepthFramebuffer m_DepthFramebuffer;

		std::vector<Technique*> m_TechniqueList;

		std::vector<RendererCommand*> m_CommandList;
		std::vector<DrawElementsCommand> m_RenderCommandList;

		ModelManager* m_ModelManager;
		TextureArray* m_TextureArray;

		ShaderBuffer m_IDBO;
		glm::vec4 m_FrustumPlanes[6];

	public:
		void NormalizePlane(glm::vec4& plane);
		void CalculateFrustum();
		bool FrustumAABB(glm::vec3 min, glm::vec3 max);
		bool FrustumAABB(glm::vec3 center, float size);

		void SubmitCommand(GameObject* gameObject, glm::mat4 model);
		void Render();

		void SetDepth(DepthRender* technique);
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
