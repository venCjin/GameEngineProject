#pragma once

#include <Renderer/TextureArray.h>
#include <Renderer/ModelManager.h>
#include <Renderer/Techniques/SkyboxRender.h>
#include <Renderer/Techniques/DepthRender.h>
#include <Renderer/Techniques/ParticleRender.h>
#include <Renderer/Techniques/Water.h>
#include <Renderer/Techniques/Technique.h>
#include <Renderer/BufferLockManager.h>
#include <Renderer/LightData.h>
#include <Renderer/ShadowFrustum.h>
#include <Renderer/RenderTarget.h>

#include <Gameplay/GameObject.h>
#include <Gameplay/Components/ParticleEmitter.h>

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

	struct Light
	{
		LightData m_LightData;
		glm::vec3 m_DirectionalLightPos;

		DepthFramebuffer m_DepthFramebuffer;
		ShadowFrustum m_Frustum;

		glm::mat4 m_LightView;
		glm::mat4 m_Ortho;

		GameObject* m_Center;

		Light(GameObject* center, int size = 4096)
			: m_DepthFramebuffer(size), m_Frustum(&m_LightView, center), m_Center(center)
		{
			m_DirectionalLightPos = glm::vec3(-8.0f, 12.0f, -4.0f);

			m_LightData.ao = 0.4f;
			m_LightData.metallic = 0.4f;
			m_LightData.roughness = 0.6f;

			m_LightData.dirLight.position = glm::vec4(Camera::ActiveCamera->GetViewMatrix() * glm::vec4(m_DirectionalLightPos, 1.0f));
			m_LightData.dirLight.direction = glm::vec4(Camera::ActiveCamera->GetViewMatrix() * glm::vec4(-m_DirectionalLightPos, 0.0f));
			m_LightData.dirLight.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

			m_LightView = glm::lookAt(m_DirectionalLightPos,
				glm::vec3(0.0f, 0.0f, 0.0f),
				glm::vec3(0.0f, 1.0f, 0.0f));
		}

		void Update()
		{
			m_Frustum.Update();
			UpdateOrthoProjection(m_Frustum.GetWidth(), m_Frustum.GetHeight(), m_Frustum.GetLength());
			UpdateLightView(-m_DirectionalLightPos, m_Frustum.GetCenter());
		}

		void UpdateLightView(glm::vec3 direction, glm::vec3 center)
		{
			direction = glm::normalize(direction);
			//center = -Camera::ActiveCamera->m_Transform->GetWorldPosition(); 
			//std::cout << center.x << " " << center.y << " " << center.z << "\n";
			center = -center;

			m_LightView = glm::mat4(1.0f);

			float pitch = glm::acos(glm::length(glm::vec2(direction.x, direction.z)));
			m_LightView = glm::rotate(m_LightView, pitch, glm::vec3(1, 0, 0));

			float yaw = glm::degrees((glm::atan(direction.x / direction.z)));
			yaw = direction.z > 0 ? yaw - 180 : yaw;
			m_LightView = glm::rotate(m_LightView, yaw, glm::vec3(0, 1, 0));

			m_LightView = glm::translate(m_LightView, center);
		}

		void UpdateOrthoProjection(float width, float height, float length)
		{
			m_Ortho = glm::mat4(1.0f);
			m_Ortho[0][0] = 2.0f / width;
			m_Ortho[1][1] = 2.0f / height;
			m_Ortho[2][2] = -2.0f / length;
			m_Ortho[3][3] = 1.0f;
		}
	};

	class BatchRenderer
	{
	private:
		FrameAllocator<RendererCommand> m_FrameAllocator;
		static BatchRenderer* m_BatchRendererInstance;

		SkyboxRender* m_Skybox;

		DepthRender* m_DepthStatic;
		DepthRender* m_DepthAnimated;

		ParticleRender* m_ParticleRender;
		Water* m_Water;

		std::vector<Technique*> m_TechniqueList;

		std::vector<GameObject*> m_ParticleList;

		std::vector<RendererCommand*> m_CommandList;
		std::vector<DrawElementsCommand> m_RenderCommandList;

		ModelManager* m_ModelManager;
		TextureArray* m_TextureArray;

		ShaderBuffer m_IDBO;
		unsigned int m_Offset;

		glm::vec4 m_FrustumPlanes[6];

		Light* m_DirectionalLight;

		RenderTarget m_Default;
		RenderTarget m_PostProcess;

		Shader* m_BlurShader;
		bool m_Blur = false;
		bool m_Shake = false;

		unsigned int m_QuadVAO;

	public:
		void NormalizePlane(glm::vec4& plane);
		void CalculateFrustum();
		bool FrustumAABB(glm::vec3 min, glm::vec3 max);

		void SubmitCommand(GameObject* gameObject, glm::mat4 model);
		void Render();
		void RenderDepth(Technique* depth, Technique* technique);
		void RenderSkybox();
		void RenderWater(Technique* technique1, Technique* technique2);
		void RenderQuad();

		void SetBlurShader(Shader* blur);
		void SetBlur(bool blur);
		void SetShake(bool shake);
		void ApplyBlur();

		void SetParticle(ParticleRender* technique);
		void SetSkybox(SkyboxRender* technique);
		void SetStaticDepth(DepthRender* technique);
		void SetAnimatedDepth(DepthRender* technique);
		void SetWater(Water* technique);
		void SetLight(Light* light);
		void AddTechnique(Technique* technique);
		void Configure();

		static BatchRenderer* Instance() { return m_BatchRendererInstance; }
		static void Initialize(ModelManager* modelManager, TextureArray* textureArray);

	private:
		BatchRenderer(ModelManager* modelManager, TextureArray* textureArray);
		~BatchRenderer();
		float Distance(glm::vec3* x, glm::vec3* y);

		friend class Scene;
	};

}
