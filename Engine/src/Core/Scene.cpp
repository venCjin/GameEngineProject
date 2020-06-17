#include "pch.h"
#include "Scene.h"

#include "Core/Application.h"
#include "Core/ShaderManager.h"

#include "Gameplay/Systems/BillboardSystem.h"
#include "Gameplay/Systems/RotationSystem.h"
#include "Gameplay/Systems/UIRendererSystem.h"
#include "Gameplay/GameObject.h"

#include "Renderer/PrimitiveUtils.h"
#include "Renderer/Gizmo.h"

#include "Renderer/Techniques/AnimationPBR.h"
#include "Renderer/Techniques/StaticPBR.h"

#include "Gameplay/Components/DirectionalLight.h"
#include "Gameplay/Components/PointLight.h"
#include "Gameplay/Components/SpotLight.h"

#include "Physics/Components/Collider.h"
#include "Physics/Systems/CollisionSystem.h"

#include "Gameplay/Components/SimplePlayer.h"
#include "Gameplay/Systems/SimplePlayerSystem.h"

#include "Core/CameraSystem/FlyingCamera.h"
#include <Core/CameraSystem/FlyingCameraSystem.h>
#include "Core/CameraSystem/FollowCamera.h"
#include <Core/CameraSystem/FollowCameraSystem.h>
#include "Core/CameraSystem/MixingCamera.h"
#include <Core/CameraSystem/MixingCameraSystem.h>
#include "Core/CameraSystem/OrbitalCamera.h"
#include <Core/CameraSystem/OrbitalCameraSystem.h>

#include <AI/EnemiesManager.h>
#include <AI/StateMachineSystem.h>
#include <AI/States/IdleState.h>
#include <AI/States/PatrolState.h>
#include <AI/States/AttackState.h>
#include <AI/States/SearchState.h>
#include "AI/NavMesh/NavAgent.h"
#include "AI/NavMesh/NavAgentSystem.h"

#define LOAD(COMPONENT)								\
{													\
	if (s == "-"#COMPONENT)							\
	{												\
		go->AddComponent<COMPONENT>(go);			\
		go->GetComponent<COMPONENT>()->Load(file);	\
		continue;									\
	}												\
}

namespace sixengine {

	Scene::Scene(unsigned int width, unsigned int height) : m_UIRoot(nullptr), m_SceneRoot(nullptr)
	{
		m_ModelManager = new ModelManager();
		m_ShaderManager = new ShaderManager();
		m_MaterialManager = new MaterialManager();
		m_TextureArray = new TextureArray(2048, 2048);
		BatchRenderer::Initialize(m_ModelManager, m_TextureArray);
		m_BatchRenderer = BatchRenderer::Instance();

		m_SceneRoot = new GameObject(*Application::Get().GetEntityManager());
		m_SceneRoot->AddComponent<Transform>(m_SceneRoot);

		m_UIRoot = new GameObject(*Application::Get().GetEntityManager());
		m_UIRoot->AddComponent<Transform>(m_UIRoot);

		GameObject* go = new GameObject(*Application::Get().GetEntityManager());
		go->AddComponent<Transform>(go);
		go->AddComponent<Camera>(go);
		go->GetComponent<Camera>()->SetPerspective((float)width / (float)height);
		go->AddComponent<FlyingCamera>();

		Camera::ActiveCamera = go->GetComponent<Camera>().Get();
	}

	Scene::~Scene()
	{
	}

	bool Scene::LoadScene(const char* filePath)
	{
		std::fstream file;
		file.open(filePath, std::ios::in);
		if (!file.good())
		{
			LOG_FATAL("Cannot open scene file: {0}", filePath);
			return false;
		}
		LOG_TRACE("Loading scene from file: {0}", filePath);
		

		EntityManager* en = Application::Get().GetEntityManager();
		//EventManager* ev = Application::Get().GetEventManager();
		SystemManager* sys = Application::Get().GetSystemManager();

		std::string line, s;
		std::getline(file, line);
		std::stringstream ss(line);
		
		while (file)
		{
			std::getline(ss, s, ' ');
			if (s == "-AddShader")
			{
				ss >> s;
				if (s == "StaticPBR")
				{
					ss >> s;
					m_BatchRenderer->AddTechnique(
						new StaticPBR(
							m_ShaderManager->AddShader(s)
						));
				}
				else if (s == "AnimationPBR")
				{
					ss >> s;
					m_BatchRenderer->AddTechnique(
						new AnimationPBR(
							m_ShaderManager->AddShader(s)
							));
				}
				else
				{
					LOG_ERROR("Cannot read shader or technique in line: {0}", line);
				}
			}
			else if (s == "-AddMaterial")
			{
				std::string materialName, shaderName;
				ss >> materialName;
				ss >> shaderName;
				
				int count;
				ss >> count;

				glm::vec4 tex(0.0f);
				for (int i = 0; i < count; i++)
				{
					ss >> s;
					tex[i] = m_TextureArray->AddTexture(s);
				}

				m_MaterialManager->CreateMaterial(
					m_ShaderManager->Get(shaderName),
					tex,
					materialName
					);
			}
			else if (s == "-AddSystem")
			{
				ss >> s;
				if (s == "BillboardSystem") sys->AddSystem<BillboardSystem>();
				else if (s == "RotationSystem") sys->AddSystem<RotationSystem>();
				else if (s == "UIRendererSystem") sys->AddSystem<UIRendererSystem>();
				else if (s == "SimplePlayerSystem") sys->AddSystem<SimplePlayerSystem>();
				else if (s == "CollisionSystem") sys->AddSystem<CollisionSystem>();
				else if (s == "StateMachineSystem") sys->AddSystem<StateMachineSystem>();
				else if (s == "FlyingCameraSystem") sys->AddSystem<FlyingCameraSystem>();
				else if (s == "FollowCameraSystem") sys->AddSystem<FollowCameraSystem>();
				else if (s == "MixingCameraSystem") sys->AddSystem<MixingCameraSystem>();
				else if (s == "OrbitalCameraSystem") sys->AddSystem<OrbitalCameraSystem>();
				else LOG_WARN("Not recognized system: {0}", s);
			}
			else if (s == "+SceneGameObject")
			{
				m_SceneRoot->AddChild(ReadGameObject(file, *en));
			}
			else if (s == "+UIGameObject")
			{
				m_UIRoot->AddChild(ReadGameObject(file, *en));
			}
			else if (!s.empty())
			{
				LOG_WARN("Not recognized command: {0}", s);
			}

			std::getline(file, line);
			ss.str(line);
			ss.clear();
		}
		file.close();

		//m_TextureArray->CreateTextureArray();
		//m_ModelManager->CreateVAO();

		return true;
	}

	bool Scene::SaveScene(const char* filePath)
	{
		std::fstream file;
		file.open(filePath, std::ios::out);
		if (!file.good()) return nullptr;

		file << "saved!";

		file.close();
		return true;
	}

	void Scene::Render(bool first)
	{
		m_SceneRoot->Render(first);
		m_UIRoot->Render(first);
	}

	void Scene::DrawGizmos()
	{
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_CULL_FACE);
		glLineWidth(5.0f);

		Shader* s = m_ShaderManager->Get("Gizmo");
		if (s)
		{
			s->Bind();
			s->SetMat4("projection", Camera::ActiveCamera->GetProjectionMatrix());
			s->SetMat4("view", Camera::ActiveCamera->GetViewMatrix());
			m_SceneRoot->OnDrawGizmos(true);
		}

		glPolygonMode(GL_FRONT, GL_FILL);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}

	GameObject* Scene::ReadGameObject(std::fstream& file, EntityManager& en)
	{
		std::string s;
		GameObject* go = new GameObject(en);
		do {
			file >> s;
			if (s == "+end") break;
			
			LOAD(Transform);

			/*LOAD(SpotLight);

			LOAD(DirectionalLight);

			LOAD(PointLight);*/

			if (s == "-Gizmo")
			{
				Gizmo* gizmo = new Gizmo(m_ShaderManager->AddShader("res/shaders/Gizmo.glsl"));
				gizmo->Load(file);
				go->AddGizmo(gizmo);
				continue;
			}

			else if (s == "-Model")
			{
				file >> s;
				go->AddComponent<Mesh>(m_ModelManager->AddModel(s));
				continue;
			}

			else if (s == "-Material")
			{
				file >> s;
				go->AddComponent<Material>(*m_MaterialManager->Get(s));
				continue;
			}

			else if (s == "-Enemy")
			{
				go->AddComponent<BoxCollider>(glm::vec3(1.0f, 1.0f, 1.0f));
				go->AddComponent<Mesh>(m_ModelManager->AddModel("res/models/primitives/cylinder.obj"));
				go->AddComponent<Material>(*m_MaterialManager->Get("JaJebe"));
				go->AddComponent<DynamicBody>();

				GameObject* child = new GameObject(en);
				child->AddComponent<Transform>(child);
				child->GetComponent<Transform>()->SetParent(go->GetComponent<Transform>().Get());
				child->GetComponent<Transform>()->SetLocalPosition(glm::vec3(0.0f, 0.6f, -0.75f));
				child->GetComponent<Transform>()->SetLocalScale(glm::vec3(1.0f, 0.2f, 1.0f));
				child->AddComponent<Mesh>(m_ModelManager->AddModel("res/models/primitives/cylinder.obj"));
				child->AddComponent<Material>(*m_MaterialManager->Get("JaJebe"));

				go->AddComponent<Enemy>(go);
				go->AddComponent<StateMachine>();
				go->AddComponent<NavAgent>(go);
				go->GetComponent<StateMachine>()->m_States.push_back(new AttackState(go));
				go->GetComponent<StateMachine>()->m_States.push_back(new SearchState(go));
				go->GetComponent<StateMachine>()->m_States.push_back(new IdleState(go));

				go->GetComponent<StateMachine>()->m_CurrentState = go->GetComponent<StateMachine>()->m_States[0];

				m_SceneRoot->AddChild(child);
			}
			
			LOAD(Rotation)
			
			LOAD(Billboard);
			
			LOAD(SimplePlayer);
			
			LOAD(BoxCollider);
			
			LOAD(SphereCollider);

			LOG_WARN("Not recognized scene component: {0}", s);
		} while (true);
		return go;
	}
}
