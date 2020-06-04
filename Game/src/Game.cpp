#include <Engine.h>
#include <EntryPoint.h>

#include "Core/Scene.h"
#include "Renderer/BatchRenderer.h"

// hacks
#include "Gameplay/Systems/AnimationSystem.h"
#include "Renderer/Techniques/AnimationPBR.h"
#include "Renderer/Techniques/DepthRender.h"
#include "Renderer/Techniques/Water.h"
#include "Renderer/Techniques/UI.h"
#include <Renderer/Techniques/Transparent.h>
#include "Gameplay/Components/SimplePlayer.h"
#include <Physics/Components/BoxCollider.h>
#include "Renderer/PrimitiveUtils.h"
#include "Renderer/Gizmo.h"

#include <Core/CameraSystem/FlyingCameraSystem.h>
#include <Core/CameraSystem/OrbitalCameraSystem.h>
#include <Core/CameraSystem/MixingCameraSystem.h>
// hacks end
//#include <Gameplay/Systems/PlayerMaterialManager.h>
#include <Core/AudioManager.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>



namespace sixengine {

	class Game : public Application
	{
	private:
		Scene m_Scene;
		BatchRenderer* m_BatchRenderer;

		GameObject* orbitalCamA;
		GameObject* orbitalCamB;
		GameObject* mixingCam;
		GameObject* flying;
		GameObject* m_Player;
	#ifdef DEBUG
		std::array<glm::vec3, 10> tr;
	#endif //DEBUG

	public:
		Game(std::string title, unsigned int width, unsigned int height)
			: Application(title, width, height), m_Scene(width, height)
		{
			m_BatchRenderer = BatchRenderer::Instance();
#ifdef DEBUG
			tr.fill(glm::vec3(0.0f));
#endif // DEBUG

		}

		~Game()
		{

		}

		virtual void OnInit() override
		{
			m_Scene.LoadScene("res/scenes/exported.scene");
			//ADD_TRACK("res/sounds/ophelia.mp3", "ophelia");
			//PLAY_TRACK("ophelia");
			// HACKS

			m_SystemManager.AddSystem<AnimationSystem>();
			//m_SystemManager.AddSystem<PlayerMaterialManagerSystem>();

			Shader* m_BasicShader2 = m_Scene.m_ShaderManager->AddShader("res/shaders/AnimationPBR.glsl");
			Shader* m_BasicShader = m_Scene.m_ShaderManager->AddShader("res/shaders/PBR.glsl");
			Shader* m_FontShader = m_Scene.m_ShaderManager->AddShader("res/shaders/Font.glsl");
			Shader* m_TransparentShader = m_Scene.m_ShaderManager->AddShader("res/shaders/Transparent.glsl");
			m_Scene.m_ShaderManager->AddShader("res/shaders/Depth.glsl");
			m_Scene.m_ShaderManager->AddShader("res/shaders/DepthAnim.glsl");
			m_Scene.m_ShaderManager->AddShader("res/shaders/Skybox.glsl");

			Skybox* skybox = new Skybox(
				{
					"res/textures/skybox/right.jpg",
					"res/textures/skybox/left.jpg",
					"res/textures/skybox/top.jpg",
					"res/textures/skybox/bottom.jpg",
					"res/textures/skybox/front.jpg",
					"res/textures/skybox/back.jpg"
				}
			);
			m_BatchRenderer->SetSkybox(new SkyboxRender(m_Scene.m_ShaderManager->Get("Skybox"), skybox));

			m_BatchRenderer->SetStaticDepth(new DepthRender(m_Scene.m_ShaderManager->Get("Depth")));
			m_BatchRenderer->SetAnimatedDepth(new DepthRender(m_Scene.m_ShaderManager->Get("DepthAnim")));


			Font* font = new Font("res/fonts/DroidSans.ttf");
			UI* ui = new UI(m_FontShader);
			ui->AddFont(font);

			m_BatchRenderer->AddTechnique(new AnimationPBR(m_BasicShader2));
			m_BatchRenderer->AddTechnique(new TransparentTechnique(m_TransparentShader));
			
			
			
			m_BatchRenderer->AddTechnique(ui);

			m_Scene.m_TextureArray->AddTexture("res/models/par/textures/parasiteZombie_diffuse.png");
			m_Scene.m_TextureArray->AddTexture("res/models/par/textures/parasiteZombie_normal.png");
			m_Scene.m_TextureArray->AddTexture("res/models/par/textures/parasiteZombie_specular.png");
			m_Scene.m_TextureArray->AddTexture("res/textures/test/Bricks.jpg");
			m_Scene.m_TextureArray->CreateTextureArray();
			MaterialManager::getInstance()->CreateMaterial(
				m_Scene.m_ShaderManager->Get("Font"),
				glm::vec4(0),
				"FontMaterial");


			MaterialManager::getInstance()->CreateMaterial(
				m_Scene.m_ShaderManager->Get("PBR"),
				glm::vec4(m_Scene.m_TextureArray->GetTexture("Bricks"), 0, 0, 0),
				"Bricks");

			MaterialManager::getInstance()->CreateMaterial(
				m_Scene.m_ShaderManager->Get("AnimationPBR"),
				glm::vec4(m_Scene.m_TextureArray->GetTexture("parasiteZombie_diffuse"),
					m_Scene.m_TextureArray->GetTexture("parasiteZombie_normal"),
					m_Scene.m_TextureArray->GetTexture("parasiteZombie_specular"), 0.0f),
				"parasiteZombie");

			MaterialManager::getInstance()->CreateMaterial(
				m_Scene.m_ShaderManager->Get("Transparent"),
				glm::vec4(m_Scene.m_TextureArray->GetTexture("Bricks")),
				"Transparent");

			m_Scene.m_MaterialManager->CreateMaterial(
				m_Scene.m_ShaderManager->Get("PBR"),
				glm::vec4(m_Scene.m_TextureArray->GetTexture("Bricks")),
				"PBR");

			//TODO:*************
			Shader* m_WaterShader = m_Scene.m_ShaderManager->AddShader("res/shaders/Water.glsl");
			MaterialManager::getInstance()->CreateMaterial(
				m_Scene.m_ShaderManager->Get("Water"),
				glm::vec4(0),
				"WaterMaterial");
			m_Scene.m_ModelManager->AddModel("res/models/primitives/plane.obj");
			GameObject* w;
			w = new GameObject(m_EntityManager);
			w->AddComponent<Transform>(w);
			w->GetComponent<Transform>()->SetWorldPosition(0.0f, 0.5f, 0.0f);
			w->AddComponent<Mesh>(m_Scene.m_ModelManager->GetModel("plane"));
			w->AddComponent<Material>(*MaterialManager::getInstance()->Get("WaterMaterial"));
			m_Scene.m_SceneRoot->AddChild(w);
			Water* water = new Water(m_WaterShader, w);
			m_BatchRenderer->SetWater(water);
			m_BatchRenderer->AddTechnique(water);
			//TODO:*************

			m_Scene.m_ModelManager->AddModel("res/models/par/par.dae");
			m_Scene.m_ModelManager->AddModel("res/models/primitives/cylinder.obj");
			m_Scene.m_ModelManager->AddModel("res/models/primitives/cube.obj");
			m_Scene.m_ModelManager->CreateVAO();
			m_Scene.m_ModelManager->GetModel("par")->LoadAnimation("res/models/par/par_idle.dae", "idle");
			m_Scene.m_ModelManager->GetModel("par")->LoadAnimation("res/models/par/par_walk.dae", "walk");
			m_Scene.m_ModelManager->GetModel("par")->LoadAnimation("res/models/par/par_punch.dae", "punch");

			GameObject* obj;
			obj = new GameObject(m_EntityManager);
			obj->AddComponent<Transform>(obj);
			obj->GetComponent<Transform>()->SetWorldPosition(-10.0f, 1.0f, -10.0f);
			obj->AddComponent<Mesh>(m_Scene.m_ModelManager->GetModel("cylinder"));
			obj->AddComponent<Material>(*MaterialManager::getInstance()->Get("Transparent"));
			m_Scene.m_SceneRoot->AddChild(obj);
			m_Player = obj;

			obj = new GameObject(m_EntityManager);
			obj->AddComponent<Transform>(obj);
			obj->GetComponent<Transform>()->SetWorldPosition(5.0f, 10.0f, 0.0f);
			obj->AddComponent<Text>("Testowy tekst, do testowania.", glm::vec3(0.0f, 1.0f, 1.0f), 1.0f);
			obj->AddComponent<Material>(*MaterialManager::getInstance()->Get("FontMaterial"));
			m_Scene.m_UIRoot->AddChild(obj);

			obj = new GameObject(m_EntityManager);
			obj->AddComponent<Transform>(obj);
			obj->GetComponent<Transform>()->SetWorldPosition(5.0, 690.0f, 0.0f);
			obj->AddComponent<Text>("Sixengine 0.?", glm::vec3(1.0f, 0.0f, 1.0f), 0.3f);
			obj->AddComponent<Material>(*MaterialManager::getInstance()->Get("FontMaterial"));
			m_Scene.m_UIRoot->AddChild(obj);

			obj = new GameObject(m_EntityManager);
			obj->AddComponent<Transform>(obj);
			obj->GetComponent<Transform>()->SetWorldPosition(0.0f, 0.0f, 0.0f);
			obj->GetComponent<Transform>()->SetLocalScale(0.01f, 0.01f, 0.01f);
			obj->GetComponent<Transform>()->SetLocalOrientation(180.0f, 0.0f, 0.0f);
			//obj->AddComponent<Mesh>(m_Scene.m_ModelManager->GetModel("cylinder"));
			//obj->AddComponent<Material>(*MaterialManager::getInstance()->Get("Transparent"));
			obj->AddComponent<Mesh>(m_Scene.m_ModelManager->GetModel("par"));
			obj->AddComponent<Material>(*MaterialManager::getInstance()->Get("parasiteZombie"));
			obj->AddComponent<Animation>();
			obj->AddComponent<SimplePlayer>();
			obj->AddComponent<BoxCollider>(glm::vec3(1, 2, 1), 0);

			std::vector<GizmoVertex> vertices;
			std::vector<unsigned int> indices;
			PrimitiveUtils::GenerateBox(vertices, indices, 100, 200, 100);
			VertexArray* vao = new VertexArray();
			VertexBuffer* vbo = new VertexBuffer(&vertices[0], vertices.size());
			vbo->SetLayout({
				{ VertexDataType::VEC3F, "Position" }
				});
			IndexBuffer* ibo = new IndexBuffer(&indices[0], indices.size());
			vao->AddVertexBuffer(*vbo);
			vao->AddIndexBuffer(*ibo);
			obj->AddGizmo(new Gizmo(vao, m_Scene.m_ShaderManager->AddShader("res/shaders/Gizmo.glsl"), glm::vec3(0, 255, 0)));

			m_BatchRenderer->SetLight(new Light(obj));

			m_Scene.m_SceneRoot->AddChild(obj);

			flying = Camera::ActiveCamera->m_GameObject;

			m_SystemManager.AddSystem<OrbitalCameraSystem>();
			m_SystemManager.AddSystem<MixingCameraSystem>();
			orbitalCamA = new GameObject(m_EntityManager);
			orbitalCamA->AddComponent<Transform>(orbitalCamA);
			orbitalCamA->AddComponent<Camera>(orbitalCamA);
			orbitalCamA->GetComponent<Camera>()->SetPerspective((float)Application::Get().GetWindow().GetWidth() / (float)Application::Get().GetWindow().GetHeight());

			orbitalCamA->AddComponent<OrbitalCamera>();
			orbitalCamA->GetComponent<OrbitalCamera>()->m_LookTarget = obj->GetComponent<Transform>().Get();
			orbitalCamA->GetComponent<OrbitalCamera>()->m_FollowTarget = obj->GetComponent<Transform>().Get();
			orbitalCamA->GetComponent<OrbitalCamera>()->m_FollowOffset = glm::vec3(0.0f, 5.0f, 10.0f);

			orbitalCamB = new GameObject(m_EntityManager);
			orbitalCamB->AddComponent<Transform>(orbitalCamB);
			orbitalCamB->AddComponent<Camera>(orbitalCamB);
			orbitalCamB->GetComponent<Camera>()->SetPerspective((float)Application::Get().GetWindow().GetWidth() / (float)Application::Get().GetWindow().GetHeight());

			orbitalCamB->AddComponent<OrbitalCamera>();
			orbitalCamB->GetComponent<OrbitalCamera>()->m_LookTarget = obj->GetComponent<Transform>().Get();
			orbitalCamB->GetComponent<OrbitalCamera>()->m_FollowTarget = obj->GetComponent<Transform>().Get();
			orbitalCamB->GetComponent<OrbitalCamera>()->m_FollowOffset = glm::vec3(0.0f, 7.5f, 7.5f);

			mixingCam = new GameObject(m_EntityManager);
			mixingCam->AddComponent<Transform>(mixingCam);
			mixingCam->AddComponent<Camera>(mixingCam);
			mixingCam->GetComponent<Camera>()->SetPerspective((float)Application::Get().GetWindow().GetWidth() / (float)Application::Get().GetWindow().GetHeight());

			mixingCam->AddComponent<MixingCamera>(mixingCam);
			mixingCam->GetComponent<MixingCamera>()->SetTargetCamera(orbitalCamA->GetComponent<Camera>().Get());

			Camera::ActiveCamera = mixingCam->GetComponent<Camera>().Get();

			// HACKS END

			m_BatchRenderer->Configure();

			m_Scene.Render(true);
			
			m_BatchRenderer->Render();
		}

		virtual void OnUpdate(float dt) override
		{
			if (Input::IsKeyPressed(KeyCode::DEL))
			{
				WindowCloseEvent e;
				Application::Get().OnEvent(e);
			}

			if (Input::IsKeyPressed(KeyCode::F9))
			{
				Application::Get().GetWindow().SwitchCursorVisibility();
			}

			if (Input::IsKeyPressed(KeyCode::F5))
			{
				mixingCam->GetComponent<MixingCamera>()->SetTargetCamera(orbitalCamA->GetComponent<Camera>().Get());
				//m_Player->GetComponent<Material>()->SetShader(m_Scene.m_ShaderManager->Get("PBR"));
				m_Player->RemoveComponent<Material>();
				m_Player->AddComponent<Material>(*MaterialManager::getInstance()->Get("Bricks"));
			}

			if (Input::IsKeyPressed(KeyCode::F6))
			{
				mixingCam->GetComponent<MixingCamera>()->SetTargetCamera(orbitalCamB->GetComponent<Camera>().Get());
				//m_Player->GetComponent<Material>()->SetShader(m_Scene.m_ShaderManager->Get("Transparent"));
				m_Player->RemoveComponent<Material>();
				m_Player->AddComponent<Material>(*MaterialManager::getInstance()->Get("Transparent"));
			}

			if (Input::IsKeyPressed(KeyCode::F7))
			{
				mixingCam->GetComponent<MixingCamera>()->SetTargetCamera(flying->GetComponent<Camera>().Get());
				//Camera::ActiveCamera = flying->GetComponent<Camera>().Get();
			}

			{
				//PROFILE_SCOPE("ECS")
				m_SystemManager.UpdateAll(dt);
			}

		#ifdef DEBUG
			// IMGUI
			{
				//PROFILE_SCOPE("ImGui Scene Graph");

				ImGui::Begin("Scene graph");
				if (ImGui::TreeNode("Scene Game Objects"))
				{
					m_Scene.m_SceneRoot->ImGuiWriteSceneTree();
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("UI Game Objects"))
				{
					m_Scene.m_UIRoot->ImGuiWriteUITree();
					ImGui::TreePop();
				}
				ImGui::End();
			}
			//------
		#endif // DEBUG
		}

		virtual void OnRender(float dt) override
		{
			//PROFILE_SCOPE("ON RENDER")
			m_BatchRenderer->CalculateFrustum();

			{
				//PROFILE_SCOPE("SUBMIT COMMANDS")
				m_Scene.Render();
			}

			{
				//PROFILE_SCOPE("RENDER")
				m_BatchRenderer->Render();
			}

			{
				//PROFILE_SCOPE("DRAW GIZMOS")
				m_Scene.DrawGizmos();
			}
		}

	};

}

sixengine::Application* sixengine::CreateApplication()
{
	return new Game("App", 1280, 720);
}