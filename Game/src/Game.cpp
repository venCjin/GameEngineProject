#include <Engine.h>
#include <EntryPoint.h>

#include "Core/Scene.h"
#include "Renderer/BatchRenderer.h"
#include "Renderer/Texture.h"

// hacks
#include "Renderer/PrimitiveUtils.h"
#include "Renderer/Gizmo.h"

#include <Core/CameraSystem/FlyingCameraSystem.h>
#include <Core/CameraSystem/OrbitalCameraSystem.h>
#include <Core/CameraSystem/MixingCameraSystem.h>
// hacks end

#include <Core/AudioManager.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <Physics\Components\DynamicBody.h>
#include <Physics/Systems/DynamicBodySystem.h>
#include <Gameplay\Components\AirText.h>
#include <Gameplay\Systems\AirTextSystem.h>

#include <Gameplay/Components/Collectable.h>
#include <Gameplay\Components\Billboard.h>
#include <Gameplay/Systems/BillboardSystem.h>
#include <Renderer/Techniques/StaticPBR.h>
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
		GameObject* m_Billboard;
		float m_BarFill;
		float shakeTimer;
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
			m_Scene.LoadScene("res/scenes/new.scene");
			//ADD_TRACK("res/sounds/ophelia.mp3", "ophelia");
			//PLAY_TRACK("ophelia");

			// HACKS
			//////SHITT!!!11
			std::vector<GizmoVertex> vertices;
			std::vector<unsigned int> indices;
			PrimitiveUtils::GenerateSphere(vertices, indices, 1.0f);
			VertexArray* vao = new VertexArray();
			VertexBuffer* vbo = new VertexBuffer(&vertices[0], vertices.size());
			vbo->SetLayout({
				{ VertexDataType::VEC3F, "Position" }
				});
			IndexBuffer* ibo = new IndexBuffer(&indices[0], indices.size());
			vao->AddVertexBuffer(*vbo);
			vao->AddIndexBuffer(*ibo);
			Application::attack = new Gizmo(vao, m_Scene.m_ShaderManager->AddShader("res/shaders/Gizmo.glsl"), glm::vec3(255, 0, 0));
			//////SHIT!!!!
			
			GameObject* obj;
			//BAR
			Shader* bar = m_Scene.m_ShaderManager->AddShader("res/shaders/Bar.glsl");
			m_Scene.m_TextureArray->AddTexture("res/textures/ui/question_sign2.png");
			MaterialManager::getInstance()->CreateMaterial(
				bar,
				glm::vec4(m_Scene.m_TextureArray->GetTexture("question_sign2"), 0.0f, 0.0f, 0.0f),
				"Bar");
			m_Scene.m_BatchRenderer->AddTechnique(new StaticPBR(bar));
			m_Scene.m_ModelManager->AddModel("res/models/primitives/billboard.obj");
			bar->Bind();
			bar->SetFloat("barFill", m_BarFill);
			bar->SetVec3("barOrientation", glm::vec3(0.0f, 1.0f, 0.0f));
			bar->Unbind();
			m_Billboard = new GameObject(m_EntityManager);
			m_Billboard->AddComponent<Transform>(m_Billboard);
			m_Billboard->GetComponent<Transform>()->SetWorldPosition(0.0f, 2.0f, 0.0f);
			m_Billboard->GetComponent<Transform>()->SetLocalScale(1.0f, 1.0f, 1.0f);
			m_Billboard->GetComponent<Transform>()->SetLocalOrientation(0.0f, 90.0f, 90.0f);
			m_Billboard->AddComponent<Mesh>(m_Scene.m_ModelManager->GetModel("billboard"));
			m_Billboard->AddComponent<Material>(*MaterialManager::getInstance()->Get("Bar"));
			m_Billboard->AddComponent<Billboard>(m_Billboard);
			m_Scene.m_SceneRoot->AddChild(m_Billboard);
			//BAR

			//PLAYER
			/*obj = new GameObject(m_EntityManager);
			obj->AddComponent<Transform>(obj);
			obj->GetComponent<Transform>()->SetWorldPosition(0.0f, 0.0f, 0.0f);
			obj->GetComponent<Transform>()->SetLocalScale(0.01f, 0.01f, 0.01f);
			obj->GetComponent<Transform>()->SetLocalOrientation(180.0f, 0.0f, 0.0f);
			m_Scene.m_ModelManager->AddModel("res/models/par/par.dae");
			obj->AddComponent<Mesh>(m_Scene.m_ModelManager->GetModel("par"));
			m_Scene.m_ModelManager->GetModel("par")->LoadAnimation("res/models/par/par_idle.dae", "idle");
			m_Scene.m_ModelManager->GetModel("par")->LoadAnimation("res/models/par/par_walk.dae", "walk");
			m_Scene.m_ModelManager->GetModel("par")->LoadAnimation("res/models/par/par_punch.dae", "punch");
			obj->AddComponent<Material>(*m_Scene.m_MaterialManager->Get("parasiteZombie"));
			obj->AddComponent<Animation>();
			obj->AddComponent<DynamicBody>();
			Texture* particleTexture = new Texture("res/textures/particles/star.png");
			obj->AddComponent<ParticleEmitter>(particleTexture);
			obj->AddComponent<BoxCollider>(glm::vec3(1, 2, 1), 0);
			obj->AddComponent<SimplePlayer>(obj);
			//std::vector<GizmoVertex> vertices;
			//std::vector<unsigned int> indices;
			PrimitiveUtils::GenerateBox(vertices, indices, 100, 200, 100);
			//VertexArray*
			vao = new VertexArray();
			//VertexBuffer*
			vbo = new VertexBuffer(&vertices[0], vertices.size());
			vbo->SetLayout({
				{ VertexDataType::VEC3F, "Position" }
				});
			//IndexBuffer*
			ibo = new IndexBuffer(&indices[0], indices.size());
			vao->AddVertexBuffer(*vbo);
			vao->AddIndexBuffer(*ibo);
			obj->AddGizmo(new Gizmo(vao, m_Scene.m_ShaderManager->AddShader("res/shaders/Gizmo.glsl"), glm::vec3(0, 255, 0)));
			m_Scene.m_SceneRoot->AddChild(obj);
			m_BatchRenderer->SetLight(new Light(obj));*/
			//PLAYER


			//COLLECTABLE
			// TODO: 2 collectable cause a crash on collision with one of them
			// TODO: also player and collectable must not be separated with other GO with collider in sceneRoot
			/*obj = new GameObject(m_EntityManager);
			obj->AddComponent<Transform>(obj);
			obj->GetComponent<Transform>()->SetWorldPosition(5.0, 1.0f, 5.0f);
			obj->AddComponent<Mesh>(m_Scene.m_ModelManager->GetModel("WoodenCrate"));
			obj->AddComponent<BoxCollider>(glm::vec3(1, 1, 1), 0);
			obj->AddComponent<Collectable>();
			obj->AddComponent<Material>(*m_Scene.m_MaterialManager->Get("WoodenCratePBR"));
			m_Scene.m_SceneRoot->AddChild(obj);*/
			//COLLECTABLE

			//COLLECTABLE2
			obj = new GameObject(m_EntityManager);
			obj->AddComponent<Transform>(obj);
			obj->GetComponent<Transform>()->SetWorldPosition(10.0, 1.0f, 10.0f);
			obj->AddComponent<Mesh>(m_Scene.m_ModelManager->GetModel("WoodenCrate"));
			obj->AddComponent<BoxCollider>(glm::vec3(1, 1, 1), 0);
			obj->AddComponent<Collectable>();
			obj->AddComponent<Material>(*m_Scene.m_MaterialManager->Get("WoodenCrate2PBR"));
			m_Scene.m_SceneRoot->AddChild(obj);
			//COLLECTABLE2
			
			
		#if SCENE_ENDS_IN_GAME_CPP
			m_Scene.m_ModelManager->CreateVAO();
			m_Scene.m_TextureArray->CreateTextureArray();
		#endif
			// HACKS END

			// Cameras setup
			flying = Camera::ActiveCamera->m_GameObject; //flying camera domyœlnie stworzona w konstruktorze Scene

			//obj = m_Scene.GetFirstGameObjectWithComponent<SimplePlayer>();
			obj = m_Scene.GetGameObjectsWithComponent<SimplePlayer>()[0];

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
			mixingCam->GetComponent<MixingCamera>()->SetTargetCamera(orbitalCamB->GetComponent<Camera>().Get());
			
			Camera::ActiveCamera = mixingCam->GetComponent<Camera>().Get();
			// Cameras setup end

			m_BatchRenderer->Configure();

			m_Scene.Render(true);
			
			m_BatchRenderer->Render();
		}

		virtual void OnUpdate(float dt) override
		{
			if (shakeTimer > 0)
				shakeTimer -= dt;
			else
				m_BatchRenderer->SetBlur(false);

			m_BarFill = m_Billboard->GetComponent<Transform>()->GetLocalScale().y - .01f;
			m_BarFill = glm::clamp(m_BarFill, 0.0f, 1.0f);
			if (m_BarFill == .0f) m_BarFill = 1.00001f;
			MaterialManager::getInstance()->Get("Bar")->GetShader()->Bind();
			MaterialManager::getInstance()->Get("Bar")->GetShader()->SetFloat("barFill", m_BarFill);
			MaterialManager::getInstance()->Get("Bar")->GetShader()->SetVec3("barOrientation", glm::vec3(1.0f, .0f, 0.0f));
			MaterialManager::getInstance()->Get("Bar")->GetShader()->SetVec3("color", glm::vec3(1.0f, .0f, 0.0f));
			MaterialManager::getInstance()->Get("Bar")->GetShader()->Unbind();
			m_Billboard->GetComponent<Transform>()->SetLocalScale(1.0f, m_BarFill, 1.0f);
			LOG_CORE_INFO("{0} {1} {2} {3}", m_Billboard->GetComponent<Transform>()->GetLocalScale().x, m_Billboard->GetComponent<Transform>()->GetLocalScale().y, m_Billboard->GetComponent<Transform>()->GetLocalScale().z, m_BarFill);
			
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
			}

			if (Input::IsKeyPressed(KeyCode::F6))
			{
				mixingCam->GetComponent<MixingCamera>()->SetTargetCamera(orbitalCamB->GetComponent<Camera>().Get());
			}

			if (Input::IsKeyPressed(KeyCode::F7))
			{
				mixingCam->GetComponent<MixingCamera>()->SetTargetCamera(flying->GetComponent<Camera>().Get());
			}

			if (Input::IsKeyPressed(KeyCode::P))
			{
				m_BatchRenderer->SetBlur(true);
				shakeTimer = 0.1f;
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

				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glDisable(GL_CULL_FACE);
				glLineWidth(1.0f);
				attack->Draw(attack->model);
				glPolygonMode(GL_FRONT, GL_FILL);
				glEnable(GL_CULL_FACE);
				glCullFace(GL_BACK);
			}
		}

	};

}

sixengine::Application* sixengine::CreateApplication()
{
	return new Game("App", 1280, 720);
}