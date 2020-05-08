#include <Engine.h>
#include <EntryPoint.h>

/*#include "Renderer/PrimitiveUtils.h"
#include "Core/Timer.h"
#include "Core/ResourceManager.h"
#include "Core/ShaderManager.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/Components/Billboard.h"
#include "Gameplay/Systems/BillboardSystem.h"
#include <Gameplay\Components\UIElement.h>
#include <Gameplay\Systems\UIRendererSystem.h>*/
#include "Core/Scene.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

namespace sixengine {
	
	class Game : public Application
	{
	private:
		Scene m_Scene;

	public:
		Game(std::string title, unsigned int width, unsigned int height)
			: Application(title, width, height), m_Scene(width, height)
		{
		}
		
		~Game()
		{}

		virtual void OnInit() override
		{
			m_Scene.LoadScene("res/scenes/test.scene");
/*
			m_Model.LoadModel("res/models/par/par.dae");
			m_Model.BoneTransform(0.0f, transforms);

			std::vector<Vertex> vertices;
			std::vector<unsigned int> indices;

			GameObject* go;

			// Setup First (CENTER) Object
			PrimitiveUtils::GenerateCube(vertices, indices);

			m_SceneRoot = new GameObject(m_EntityManager);
			m_SceneRoot->AddComponent<Transform>(m_SceneRoot);
			m_SceneRoot->AddComponent<TestMesh>(vertices, indices);
			m_SceneRoot->AddComponent<TestMaterial>(m_BasicShader, "res/textures/floor/albedo.png");
			
			// Setup Second (LEFT) Object
			PrimitiveUtils::GenerateQuad(vertices, indices);

			go = new GameObject(m_EntityManager);
			go->AddComponent<Transform>(go, glm::mat4(1.0f),
				glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, 0.0f)));
			go->AddComponent<TestMesh>(vertices, indices);
			go->AddComponent<TestMaterial>(m_BasicShader, "res/textures/floor/albedo.png");
			go->AddComponent<Billboard>(&cam);

			m_SceneRoot->AddChild(go);

			// Setup Second (RIGHT) Object
			PrimitiveUtils::GenerateCapsule(vertices, indices);

			go = new GameObject(m_EntityManager);
			go->AddComponent<Transform>(go, glm::mat4(1.0f),
				glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 0.0f)));
			go->AddComponent<TestRotation>(glm::vec3(0.0f, 1.0f, 0.0f), 25.0f);
			go->AddComponent<TestMesh>(vertices, indices);
			go->AddComponent<TestMaterial>(m_UIShader, "res/textures/floor/albedo.png");

			m_SceneRoot->AddChild(go);
			



			//UI Scene Setup
			PrimitiveUtils::GenerateQuad(vertices, indices);
			m_UIRoot = new GameObject(m_EntityManager);
			glm::mat4 m(1.0f);
			m = glm::translate(m, glm::vec3(200.0f, 200.0f, 0.f));
			m = glm::scale(m, glm::vec3(300.0f, 300.0f, 1.0f));
			m_UIRoot->AddComponent<Transform>(go, glm::mat4(1.0f), m);
			m_UIRoot->AddComponent<TestMesh>(vertices, indices);
			m_UIRoot->AddComponent<TestMaterial>(m_UIShader, "res/textures/floor/albedo.png");
*/
			glEnable(GL_DEPTH_TEST);
		}

		virtual void OnUpdate(float dt) override
		{		
			{
				//PROFILE_SCOPE("UPDATE")
				m_SystemManager.UpdateAll(dt);
			}

			{
				//PROFILE_SCOPE("RENDER")
				Renderer::Clear(0.3f, 0.3f, 0.3f);
				
				m_Scene.Render();
			}
		}
	};

}

sixengine::Application* sixengine::CreateApplication()
{
	return new Game("App", 1280, 720);
}