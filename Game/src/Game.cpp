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
#include <Physics/Components/DynamicBody.h>
#include <Physics/Components/StaticBody.h>
#include <Physics/Systems/DynamicBodySystem.h>
#include <Gameplay/Components/AirText.h>
#include <Gameplay/Systems/AirTextSystem.h>
#include <Gameplay/Systems/GateSystem.h>

#include <Gameplay/Components/Collectable.h>
#include "Gameplay/Components/Generator.h"
#include "Gameplay/Components/Gate.h"
#include <Gameplay/Components/Billboard.h>
#include <Gameplay/Systems/BillboardSystem.h>
#include <Gameplay/Systems/AnimationSystem.h>
#include <Gameplay/Systems/ParticleSystem.h>
#include <Gameplay/Systems/ScolopendraSystem.h>
#include <Gameplay/Systems/ProjectileSystem.h>
#include <Renderer/Techniques/StaticPBR.h>
#include <Renderer/Techniques/AnimationPBR.h>
#include <Renderer/Techniques/UI.h>
#include <Renderer/Techniques/Transparent.h>

#include <AI/EnemiesManager.h>
#include <AI/StateMachineSystem.h>
#include <AI/States/IdleState.h>
#include <AI/States/PatrolState.h>
#include <AI/States/AttackState.h>
#include <AI/States/SearchState.h>
#include "AI/NavMesh/NavAgent.h"
#include "AI/NavMesh/NavAgentSystem.h"
#include "Gameplay/Systems/QuestionmarkSystem.h"
#include "Renderer/Techniques/QuestionmarkTechnique.h"
#include <Gameplay/Systems/StaticLoopedSoundSystem.h>
#include <Gameplay/Components/LoopedSound.h>
#include  <Gameplay/Systems/DynamicSoundSystem.h>
#include <Gameplay/Components/BackgroundSound.h>
#include <Gameplay/Components/BackgroundSoundSystem.h>


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
		//GameObject* m_Player;
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
			obj->GetComponent<Transform>()->SetLocalScale(0.02f, 0.02f, 0.02f);
			obj->GetComponent<Transform>()->SetLocalOrientation(rotation);
			obj->AddComponent<BoxCollider>(glm::vec3(1.0f, 2.0f, 1.0f));
			obj->AddComponent<Mesh>(m_Scene.m_ModelManager->AddModel("res/models/Enemies/BlackAgent/agent2.dae"));
			obj->AddComponent<LoopedSound3D>("footstep", pos, 5.0f);
			//obj->AddComponent<Animation>();
			obj->AddComponent<Material>(*m_Scene.m_MaterialManager->Get("AgentMaterial"));
			obj->AddComponent<DynamicBody>();
			obj->AddComponent<Enemy>(obj);
			obj->AddComponent<StateMachine>();
			obj->AddComponent<NavAgent>(obj);
			obj->GetComponent<StateMachine>()->m_States.push_back(new AttackState(obj));
			obj->GetComponent<StateMachine>()->m_States.push_back(new SearchState(obj));
			obj->GetComponent<StateMachine>()->m_States.push_back(new IdleState(obj));
			obj->GetComponent<StateMachine>()->m_CurrentState = obj->GetComponent<StateMachine>()->m_States[2];
			m_Scene.m_SceneRoot->AddChild(obj);

			// pistol
			// bone name : mixamorig_RightHand
			GameObject* pistol = new GameObject(m_EntityManager);
			pistol->AddComponent<Transform>(pistol);
			pistol->GetComponent<Transform>()->SetParent(obj->GetComponent<Transform>().Get());
			//pistol->GetComponent<Transform>()->Rotate();
			pistol->AddComponent<Mesh>(m_Scene.m_ModelManager->AddModel("res/models/Enemies/Gun/pm-40.obj"));
			pistol->AddComponent<Material>(*m_Scene.m_MaterialManager->Get("PistolMaterial"));
			obj->AddChild(pistol);
			/*
			GameObject* p = new GameObject(m_EntityManager);
			p->AddComponent<Transform>(p);
			p->GetComponent<Transform>()->SetParent(pistol->GetComponent<Transform>().Get());
			p->AddComponent<Mesh>(m_Scene.m_ModelManager->AddModel("res/models/Enemies/Gun/MP40_Slider.obj"));
			p->AddComponent<Material>(*m_Scene.m_MaterialManager->Get("PistolMaterial"));
			pistol->AddChild(p);
			p = new GameObject(m_EntityManager);
			p->AddComponent<Transform>(p);
			p->GetComponent<Transform>()->SetParent(pistol->GetComponent<Transform>().Get());
			p->AddComponent<Mesh>(m_Scene.m_ModelManager->AddModel("res/models/Enemies/Gun/PM40_Receiver.obj"));
			p->AddComponent<Material>(*m_Scene.m_MaterialManager->Get("PistolMaterial"));
			pistol->AddChild(p);
			p = new GameObject(m_EntityManager);
			p->AddComponent<Transform>(p);
			p->GetComponent<Transform>()->SetParent(pistol->GetComponent<Transform>().Get());
			p->AddComponent<Mesh>(m_Scene.m_ModelManager->AddModel("res/models/Enemies/Gun/MP40_Trigger.obj"));
			p->AddComponent<Material>(*m_Scene.m_MaterialManager->Get("PistolMaterial"));
			pistol->AddChild(p);
			p = new GameObject(m_EntityManager);
			p->AddComponent<Transform>(p);
			p->GetComponent<Transform>()->SetParent(pistol->GetComponent<Transform>().Get());
			p->AddComponent<Mesh>(m_Scene.m_ModelManager->AddModel("res/models/Enemies/Gun/PM40_Magazine.obj"));
			p->AddComponent<Material>(*m_Scene.m_MaterialManager->Get("PistolMaterial"));
			pistol->AddChild(p);
			*/
			//pistol

			//question mark
			m_Billboard = new GameObject(m_EntityManager);
			m_Billboard->AddComponent<Transform>(m_Billboard);
			//m_Billboard->GetComponent<Transform>()->SetParent(child->GetComponent<Transform>().Get());
			//m_Billboard->GetComponent<Transform>()->SetLocalScale(glm::vec3(1.0f, 5.f, 1.0f));
			m_Billboard->GetComponent<Transform>()->SetLocalPosition(glm::vec3(0, 2, 0));
			m_Billboard->AddComponent<Mesh>(m_Scene.m_ModelManager->GetModel("billboard"));
			m_Billboard->AddComponent<Material>(*MaterialManager::getInstance()->Get("Bar"));
			m_Billboard->AddComponent<Questionmark>();
			m_Billboard->GetComponent<Questionmark>()->enemy = obj;
			m_Billboard->AddComponent<Billboard>(m_Billboard);
			obj->AddChild(m_Billboard);
			//question mark
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
			Texture* starParticleTexture = new Texture("res/textures/particles/star.png");
			Texture* rippleParticleTexture = new Texture("res/textures/particles/ripple.png");

			m_SystemManager.AddSystem<GateSystem>();


			m_Scene.LoadScene("res/scenes/exported.scene");
			//ADD_TRACK("res/sounds/solider base/military-helicopter.wav", "ophelia");
			//INIT_TRACK("ophelia");

			ADD_TRACK("res/sounds/electricity-generator-loop.mp3", "generator");
			ADD_TRACK("res/sounds/footstep-gravel.mp3", "footstep");
			ADD_TRACK("res/sounds/desert/wind-sounds.mp3", "wind");
			ADD_TRACK("res/sounds/solider_base/military-helicopter.mp3", "helicopter");

			//INIT_TRACK_3D("generator", glm::vec3(0.0f));
			//INIT_TRACK_3D_LOOPED("generator", glm::vec3(30.0f, 0, 0));

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
			Shader* bar = m_Scene.m_ShaderManager->AddShader("res/shaders/Questionmark.glsl");
			m_Scene.m_TextureArray->AddTexture("res/textures/ui/question_sign2.png");
			m_Scene.m_TextureArray->AddTexture("res/textures/ui/question_sign2_outline.png");
			MaterialManager::getInstance()->CreateMaterial(
				bar,
				glm::vec4(m_Scene.m_TextureArray->GetTexture("question_sign2"), m_Scene.m_TextureArray->GetTexture("question_sign2_outline"), 0.0f, 0.0f),
				"Bar");
			m_Scene.m_ModelManager->AddModel("res/models/primitives/billboard.obj");
			//BAR
			//Audio
			obj = new GameObject(m_EntityManager);
			obj->AddComponent<BackgroundSound>(100.0f, 100.0f, "helicopter", 3.0f, .25f);
			//obj->GetComponent<BackgroundSound>()->m_Sound->setPlaybackSpeed(3.0f);
			//obj->AddComponent<LoopedSound>("wind");
			//obj->GetComponent<LoopedSound>()->Play();
			//obj->GetComponent<LoopedSound>()->SetVolume(.5f);
			//Audio
			Texture* particleTexture = new Texture("res/textures/particles/star.png");
			
			// WORKING SCOLOPENDRA
			m_Scene.m_ModelManager->AddModel("res/models/scolopendra/scolo.dae");

			GameObject* player = new GameObject(m_EntityManager);
			player->AddComponent<Transform>(player);
			player->GetComponent<Transform>()->SetWorldPosition(-35.0f, 0.0f, 0.0f);
			player->GetComponent<Transform>()->SetLocalScale(0.001f, 0.001f, 0.001f);
			player->AddComponent<Mesh>(m_Scene.m_ModelManager->AddModel("res/models/primitives/cylinder.obj"));
			player->AddComponent<Material>(*m_Scene.m_MaterialManager->Get("Green"));
			player->AddComponent<DynamicBody>();
			player->AddComponent<BoxCollider>(glm::vec3(1, 2, 1), 0);
			player->AddComponent<SimplePlayer>(player);
			m_BatchRenderer->SetLight(new Light(player));
			m_Scene.m_SceneRoot->AddChild(player);

			GameObject* scolopendra = new GameObject(m_EntityManager);
			scolopendra->AddComponent<Transform>(scolopendra);
			scolopendra->AddComponent<Mesh>(m_Scene.m_ModelManager->GetModel("scolo"));
			scolopendra->AddComponent<Material>(*m_Scene.m_MaterialManager->Get("GreenAnim"));
			scolopendra->AddComponent<Animation>();
			scolopendra->AddComponent<ScolopendraComponent>(scolopendra, player);
			m_Scene.m_SceneRoot->AddChild(scolopendra);

			scolopendra->GetComponent<Animation>().Get()->LoadAnimation("res/models/scolopendra/scolo_idle.dae", "idle", false, 0.5f);
			scolopendra->GetComponent<Animation>().Get()->LoadAnimation("res/models/scolopendra/scolo_attack.dae", "attack", true, 0.0f);
			player->GetComponent<SimplePlayer>().Get()->scolopendraMaterial = scolopendra->GetComponent<Material>().Get();
			player->GetComponent<SimplePlayer>().Get()->scolopendraAnimation = scolopendra->GetComponent<Animation>().Get();
			//m_Scene.m_ModelManager->GetModel("scolo")->LoadAnimation("res/models/scolopendra/scolo_anim.dae", "idle");


			//Texture* particleTexture = new Texture("res/textures/particles/star.png");
			//COLLECTABLE
			obj = new GameObject(m_EntityManager);
			obj->AddComponent<Transform>(obj);
			obj->GetComponent<Transform>()->SetWorldPosition(5.0, 0.5f, 5.0f);
			obj->AddComponent<Mesh>(m_Scene.m_ModelManager->GetModel("WoodenCrate"));
			obj->AddComponent<BoxCollider>(glm::vec3(1, 1, 1), 0);
			obj->AddComponent<Collectable>();
			obj->AddComponent<Material>(*m_Scene.m_MaterialManager->Get("WoodenCratePBR"));

			m_Scene.m_SceneRoot->AddChild(obj);
			//COLLECTABLE

			//COLLECTABLE2
			obj = new GameObject(m_EntityManager);
			obj->AddComponent<Transform>(obj);
			obj->GetComponent<Transform>()->SetWorldPosition(10.0, 0.5f, 9.0f);
			obj->AddComponent<Mesh>(m_Scene.m_ModelManager->GetModel("WoodenCrate"));
			obj->AddComponent<BoxCollider>(glm::vec3(1, 1, 1), 0);
			obj->AddComponent<Collectable>();
			obj->AddComponent<Material>(*m_Scene.m_MaterialManager->Get("WoodenCrate2PBR"));
			m_Scene.m_SceneRoot->AddChild(obj);
			//COLLECTABLE2

			//Generator 1
			obj = new GameObject(m_EntityManager);
			obj->AddComponent<Transform>(obj);
			obj->GetComponent<Transform>()->SetWorldPosition(5.0, 0.5f, -5.0f);
			obj->AddComponent<Mesh>(m_Scene.m_ModelManager->GetModel("Generator"));
			obj->AddComponent<StaticBody>();
			obj->AddComponent<BoxCollider>(glm::vec3(1, 1, 1), true);
			obj->AddComponent<Material>(*m_Scene.m_MaterialManager->Get("YellowGeneratorPBR"));
			obj->AddComponent<Generator>(m_Scene.m_ModelManager->GetModel("WoodenCrate"), obj);
			obj->AddComponent<ParticleEmitter>(obj, starParticleTexture);
			obj->AddComponent<LoopedSound3D>("generator", obj->GetComponent<Transform>()->GetWorldPosition(), 15.f);
			obj->GetComponent<LoopedSound3D>()->SetMinDistance(-1.0f);
			Generator* gen1 = obj->GetComponent<Generator>().Get();
			m_Scene.m_SceneRoot->AddChild(obj);

			obj = new GameObject(m_EntityManager);
			obj->AddComponent<Transform>(obj);
			obj->GetComponent<Transform>()->SetWorldPosition(9.0, 0.5f, -9.0f);
			obj->AddComponent<ParticleEmitter>(obj, starParticleTexture);
			m_Scene.m_SceneRoot->AddChild(obj);

			//Generator 2
			obj = new GameObject(m_EntityManager);
			obj->AddComponent<Transform>(obj);
			obj->GetComponent<Transform>()->SetWorldPosition(100.0, 0.5f, -2.0f);
			obj->AddComponent<Mesh>(m_Scene.m_ModelManager->GetModel("Generator"));
			obj->AddComponent<StaticBody>();
			obj->AddComponent<BoxCollider>(glm::vec3(1, 1, 1), true);
			obj->AddComponent<Material>(*m_Scene.m_MaterialManager->Get("YellowGeneratorPBR"));
			obj->AddComponent<Generator>(m_Scene.m_ModelManager->GetModel("WoodenCrate"), obj);
			obj->AddComponent<ParticleEmitter>(obj, starParticleTexture);
			obj->AddComponent<LoopedSound3D>("generator", obj->GetComponent<Transform>()->GetWorldPosition(), 15.f);
			obj->GetComponent<LoopedSound3D>()->SetMinDistance(-1.0f);
			Generator* gen2 = obj->GetComponent<Generator>().Get();
			m_Scene.m_SceneRoot->AddChild(obj);

			//Gate
			obj = new GameObject(m_EntityManager);
			obj->AddComponent<Transform>(obj);
			obj->GetComponent<Transform>()->SetWorldPosition(10.0, 0.0f, -5.0f);
			obj->GetComponent<Transform>()->SetLocalScale(1.0, 6.0f, 10.0f);
			obj->AddComponent<Mesh>(m_Scene.m_ModelManager->GetModel("WoodenCrate"));
			obj->AddComponent<StaticBody>();
			obj->AddComponent<BoxCollider>(glm::vec3(1, 1, 1), true);
			obj->AddComponent<Material>(*m_Scene.m_MaterialManager->Get("WoodenCrate2PBR"));
			obj->AddComponent<Gate>();
			obj->GetComponent<Gate>()->AddGenerator(gen1);
			obj->GetComponent<Gate>()->AddGenerator(gen2);
			m_Scene.m_SceneRoot->AddChild(obj);
			
			// PARTICLE SYSTEM1
			obj = new GameObject(m_EntityManager);
			obj->AddComponent<Transform>(obj);
			obj->GetComponent<Transform>()->SetWorldPosition(-1.0f, 0.1f, 0.0f);
			obj->GetComponent<Transform>()->SetLocalScale(0.001f, 0.001f, 0.001f);
			obj->AddComponent<Mesh>(m_Scene.m_ModelManager->AddModel("res/models/primitives/cylinder.obj"));
			obj->AddComponent<Material>(*m_Scene.m_MaterialManager->Get("Green"));

			obj->AddComponent<ParticleEmitter>(obj, rippleParticleTexture, std::string("EnemyEffect"));
			m_Scene.m_SceneRoot->AddChild(obj);
			// END Particle Systems

			//ENEMIES
			m_SystemManager.AddSystem<StateMachineSystem>();
			m_SystemManager.AddSystem<NavAgentSystem>();

			obj = new GameObject(m_EntityManager);
			obj->AddComponent<EnemiesManager>();

			MakeEnemy(glm::vec3(17.30613f, 0.0f, 1.866652f), glm::vec3(220.029f, 0.0f, 0.0f));
			MakeEnemy(glm::vec3(-0.9738712, 0.0f, 7.736652f), glm::vec3(229.8f, 0.0f, 0.0f));
			MakeEnemy(glm::vec3(-7.743871f, 0.0f, 7.686653), glm::vec3(180, 0.0f, 0.0f));
			MakeEnemy(glm::vec3(-3.423871, 0.0f, -9.783347), glm::vec3(90, 0.0f, 0.0f));
			MakeEnemy(glm::vec3(-9.323872, 0.0f, -9.783347), glm::vec3(-90, 0.0f, 0.0f));
			//ENEMIES

			//Projetlie
			m_Scene.m_ModelManager->AddModel("res/models/Enemies/Bullet/Bullet.obj");
			MaterialManager::getInstance()->CreateMaterial(
				m_Scene.m_ShaderManager->Get("PBR"),
				glm::vec4(
					m_Scene.m_TextureArray->AddTexture("res/models/Enemies/Bullet/al.png"), 
					m_Scene.m_TextureArray->AddTexture("res/models/Enemies/Bullet/no.png"),
					0.0f,
					0.0f),
				"BulletMaterial");
			m_SystemManager.AddSystem<ProjectileSystem>();
			//Projetlie

		#if SCENE_ENDS_IN_GAME_CPP
			m_Scene.m_ModelManager->CreateVAO();
			m_Scene.m_TextureArray->CreateTextureArray();
#endif
			// HACKS END

			// CAMERAS SETUP
			flying = Camera::ActiveCamera->m_GameObject; //flying camera domy�lnie stworzona w konstruktorze Scene

			//obj = m_Scene.GetFirstGameObjectWithComponent<SimplePlayer>();
			obj = player;//m_Scene.GetGameObjectsWithComponent<SimplePlayer>()[0];

			m_SystemManager.AddSystem<OrbitalCameraSystem>();
			m_SystemManager.AddSystem<MixingCameraSystem>();
			m_SystemManager.AddSystem<QuestionmarkSystem>();
			m_SystemManager.AddSystem<DynamicBodySystem>();
			m_SystemManager.AddSystem<ScolopendraSystem>();
			m_SystemManager.AddSystem<LoopedSoundSystem>();
			m_SystemManager.AddSystem<StaticLoopedSound3DSystem>(m_Scene.GetGameObjectsWithComponent<SimplePlayer>()[0]);
			m_SystemManager.AddSystem<DynamicSoundSystem>();
			m_SystemManager.AddSystem<BackgroundSoundSystem>();

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
			// CAMERAS SETUP END

			m_BatchRenderer->Configure();

			m_Scene.Render(true);

			m_BatchRenderer->Render();
		}

		virtual void OnUpdate(float dt) override
		{
			if (shakeTimer > 0)
				shakeTimer -= dt;
			else
			{
				//m_BatchRenderer->SetBlur(false);
				//m_BatchRenderer->SetShake(false);
			}


			//AUDIO
			AudioManager::getInstance()->ClearSoundsArray();
			std::vector<irrklang::ISound*> s = AudioManager::getInstance()->sounds;
			for (auto sound : s)
			{
				LOG_CORE_ERROR("Sound: {0}", sound->getIsPaused());
			}
			//AUDIO
			// BAR
			/*m_BarFill += .01f;
			m_BarFill = glm::clamp(m_BarFill, 0.0f, 1.0f);
			if (m_BarFill == 1.0f) m_BarFill = .00001f;
			MaterialManager::getInstance()->Get("Bar")->GetShader()->Bind();
			MaterialManager::getInstance()->Get("Bar")->GetShader()->SetFloat("barFill", m_BarFill);
			MaterialManager::getInstance()->Get("Bar")->GetShader()->SetVec3("barOrientation", glm::vec3(1.0f, 0.0f, 0.0f));
			MaterialManager::getInstance()->Get("Bar")->GetShader()->SetVec3("color", glm::vec3(1.0f, 0.0f, 0.0f));
			MaterialManager::getInstance()->Get("Bar")->GetShader()->Unbind();
			m_Billboard->GetComponent<Transform>()->SetLocalPosition(m_BarFill*10.0f, 2, 1.0f);*/
			//LOG_CORE_INFO("{0} {1} {2} {3}", m_Billboard->GetComponent<Transform>()->GetLocalScale().x, m_Billboard->GetComponent<Transform>()->GetLocalScale().y, m_Billboard->GetComponent<Transform>()->GetLocalScale().z, m_BarFill);
			// BAR

			if (Input::IsKeyPressed(KeyCode::DEL))
			{
				WindowCloseEvent e;
				Application::Get().OnEvent(e);
			}

			if (Input::IsKeyPressed(KeyCode::F9))
			{
				Application::Get().GetWindow().SwitchCursorVisibility();
			}

			if (Input::IsKeyPressed(KeyCode::N))
			{
				//INIT_TRACK_3D("generator", glm::vec3(30.0f, 0, 0));
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

			if (Input::IsKeyPressed(KeyCode::B))
			{
				m_BatchRenderer->SetBlur(true);
				shakeTimer = 1.0f;
			}

			if (Input::IsKeyPressed(KeyCode::N))
			{
				m_BatchRenderer->SetShake(true);
				shakeTimer = 1.0f;
			}

			if (Input::IsKeyPressed(KeyCode::M))
			{
				m_BatchRenderer->SetBlur(false);
				m_BatchRenderer->SetShake(false);
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
				//m_Scene.DrawGizmos();

				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glDisable(GL_CULL_FACE);
				glLineWidth(1.0f);
				attack->Draw(attack->model);
				glPolygonMode(GL_FRONT, GL_FILL);
				glEnable(GL_CULL_FACE);
				glCullFace(GL_BACK);
			}
		}

		virtual Scene* GetScene() override { return &m_Scene; }
	};

}

sixengine::Application* sixengine::CreateApplication()
{
	return new Game("App", 1280, 720);
}