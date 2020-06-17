#include <Engine.h>
#include <EntryPoint.h>

#include "Core/Scene.h"
#include "Renderer/BatchRenderer.h"
#include "Renderer/Texture.h"

// hacks
#include "Gameplay/Systems/AnimationSystem.h"
#include "Gameplay/Systems/ParticleSystem.h"
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

#include <AI/EnemiesManager.h>
#include <AI/StateMachineSystem.h>
#include <AI/States/IdleState.h>
#include <AI/States/PatrolState.h>
#include <AI/States/AttackState.h>
#include <AI/States/SearchState.h>
#include "AI/NavMesh/NavAgent.h"
#include "AI/NavMesh/NavAgentSystem.h"


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
		void MakeEnemy(glm::vec3 pos, glm::vec3 rotation)
		{
			GameObject* obj = new GameObject(m_EntityManager);
			obj->AddComponent<Transform>(obj);
			obj->GetComponent<Transform>()->SetWorldPosition(pos);
			obj->GetComponent<Transform>()->SetLocalScale(0.5f, 1.0f, 0.5f);
			obj->GetComponent<Transform>()->SetLocalOrientation(rotation);
			obj->AddComponent<BoxCollider>(glm::vec3(1.0f, 1.0f, 1.0f));
			obj->AddComponent<Mesh>(m_Scene.m_ModelManager->AddModel("res/models/primitives/cylinder.obj"));
			obj->AddComponent<Material>(*m_Scene.m_MaterialManager->Get("Red"));
			obj->AddComponent<DynamicBody>();

			GameObject* child = new GameObject(m_EntityManager);
			child->AddComponent<Transform>(child);
			child->GetComponent<Transform>()->SetParent(obj->GetComponent<Transform>().Get());
			child->GetComponent<Transform>()->SetLocalPosition(glm::vec3(0.0f, 0.6f, -0.75f));
			child->GetComponent<Transform>()->SetLocalScale(glm::vec3(1.0f, 0.2f, 1.0f));
			child->AddComponent<Mesh>(m_Scene.m_ModelManager->AddModel("res/models/primitives/cylinder.obj"));
			child->AddComponent<Material>(*m_Scene.m_MaterialManager->Get("Red"));

			obj->AddComponent<Enemy>(obj);
			obj->AddComponent<StateMachine>();
			obj->AddComponent<NavAgent>(obj);
			obj->GetComponent<StateMachine>()->m_States.push_back(new AttackState(obj));
			obj->GetComponent<StateMachine>()->m_States.push_back(new SearchState(obj));
			obj->GetComponent<StateMachine>()->m_States.push_back(new IdleState(obj));

			obj->GetComponent<StateMachine>()->m_CurrentState = obj->GetComponent<StateMachine>()->m_States[2];

			m_Scene.m_SceneRoot->AddChild(obj);
			m_Scene.m_SceneRoot->AddChild(child);
		}

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
			m_Scene.LoadScene("res/scenes/playground.scene");
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
			m_Scene.m_TextureArray->AddTexture("res/textures/ui/question_sign2.png");


			m_SystemManager.AddSystem<AnimationSystem>();
			m_SystemManager.AddSystem<ParticleSystem>();
			m_SystemManager.AddSystem<BillboardSystem>();

			//m_SystemManager.AddSystem<PlayerMaterialManagerSystem>();

			Shader* m_BasicShader2 = m_Scene.m_ShaderManager->AddShader("res/shaders/AnimationPBR.glsl");
			Shader* m_BasicShader = m_Scene.m_ShaderManager->AddShader("res/shaders/PBR.glsl");
			Shader* m_FontShader = m_Scene.m_ShaderManager->AddShader("res/shaders/Font.glsl");
			Shader* m_TransparentShader = m_Scene.m_ShaderManager->AddShader("res/shaders/Transparent.glsl");
			m_Scene.m_ShaderManager->AddShader("res/shaders/Depth.glsl");
			m_Scene.m_ShaderManager->AddShader("res/shaders/DepthAnim.glsl");
			m_Scene.m_ShaderManager->AddShader("res/shaders/Skybox.glsl");
			m_Scene.m_ShaderManager->AddShader("res/shaders/ParticlesShader.glsl");
			m_Scene.m_ShaderManager->AddShader("res/shaders/PostProcessing.glsl");

			m_BatchRenderer->SetBlurShader(m_Scene.m_ShaderManager->Get("PostProcessing"));

			Texture* particleTexture = new Texture("res/textures/particles/star.png");


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

			m_BatchRenderer->SetParticle(new ParticleRender(m_Scene.m_ShaderManager->Get("ParticlesShader")));

			Font* font = new Font("res/fonts/DroidSans.ttf");
			UI* ui = new UI(m_FontShader);
			ui->AddFont(font);

			m_BatchRenderer->AddTechnique(new AnimationPBR(m_BasicShader2));
			m_BatchRenderer->AddTechnique(new TransparentTechnique(m_TransparentShader));


			//TODO:*************
			//Shader* m_WaterShader = m_Scene.m_ShaderManager->AddShader("res/shaders/Water.glsl");
			//MaterialManager::getInstance()->CreateMaterial(
			//	m_Scene.m_ShaderManager->Get("Water"),
			//	glm::vec4(
			//		m_Scene.m_TextureArray->AddTexture("res/textures/water/dudv2.png"),
			//		m_Scene.m_TextureArray->AddTexture("res/textures/water/normal2.png"),
			//		0.0f,
			//		0.0f
			//	),
			//	"WaterMaterial");
			//m_Scene.m_ModelManager->AddModel("res/models/primitives/circleplane.obj");

			//Shader* bar = m_Scene.m_ShaderManager->AddShader("res/shaders/Bar.glsl");
			//MaterialManager::getInstance()->CreateMaterial(
			//	bar,
			//	glm::vec4(m_Scene.m_TextureArray->GetTexture("question_sign2"), 0.0f, 0.0f, 0.0f),
			//	"Bar");

			//m_Scene.m_BatchRenderer->AddTechnique(new StaticPBR(bar));

			//m_Scene.m_ModelManager->AddModel("res/models/primitives/billboard.obj");
			//GameObject* w;
			//w = new GameObject(m_EntityManager);
			//w->AddComponent<Transform>(w);
			//w->GetComponent<Transform>()->SetWorldPosition(-4.0f, 0.8f, -6.2f);
			////w->GetComponent<Transform>()->SetLocalScale(10.0, 1.0, 10.0);
			//w->AddComponent<Mesh>(m_Scene.m_ModelManager->GetModel("circleplane"));
			//w->AddComponent<Material>(*MaterialManager::getInstance()->Get("WaterMaterial"));
			//m_Scene.m_SceneRoot->AddChild(w);
			//Water* water = new Water(m_WaterShader, w);
			//m_BatchRenderer->SetWater(water);
			//m_BatchRenderer->AddTechnique(water);
			//TODO:*************

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
			obj->GetComponent<Transform>()->SetWorldPosition(10.0, 1.0f, 10.0f);
			obj->AddComponent<Mesh>(m_Scene.m_ModelManager->GetModel("WoodenCrate"));
			obj->AddComponent<BoxCollider>(glm::vec3(1, 1, 1), 0);
			obj->AddComponent<Collectable>();
			obj->AddComponent<Material>(*m_Scene.m_MaterialManager->Get("WoodenCrate2PBR"));
			m_Scene.m_SceneRoot->AddChild(obj);

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


			GameObject* airText = new GameObject(m_EntityManager);
			airText->AddComponent<Transform>(obj);
			airText->GetComponent<Transform>()->SetWorldPosition(5.0, 660.0f, 0.0f);
			airText->AddComponent<Text>("Air: ", glm::vec3(1.0f, 0.0f, 1.0f), 0.3f);
			airText->AddComponent<Material>(*MaterialManager::getInstance()->Get("FontMaterial"));
			airText->AddComponent<AirText>();
			m_Scene.m_UIRoot->AddChild(airText);

			obj = new GameObject(m_EntityManager);
			obj->AddComponent<Transform>(obj);
			obj->GetComponent<Transform>()->SetWorldPosition(-35.0f, 0.0f, 0.0f);
			obj->GetComponent<Transform>()->SetLocalScale(0.6f, 0.75f, 0.6f);
			obj->GetComponent<Transform>()->SetLocalOrientation(0.0, 0.0f, 0.0f);
			obj->AddComponent<Mesh>(m_Scene.m_ModelManager->AddModel("res/models/primitives/cylinder.obj"));
			obj->AddComponent<Material>(*m_Scene.m_MaterialManager->Get("Green"));
			obj->AddComponent<Animation>();
			obj->AddComponent<DynamicBody>();
			obj->AddComponent<ParticleEmitter>(particleTexture);
			obj->AddComponent<BoxCollider>(glm::vec3(1, 2, 1), 0);
			obj->AddComponent<SimplePlayer>(obj);
			m_SystemManager.AddSystem<AirTextSystem>();
			airText->GetComponent<AirText>()->player = obj->GetComponent<SimplePlayer>().Get();


			/*std::vector<GizmoVertex> vertices;
			std::vector<unsigned int> indices;*/
			PrimitiveUtils::GenerateBox(vertices, indices, 1, 2, 1);
			/*VertexArray**/ vao = new VertexArray();
			/*VertexBuffer**/ vbo = new VertexBuffer(&vertices[0], vertices.size());
			vbo->SetLayout({
				{ VertexDataType::VEC3F, "Position" }
				});
			/*IndexBuffer**/ ibo = new IndexBuffer(&indices[0], indices.size());
			vao->AddVertexBuffer(*vbo);
			vao->AddIndexBuffer(*ibo);
			obj->AddGizmo(new Gizmo(vao, m_Scene.m_ShaderManager->AddShader("res/shaders/Gizmo.glsl"), glm::vec3(0, 255, 0)));

			m_BatchRenderer->SetLight(new Light(obj));

			m_Scene.m_SceneRoot->AddChild(obj);

			flying = Camera::ActiveCamera->m_GameObject;

			m_SystemManager.AddSystem<OrbitalCameraSystem>();
			m_SystemManager.AddSystem<MixingCameraSystem>();
			m_SystemManager.AddSystem<DynamicBodySystem>();

			orbitalCamA = new GameObject(m_EntityManager);
			orbitalCamA->AddComponent<Transform>(orbitalCamA);
			orbitalCamA->AddComponent<Camera>(orbitalCamA);
			orbitalCamA->GetComponent<Camera>()->SetPerspective((float)Application::Get().GetWindow().GetWidth() / (float)Application::Get().GetWindow().GetHeight());
			obj->GetComponent<SimplePlayer>()->OnSurfaceCamera = orbitalCamA->GetComponent<Camera>().Get();

			orbitalCamA->AddComponent<OrbitalCamera>();
			orbitalCamA->GetComponent<OrbitalCamera>()->m_LookTarget = obj->GetComponent<Transform>().Get();
			orbitalCamA->GetComponent<OrbitalCamera>()->m_FollowTarget = obj->GetComponent<Transform>().Get();
			orbitalCamA->GetComponent<OrbitalCamera>()->m_FollowOffset = glm::vec3(0.0f, 6.0f, 12.0f);

			orbitalCamB = new GameObject(m_EntityManager);
			orbitalCamB->AddComponent<Transform>(orbitalCamB);
			orbitalCamB->AddComponent<Camera>(orbitalCamB);
			orbitalCamB->GetComponent<Camera>()->SetPerspective((float)Application::Get().GetWindow().GetWidth() / (float)Application::Get().GetWindow().GetHeight());
			obj->GetComponent<SimplePlayer>()->UnderSurfaceCamera = orbitalCamB->GetComponent<Camera>().Get();

			orbitalCamB->AddComponent<OrbitalCamera>();
			orbitalCamB->GetComponent<OrbitalCamera>()->m_LookTarget = obj->GetComponent<Transform>().Get();
			orbitalCamB->GetComponent<OrbitalCamera>()->m_FollowTarget = obj->GetComponent<Transform>().Get();
			orbitalCamB->GetComponent<OrbitalCamera>()->m_FollowOffset = glm::vec3(0.0f, 14.0f, 14.0f);

			mixingCam = new GameObject(m_EntityManager);
			mixingCam->AddComponent<Transform>(mixingCam);
			mixingCam->AddComponent<Camera>(mixingCam);
			mixingCam->GetComponent<Camera>()->SetPerspective((float)Application::Get().GetWindow().GetWidth() / (float)Application::Get().GetWindow().GetHeight());

			mixingCam->AddComponent<MixingCamera>(mixingCam);
			mixingCam->GetComponent<MixingCamera>()->SetTargetCamera(orbitalCamA->GetComponent<Camera>().Get());
			obj->GetComponent<SimplePlayer>()->MixingCamera = mixingCam->GetComponent<MixingCamera>().Get();

			Camera::ActiveCamera = mixingCam->GetComponent<Camera>().Get();

			m_SystemManager.AddSystem<StateMachineSystem>();
			m_SystemManager.AddSystem<NavAgentSystem>();

			obj = new GameObject(m_EntityManager);
			obj->AddComponent<EnemiesManager>();

			MakeEnemy(glm::vec3(17.30613f, 1.0f, 1.866652f), glm::vec3(220.029f, 0.0f, 0.0f));
			MakeEnemy(glm::vec3(-0.9738712, 1.0f, 7.736652f), glm::vec3(229.8f, 0.0f, 0.0f));
			MakeEnemy(glm::vec3(-7.743871f, 1.0f, 7.686653), glm::vec3(180, 0.0f, 0.0f));
			MakeEnemy(glm::vec3(-3.423871, 1.0f, -9.783347), glm::vec3(90, 0.0f, 0.0f));
			MakeEnemy(glm::vec3(-9.323872, 1.0f, -9.783347), glm::vec3(-90, 0.0f, 0.0f));


			//bar->Bind();
			//bar->SetFloat("barFill", m_BarFill);
			//bar->SetVec3("barOrientation", glm::vec3(.0f, 1.0f, 0.0f));
			//bar->Unbind();
			//m_Billboard = new GameObject(m_EntityManager);
			//m_Billboard->AddComponent<Transform>(m_Billboard);
			//m_Billboard->GetComponent<Transform>()->SetWorldPosition(0.0f, 2.0f, 0.0f);
			//m_Billboard->GetComponent<Transform>()->SetLocalScale(1.0f, 1.0f, 1.0f);
			//m_Billboard->GetComponent<Transform>()->SetLocalOrientation(0.0f, 90.0f, 90.0f);
			//m_Billboard->AddComponent<Mesh>(m_Scene.m_ModelManager->GetModel("billboard"));
			//m_Billboard->AddComponent<Material>(*MaterialManager::getInstance()->Get("Bar"));
			//m_Billboard->AddComponent<Billboard>(m_Billboard);
			//m_Scene.m_SceneRoot->AddChild(m_Billboard);
			// HACKS END

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
			/*m_BarFill = m_Billboard->GetComponent<Transform>()->GetLocalScale().y - .01f;
			m_BarFill = glm::clamp(m_BarFill, 0.0f, 1.0f);
			if (m_BarFill == .0f) m_BarFill = 1.00001f;
			MaterialManager::getInstance()->Get("Bar")->GetShader()->Bind();
			MaterialManager::getInstance()->Get("Bar")->GetShader()->SetFloat("barFill", m_BarFill);
			MaterialManager::getInstance()->Get("Bar")->GetShader()->SetVec3("barOrientation", glm::vec3(1.0f, .0f, 0.0f));
			MaterialManager::getInstance()->Get("Bar")->GetShader()->SetVec3("color", glm::vec3(1.0f, .0f, 0.0f));
			MaterialManager::getInstance()->Get("Bar")->GetShader()->Unbind();
			m_Billboard->GetComponent<Transform>()->SetLocalScale(1.0f, m_BarFill, 1.0f);
			LOG_CORE_INFO("{0} {1} {2} {3}", m_Billboard->GetComponent<Transform>()->GetLocalScale().x, m_Billboard->GetComponent<Transform>()->GetLocalScale().y, m_Billboard->GetComponent<Transform>()->GetLocalScale().z, m_BarFill);*/
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
				//m_Player->RemoveComponent<Material>();
				//m_Player->AddComponent<Material>(*MaterialManager::getInstance()->Get("parasiteZombie"));
			}

			if (Input::IsKeyPressed(KeyCode::F6))
			{
				mixingCam->GetComponent<MixingCamera>()->SetTargetCamera(orbitalCamB->GetComponent<Camera>().Get());
				//m_Player->GetComponent<Material>()->SetShader(m_Scene.m_ShaderManager->Get("Transparent"));
				//m_Player->RemoveComponent<Material>();
				//m_Player->AddComponent<Material>(*MaterialManager::getInstance()->Get("Transparent"));
			}

			if (Input::IsKeyPressed(KeyCode::F7))
			{
				mixingCam->GetComponent<MixingCamera>()->SetTargetCamera(flying->GetComponent<Camera>().Get());
				//Camera::ActiveCamera = flying->GetComponent<Camera>().Get();
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

				glCullFace(GL_FRONT);
				attack->Draw(attack->model);
			}
		}

	};

}

sixengine::Application* sixengine::CreateApplication()
{
	return new Game("App", 1280, 720);
}