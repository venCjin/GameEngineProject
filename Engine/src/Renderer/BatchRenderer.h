#pragma once

#include <Renderer/TextureArray.h>
#include <Renderer/ModelManager.h>
#include <Renderer/Techniques/SkyboxRender.h>
#include <Renderer/Techniques/DepthRender.h>
#include <Renderer/Techniques/Technique.h>
#include <Renderer/BufferLockManager.h>

#include <Gameplay/GameObject.h>

#include <ECS/ComponentManager.h>

#include <Renderer/LightData.h>

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

	struct Light
	{
		LightData m_LightData;
		glm::vec3 m_DirectionalLightPos;

		DepthFramebuffer m_DepthFramebuffer;
		glm::mat4 ortho;

		Light(int width = 4096, int height = 4096)
			: m_DepthFramebuffer(width, height)
		{

			ortho = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, 1.0f, 50.0f);
			m_DirectionalLightPos = glm::vec3(-8.0f, 24.0f, -8.0f);

			m_LightData.ao = 0.4f;
			m_LightData.metallic = 0.4f;
			m_LightData.roughness = 0.6f;

			m_LightData.dirLight.position = glm::vec4(Camera::ActiveCamera->GetViewMatrix() * glm::vec4(m_DirectionalLightPos, 1.0f));
			m_LightData.dirLight.direction = glm::vec4(Camera::ActiveCamera->GetViewMatrix() * glm::vec4(-m_DirectionalLightPos, 0.0f));
			m_LightData.dirLight.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		}
	};

	class BatchRenderer
	{
	private:
		FrameAllocator<RendererCommand> m_FrameAllocator;
		static BatchRenderer* m_BatchRendererInstance;

		SkyboxRender* m_Skybox;
		DepthRender* m_Depth;
		std::vector<Technique*> m_TechniqueList;

		std::vector<RendererCommand*> m_CommandList;
		std::vector<DrawElementsCommand> m_RenderCommandList;

		ModelManager* m_ModelManager;
		TextureArray* m_TextureArray;

		ShaderBuffer m_IDBO;
		glm::vec4 m_FrustumPlanes[6];

		Light* m_DirectionalLight;


	public:
		void NormalizePlane(glm::vec4& plane);
		void CalculateFrustum();
		bool FrustumAABB(glm::vec3 min, glm::vec3 max);

		void SubmitCommand(GameObject* gameObject, glm::mat4 model);
		void Render();
		void RenderDepth(std::vector<RendererCommand*>& commandList);
		void RenderSkybox();

		void SetSkybox(SkyboxRender* technique);
		void SetDepth(DepthRender* technique);
		void SetLight(Light* light);
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
