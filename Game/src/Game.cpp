#include <Engine.h>
#include <EntryPoint.h>
#include "Renderer/PrimitiveUtils.h"
#include "Core/Timer.h"
#include "Core/ResourceManager.h"
#include <Core/ShaderManager.h>
#include "Gameplay/GameObject.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
namespace sixengine {
	
	class Game : public Application
	{
	private:
		GameObject* m_SceneRoot;
		Shader* m_Shader;
		Camera cam;
		bool firstMouse = true;
		float lastX = 0.0f, lastY = 0.0f;
		ShaderManager* m_ShaderManager;
		Mesh* mesh;
		Model *nanosuitModel;

	public:
		Game(std::string title, unsigned int width, unsigned int height)
			: Application(title, width, height)
		{
			m_ShaderManager = new ShaderManager();
			systems.add<RotationSystem>();
			systems.configure();

			m_Shader = m_ShaderManager->makeInstance("res/shaders/TestShader.vert", "res/shaders/TestShader.frag");

			float aspectRatio = (float)width / (float)height;
			cam.MakePerspective(aspectRatio);
		}

		virtual void OnInit() override
		{
			nanosuitModel = new Model("res/models/nanosuit/nanosuit.obj");

			std::vector<Vertex> vertices;
			std::vector<unsigned int> indices;

			PrimitiveUtils::GenerateCube(vertices, indices);

			mesh = new Mesh(vertices, indices);

			GameObject* go;

			// Setup First (CENTER) Object
			PrimitiveUtils::GenerateCube(vertices, indices);

			m_SceneRoot = new GameObject(entities);
			m_SceneRoot->AddComponent<Transform>(m_SceneRoot);
			m_SceneRoot->AddComponent<TestRotation>(glm::vec3(0.0f, 1.0f, 0.0f), 25.0f);
			m_SceneRoot->AddComponent<TestMesh>(vertices, indices);
			m_SceneRoot->AddComponent<TestMaterial>(m_Shader);
			
			// Setup Second (LEFT) Object
			PrimitiveUtils::GenerateSphere(vertices, indices);

			go = new GameObject(entities);
			go->AddComponent<Transform>(go, glm::mat4(1.0f),
				glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, 0.0f)));
			go->AddComponent<TestMesh>(vertices, indices);
			go->AddComponent<TestMaterial>(m_Shader);

			m_SceneRoot->AddChild(go);

			// Setup Second (RIGHT) Object
			PrimitiveUtils::GenerateCapsule(vertices, indices);

			go = new GameObject(entities);
			go->AddComponent<Transform>(go, glm::mat4(1.0f),
				glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 0.0f)));
			go->AddComponent<TestMesh>(vertices, indices);
			go->AddComponent<TestMaterial>(m_Shader);

			m_SceneRoot->AddChild(go);

			glEnable(GL_DEPTH_TEST);

		}

		virtual void OnUpdate(float dt) override
		{		
			{
				//PROFILE_SCOPE("UPDATE")
				systems.update_all(dt);
			}

			{

				//PROFILE_SCOPE("RENDER")
				Renderer::Clear(0.3f, 0.3f, 0.3f);

				glm::mat4 model = glm::mat4(1.0f);
				m_Shader->Bind();
				m_Shader->SetMat4("projection", cam.GetProjectionMatrix());
				m_Shader->SetMat4("view", cam.GetViewMatrix());
				m_Shader->SetMat4("model", model);
				nanosuitModel->Draw(m_Shader);
				//mesh->Draw();

				//m_SceneRoot->Render(cam.GetProjectionMatrix(), cam.GetViewMatrix());
			}
		}

		virtual void OnEvent(Event& event) override
		{
			// Let Aplication handle it's own Events
			Application::OnEvent(event);

			// Game Events handle here
			EventDispatcher dispatcher(event);
			dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(Game::OnKeyPressed));
			dispatcher.Dispatch<MouseMovedEvent>(BIND_EVENT_FN(Game::OnMouseMoved));
		}

		bool OnKeyPressed(KeyPressedEvent& e)
		{
			LOG_INFO("{0}", e);

			float dt = Application::GetTimer().DeltaTime();

			// key codes stolen from glfw3.h
			// this should be access through Input.h
			int key = e.GetKeyCode();
			if (key == 'W') cam.ProcessKeyboard(CameraMovement::FORWARD, dt);
			if (key == 'S') cam.ProcessKeyboard(CameraMovement::BACKWARD, dt);
			if (key == 'A') cam.ProcessKeyboard(CameraMovement::LEFT, dt);
			if (key == 'D') cam.ProcessKeyboard(CameraMovement::RIGHT, dt);
			if (key == 32)	cam.ProcessKeyboard(CameraMovement::UP, dt);   // GLFW_KEY_LEFT_SHIFT
			if (key == 340)  cam.ProcessKeyboard(CameraMovement::DOWN, dt); // GLFW_KEY_SPACE
			if (key == 258)  GetWindow().SwitchCursorVisibility(); // GLFW_KEY_LEFT_CTRL

			LOG_INFO("{0}", e);
			return true;
		}
	
		bool OnMouseMoved(MouseMovedEvent& e)
		{
			LOG_INFO("{0}", e);
			if (firstMouse)
			{
				lastX = e.GetX();
				lastY = e.GetY();
				firstMouse = false;
			}

			float xoffset = e.GetX() - lastX;
			float yoffset = lastY - e.GetY(); // reversed since y-coordinates go from bottom to top

			lastX = e.GetX();
			lastY = e.GetY();

			cam.ProcessMouseMovement(xoffset, yoffset);
			return true;
		}
};
}

sixengine::Application* sixengine::CreateApplication()
{
	return new Game("App", 1280, 720);
}