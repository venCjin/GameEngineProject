#pragma once

#include <Gameplay/GameObject.h>
#include <Renderer/TextureArray.h>
#include <Renderer/ModelManager.h>
#include <Core/Camera.h>

#include <ECS/ComponentManager.h>

namespace sixengine {

	struct SSBO
	{
		glm::mat4 model;
		glm::vec4 textureLayer;
	};

	struct BonesStruct
	{
		glm::mat4 bones[52];
	};

	struct RendererCommand
	{
		//float distance;
		class Shader* shader;
		class Model* model;
		//bool isTranslucent;

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

		std::vector<RendererCommand*> m_CommandList;
		std::vector<DrawElementsCommand> m_RenderCommandList;

		ModelManager* m_ModelManager;
		TextureArray* m_TextureArray;
		Camera* m_PlayerCamera;

		unsigned int ssboModels;
		unsigned int ssboLayers;
		unsigned int ssboBones;

		unsigned int idbo;

		std::vector<std::vector<glm::mat4>> m_Transforms;

	public:
		void SubmitCommand(GameObject* gameObject, glm::mat4 model); //how to add gizmo?
		void Render();

		static BatchRenderer* Instance() { return m_BatchRendererInstance; }
		static void Initialize(ModelManager* modelManager, TextureArray* textureArray, Camera* camera);

	private:
		BatchRenderer(ModelManager* modelManager, TextureArray* textureArray, Camera* camera);
		~BatchRenderer();

		float Distance(glm::vec3* x, glm::vec3* y);
	};

}
