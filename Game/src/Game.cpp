#include <Engine.h>
#include <EntryPoint.h>
#include "Renderer/PrimitiveUtils.h"
#include "Core/Timer.h"

namespace sixengine {
	
	class Game : public Application
	{
	private:
		GameObject* m_GameObjects[5];
		VertexArray* m_VAO;
		Shader* m_Shader;
		Camera cam;
		bool firstMouse = true;
		float lastX = 0.0f, lastY = 0.0f;

	public:
		Game(std::string title, unsigned int width, unsigned int height)
			: Application(title, width, height)
		{
			systems.add<RotationSystem>();
			systems.configure();

			m_Shader = new Shader("res/shaders/TestShader.vert", "res/shaders/TestShader.frag");

			float aspectRatio = (float)width / (float)height;
			
			cam.SetProjectionMatrix(glm::perspective(glm::radians(cam.Zoom), (float)width / (float)height, 0.01f, 1500.0f));
			
			//cam.SetProjectionMatrix(glm::ortho(-1.0f * aspectRatio, 1.0f * aspectRatio, -1.0f, 1.0f, cam.m_NearPlane, cam.m_FarPlane));
			cam.Position = glm::vec3(0.0f, 0.0f, 5.0f);
		}

		virtual void OnInit() override
		{
			std::vector<Vertex> vertices;
			std::vector<unsigned int> indices;

			std::vector<Vertex> verticesAll;
			std::vector<unsigned int> indicesAll;

			PrimitiveUtils::GenerateCube(vertices, indices);
			verticesAll.insert(verticesAll.end(), vertices.begin(), vertices.end());
			indicesAll.insert(indicesAll.end(), indices.begin(), indices.end());

			// Setup First Object
			m_GameObjects[0] = new GameObject(entities);
			m_GameObjects[0]->AddComponent<TestTransform>(glm::translate(glm::mat4(1.0f), glm::vec3(-1.5f, 0.0f, 0.0f)));
			m_GameObjects[0]->AddComponent<TestRotation>(glm::vec3(0.0f, 1.0f, 0.0f), -15.0f);
			
			m_GameObjects[1] = new GameObject(entities);
			m_GameObjects[1]->AddComponent<TestTransform>(glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, 0.0f, 0.0f)));
			m_GameObjects[1]->AddComponent<TestRotation>(glm::vec3(0.0f, 1.0f, 0.0f), 15.0f);

			PrimitiveUtils::GenerateCapsule(vertices, indices, 10, 10);
			verticesAll.insert(verticesAll.end(), vertices.begin(), vertices.end());
			indicesAll.insert(indicesAll.end(), indices.begin(), indices.end());

			// Setup Second Object
			m_GameObjects[2] = new GameObject(entities);
			m_GameObjects[2]->AddComponent<TestTransform>(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)));

			PrimitiveUtils::GenerateSphere(vertices, indices, 30, 30);
			verticesAll.insert(verticesAll.end(), vertices.begin(), vertices.end());
			indicesAll.insert(indicesAll.end(), indices.begin(), indices.end());

			// Setup Third Object
			m_GameObjects[3] = new GameObject(entities);
			m_GameObjects[3]->AddComponent<TestTransform>(glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, 0.0f, 0.0f)));
			m_GameObjects[3]->AddComponent<TestRotation>(glm::vec3(1.0f, 0.0f, 0.0f), 3.0f);
			
			m_GameObjects[4] = new GameObject(entities);
			m_GameObjects[4]->AddComponent<TestTransform>(glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 0.0f, 0.0f)));
			


			VertexBuffer* vbo = new VertexBuffer(verticesAll.data(), verticesAll.size() * sizeof(Vertex));
			vbo->SetLayout({
				{ VertexDataType::VEC3F, "position" }
			});

			IndexBuffer* ibo = new IndexBuffer(indicesAll.data(), indicesAll.size());

			m_VAO = new VertexArray();
			m_VAO->AddVertexBuffer(*vbo);
			m_VAO->AddIndexBuffer(*ibo);
		}

		virtual void OnUpdate(float dt) override
		{		
			{
				//PROFILE_SCOPE("UPDATE")
				systems.update_all(dt);
			}

			{
				PROFILE_SCOPE("RENDER")
				Renderer::Clear(0.3f, 0.3f, 0.3f);

				m_Shader->Bind();
				m_Shader->SetMat4("projection", cam.GetProjectionMatrix());
				m_Shader->SetMat4("view", cam.GetViewMatrix());
				
				entityx::ComponentHandle<TestTransform> transform;
				entityx::ComponentHandle<TestMesh> mesh;

				/*for (entityx::Entity entity : entities.entities_with_components(transform, mesh)) 
				{
					transform = entity.component<TestTransform>();
					mesh = entity.component<TestMesh>();

					Models.push_back(transform->transform);
					//m_Shader->SetMat4("model", model);

				}*/

				std::vector<glm::mat4> models;
				models.push_back(m_GameObjects[0]->GetComponent<TestTransform>()->transform);
				models.push_back(m_GameObjects[1]->GetComponent<TestTransform>()->transform);
				models.push_back(m_GameObjects[2]->GetComponent<TestTransform>()->transform);
				models.push_back(m_GameObjects[3]->GetComponent<TestTransform>()->transform);
				models.push_back(m_GameObjects[4]->GetComponent<TestTransform>()->transform);

				Renderer::Render(m_VAO, m_Shader, models);
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