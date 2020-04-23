#include "pch.h"
#include "Scene.h"
#include "Core/Application.h"
#include "Gameplay/Systems/BillboardSystem.h"
#include "Gameplay/Systems/RotationSystem.h"
#include "Gameplay/Systems/UIRendererSystem.h"
#include "Gameplay/GameObject.h"
#include "Renderer/PrimitiveUtils.h"
#include "Core/ShaderManager.h"

namespace sixengine {

	Scene::Scene(unsigned int width, unsigned int height) : m_UI(nullptr), m_Scene(nullptr)
	{
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
		///EventManager* ev = Application::Get().GetEventManager();
		SystemManager* sys = Application::Get().GetSystemManager();

		ShaderManager sm;

		m_Scene = new GameObject(*en);
		m_Scene->AddComponent<Transform>(m_Scene, glm::mat4(1.0f), glm::mat4(1.0f));

		m_UI = new GameObject(*en);
		m_UI->AddComponent<Transform>(m_UI, glm::mat4(1.0f), glm::mat4(1.0f));

		std::string s;
		//while (!file.eof())
		while (true)
		{
			file >> s;

			if (s == "-AddSystem")
			{
				file >> s;
				if (s == "BillboardSystem") sys->AddSystem<BillboardSystem>();
				else if (s == "RotationSystem") sys->AddSystem<RotationSystem>();
				else if (s == "UIRendererSystem") sys->AddSystem<UIRendererSystem>();
				else LOG_WARN("Not recognized system: {0}", s);
			}
			else if (s == "+SceneGameObject")
			{
				m_Scene->AddChild(ReadGameObject(file, *en, sm));
			}
			else if (s == "+UIGameObject")
			{
				m_UI->AddChild(ReadGameObject(file, *en, sm));
			}
			else if (s == "-eof")
			{
				break;
			}
			else
			{
				LOG_WARN("Not recognized command: {0}", s);
			}
		}
		file.close();
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

	void Scene::Render()
	{
		m_Scene->Render(cam.GetProjectionMatrix(), cam.GetViewMatrix());
		m_UI->Render(camUI.GetProjectionMatrix(), camUI.GetViewMatrix());
	}

	GameObject* Scene::ReadGameObject(std::fstream& file, EntityManager& en, ShaderManager& sm)
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
			else if (s == "-TestMesh")
			{
				std::vector<Vertex> vertices;
				std::vector<unsigned int> indices;
				file >> s;
				if (s == "Quad") PrimitiveUtils::GenerateQuad(vertices, indices);
				else if (s == "Cube") PrimitiveUtils::GenerateCube(vertices, indices);
				else if (s == "Capsule") PrimitiveUtils::GenerateCapsule(vertices, indices);
				else PrimitiveUtils::GenerateCube(vertices, indices); //default cube
				go->AddComponent<TestMesh>(vertices, indices);
			}
			else if (s == "-Model")
			{
				file >> s;
				go->AddComponent<Model>(s);
			}
			else if (s == "-TestMaterial")
			{
				std::string vert, frag, tex;
				file >> vert;
				file >> frag;
				file >> tex;
				go->AddComponent<TestMaterial>(sm.makeInstance(vert, frag), tex);
			}
			else if (s == "-TestRotation")
			{
				float x, y, z, speed;
				file >> x;
				file >> y;
				file >> z;
				file >> speed;
				go->AddComponent<TestRotation>(glm::vec3(x, y, z), speed);
			}
			else if (s == "-Billboard")
			{
				go->AddComponent<Billboard>(&cam);
			}
			else LOG_WARN("Not recognized scene component: {0}", s);
		} while (true);
		return go;
	}
}
