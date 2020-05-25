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

#include "Gameplay/StateMachine/State.h"
#include "Gameplay/StateMachine/StateMachineSystem.h"

namespace sixengine {

	Scene::Scene(unsigned int width, unsigned int height) : m_UIRoot(nullptr), m_SceneRoot(nullptr)
	{
		m_ModelManager = new ModelManager();
		m_ShaderManager = new ShaderManager();
		m_MaterialManager = new MaterialManager();
		m_TextureArray = new TextureArray(2048, 2048);
		BatchRenderer::Initialize(m_ModelManager, m_TextureArray);
		m_BatchRenderer = BatchRenderer::Instance();

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

		m_SceneRoot = new GameObject(*en);
		m_SceneRoot->AddComponent<Transform>(m_SceneRoot);

		m_UIRoot = new GameObject(*en);
		m_UIRoot->AddComponent<Transform>(m_UIRoot);


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
							m_ShaderManager->AddShader(s),
							Camera::ActiveCamera)
						);
				}
				else if (s == "AnimationPBR")
				{
					ss >> s;
					m_BatchRenderer->AddTechnique(
						new AnimationPBR(
							m_ShaderManager->AddShader(s),
							Camera::ActiveCamera)
						);
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

		m_TextureArray->CreateTextureArray();
		m_ModelManager->CreateVAO();

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
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//m_SceneRoot->Render(cam.GetProjectionMatrix(), cam.GetViewMatrix());
		m_SceneRoot->Render(first);

		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		//m_UI->Render(camUI.GetProjectionMatrix(), camUI.GetViewMatrix());
		//m_UIRoot->Render(first);
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
			else if (s == "+Transform")
			{
				glm::mat4 l(1.0f), w(1.0f);
				do {
					file >> s;
					if (s == "+end") break;
					else if (s == "+local")
					{
						do {
							file >> s;
							if (s == "+end") break;
							else if (s == "-translate")
							{
								float x, y, z;
								file >> x;
								file >> y;
								file >> z;
								l = glm::translate(l, glm::vec3(x, y, z));
							}
							else if (s == "-rotate")
							{
								float x, y, z, degrees;
								file >> x;
								file >> y;
								file >> z;
								file >> degrees;
								l = glm::rotate(l, glm::radians(degrees), glm::vec3(x, y, z));
							}
							else if (s == "-scale")
							{
								float x, y, z;
								file >> x;
								file >> y;
								file >> z;
								l = glm::scale(l, glm::vec3(x, y, z));
							}
						} while (true);
					}
					else if (s == "+world")
					{
						do {
							file >> s;
							if (s == "+end") break;
							else if (s == "-translate")
							{
								float x, y, z;
								file >> x;
								file >> y;
								file >> z;
								w = glm::translate(w, glm::vec3(x, y, z));
							}
							else if (s == "-rotate")
							{
								float x, y, z, degrees;
								file >> x;
								file >> y;
								file >> z;
								file >> degrees;
								w = glm::rotate(w, glm::radians(degrees), glm::vec3(x, y, z));
							}
							else if (s == "-scale")
							{
								float x, y, z;
								file >> x;
								file >> y;
								file >> z;
								w = glm::scale(w, glm::vec3(x, y, z));
							}
						} while (true);
					}
				} while (true);
				go->AddComponent<Transform>(go);
				go->GetComponent<Transform>()->SetLocalMatrix(l * w);
			}
			else if (s == "-Gizmo")
			{
				float r, g, b;
				file >> r >> g >> b;

				std::vector<GizmoVertex> vertices;
				std::vector<unsigned int> indices;
				file >> s;
				if (s == "Quad")
				{
					PrimitiveUtils::GenerateQuad(vertices, indices);
				}
				else if (s == "Cube")
				{
					float x, y, z;
					file >> x >> y >> z;
					PrimitiveUtils::GenerateBox(vertices, indices, x, y, z);
				}
				else if (s == "Capsule")
				{
					PrimitiveUtils::GenerateCapsule(vertices, indices);
				}
				else if (s == "Sphere")
				{
					float radius;
					file >> radius;
					PrimitiveUtils::GenerateSphere(vertices, indices, radius);
				}
				else if (s == "Line")
				{
					float x, y, z;
					file >> x >> y >> z;
					vertices.emplace_back(GizmoVertex{ glm::vec3{ x, y, z } });
					vertices.emplace_back(GizmoVertex{ glm::vec3{ x, y, z } });
					file >> x >> y >> z;
					vertices.emplace_back(GizmoVertex{ glm::vec3{ x, y, z } });
					indices.push_back(0);
					indices.push_back(1);
					indices.push_back(2);
				}
				VertexArray* vao = new VertexArray();
				VertexBuffer* vbo = new VertexBuffer(&vertices[0], vertices.size());
				vbo->SetLayout({
					{ VertexDataType::VEC3F, "Position" }
				});
				IndexBuffer* ibo = new IndexBuffer(&indices[0], indices.size());
				vao->AddVertexBuffer(*vbo);
				vao->AddIndexBuffer(*ibo);

				go->AddGizmo(new Gizmo(vao, m_ShaderManager->AddShader("res/shaders/Gizmo.glsl"), glm::vec3(r, g, b)));
			}
			else if (s == "-Model")
			{
				file >> s;
				go->AddComponent<Mesh>(m_ModelManager->AddModel(s));
			}
			else if (s == "-Material")
			{
				file >> s;
				go->AddComponent<Material>(*m_MaterialManager->Get(s));
			}
			else if (s == "-Rotation")
			{
				float x, y, z, speed;
				file >> x;
				file >> y;
				file >> z;
				file >> speed;
				go->AddComponent<Rotation>(glm::vec3(x, y, z), speed);
			}
			else if (s == "-Billboard")
			{
				go->AddComponent<Billboard>(Camera::ActiveCamera);
			}
			else if (s == "-SimplePlayer")
			{
				go->AddComponent<SimplePlayer>();
			}
			else if (s == "-BoxCollider")
			{
				float x, y, z;
				file >> x;
				file >> y;
				file >> z;

				bool isStatic;
				file >> isStatic;

				go->AddComponent<BoxCollider>(glm::vec3(x, y, z), isStatic);
			}
			else if (s == "-SphereCollider")
			{
				float radius;
				file >> radius;

				bool isStatic;
				file >> isStatic;

				go->AddComponent<SphereCollider>(radius, isStatic);
			}
			else LOG_WARN("Not recognized scene component: {0}", s);
		} while (true);
		return go;
	}
}
