#include <Engine.h>
#include <EntryPoint.h>

#include "Renderer/PrimitiveUtils.h"
#include "Core/Timer.h"
#include "Core/ResourceManager.h"
#include "Core/ShaderManager.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/Components/Billboard.h"
#include "Gameplay/Systems/BillboardSystem.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

namespace sixengine {
	
	class Game : public Application
	{
	private:
		GameObject* m_SceneRoot;
		Shader* m_Shader, *m_BasicShader;
		Camera cam;
		ShaderManager* m_ShaderManager;
		Model m_Model;
		std::vector<glm::mat4> transforms;

	public:
		Game(std::string title, unsigned int width, unsigned int height)
			: Application(title, width, height)
		{
			m_ShaderManager = new ShaderManager();
			m_SystemManager.AddSystem<RotationSystem>();
			m_SystemManager.AddSystem<BillboardSystem>();

			m_Shader = m_ShaderManager->makeInstance("res/shaders/TestShader.vert", "res/shaders/TestShader.frag");
			m_BasicShader = m_ShaderManager->makeInstance("res/shaders/basic.vert", "res/shaders/basic.frag");
			float aspectRatio = (float)width / (float)height;
			cam.MakePerspective(aspectRatio);
		}

		virtual void OnInit() override
		{
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
			m_SceneRoot->AddComponent<TestMaterial>(m_BasicShader);
			
			// Setup Second (LEFT) Object
			PrimitiveUtils::GenerateQuad(vertices, indices);

			go = new GameObject(m_EntityManager);
			go->AddComponent<Transform>(go, glm::mat4(1.0f),
				glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, 0.0f)));
			go->AddComponent<TestMesh>(vertices, indices);
			go->AddComponent<TestMaterial>(m_BasicShader);
			go->AddComponent<Billboard>(&cam);

			m_SceneRoot->AddChild(go);

			// Setup Second (RIGHT) Object
			PrimitiveUtils::GenerateCapsule(vertices, indices);

			go = new GameObject(m_EntityManager);
			go->AddComponent<Transform>(go, glm::mat4(1.0f),
				glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 0.0f)));
			go->AddComponent<TestRotation>(glm::vec3(0.0f, 1.0f, 0.0f), 25.0f);
			go->AddComponent<TestMesh>(vertices, indices);
			go->AddComponent<TestMaterial>(m_BasicShader);

			m_SceneRoot->AddChild(go);

			glEnable(GL_DEPTH_TEST);
		}

		virtual void OnUpdate(float dt) override
		{		
			{
				PROFILE_SCOPE("UPDATE")
				m_SystemManager.UpdateAll(dt);
			}

			{
				Renderer::Clear(0.3f, 0.3f, 0.3f);

				float time = Application::GetTimer().ElapsedTime();
				m_Model.BoneTransform(time, transforms);

				glm::mat4 model = glm::mat4(1.0f);
				model = glm::scale(model, glm::vec3(0.01f));
				m_Shader->Bind();
				m_Shader->SetMat4("projection", cam.GetProjectionMatrix());
				m_Shader->SetMat4("view", cam.GetViewMatrix());
				m_Shader->SetMat4("model", model);

				for (int i = 0; i < transforms.size(); i++)
				{
					m_Shader->SetMat4("gBones[" + std::to_string(i) + "]", transforms[i]);
				}

				m_Model.Render(m_Shader);

				m_SceneRoot->Render(cam.GetProjectionMatrix(), cam.GetViewMatrix());
			}
		}

		virtual void OnEvent(Event& event) override
		{
			// Let Aplication handle it's own Events
			Application::OnEvent(event);

			// Game Events handle here
			//EventDispatcher dispatcher(event);
			//dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(Game::OnKeyPressed));
			//dispatcher.Dispatch<MouseMovedEvent>(BIND_EVENT_FN(Game::OnMouseMoved));
		}
	};

}

sixengine::Application* sixengine::CreateApplication()
{
	return new Game("App", 1280, 720);
}