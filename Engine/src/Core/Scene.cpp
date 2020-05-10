#include "pch.h"
#include "Scene.h"

#include "Core/Application.h"
#include "Core/ShaderManager.h"

#include "Gameplay/Systems/BillboardSystem.h"
#include "Gameplay/Systems/RotationSystem.h"
#include "Gameplay/Systems/UIRendererSystem.h"
#include "Gameplay/GameObject.h"

#include "Renderer/PrimitiveUtils.h"

#include "Physics/Components/Collider.h"
#include "Physics/Systems/CollisionSystem.h"

#include "Gameplay/Components/SimplePlayer.h"
#include "Gameplay/Systems/SimplePlayerSystem.h"

namespace sixengine {

	Scene::Scene(unsigned int width, unsigned int height) : m_UIRoot(nullptr), m_SceneRoot(nullptr)
	{
		m_ModelManager = new ModelManager();
		m_ShaderManager = new ShaderManager();
		m_MaterialManager = new MaterialManager();
		m_TextureArray = new TextureArray(2048, 2048);
		BatchRenderer::Initialize(m_ModelManager, m_TextureArray);
		m_BatchRenderer = BatchRenderer::Instance();

		cam.MakePerspective((float)width / (float)height);
		camUI.MakeOrtho(width, height);
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
		m_SceneRoot->AddComponent<Transform>(m_SceneRoot, glm::mat4(1.0f), glm::mat4(1.0f));

		m_UIRoot = new GameObject(*en);
		m_UIRoot->AddComponent<Transform>(m_UIRoot, glm::mat4(1.0f), glm::mat4(1.0f));


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
							&cam)
						);
				}
				else if (s == "AnimationPBR")
				{
					ss >> s;
					m_BatchRenderer->AddTechnique(
						new AnimationPBR(
							m_ShaderManager->AddShader(s),
							&cam)
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
				go->AddComponent<Transform>(go, l, w);
			}
			/*else if (s == "-TestMesh")
			{
				std::vector<Vertex> vertices;
				std::vector<unsigned int> indices;
				file >> s;
				if (s == "Quad") PrimitiveUtils::GenerateQuad(vertices, indices);
				else if (s == "Cube") PrimitiveUtils::GenerateCube(vertices, indices);
				else if (s == "Capsule") PrimitiveUtils::GenerateCapsule(vertices, indices);
				else if (s == "Sphere") PrimitiveUtils::GenerateSphere(vertices, indices);
				else PrimitiveUtils::GenerateCube(vertices, indices); //default cube
				//go->AddComponent<Mesh>(vertices, indices);
			}*/
			else if (s == "-Model")
			{
				file >> s;
				go->AddComponent<Mesh>(m_ModelManager->AddModel(s));
			}
			else if (s == "-Material")
			{
				file >> s;
				// TODO: Material adding here or in header
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
				go->AddComponent<Billboard>(&cam);
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
