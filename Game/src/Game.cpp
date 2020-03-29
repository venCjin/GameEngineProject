#include <Engine.h>
#include <EntryPoint.h>
#include "Renderer/PrimitiveUtils.h"
#include "Core/Timer.h"
#include "Core/ResourceManager.h"
#include <Core\ShaderManager.h>


namespace sixengine {

	
	class Game : public Application
	{
	private:
		GameObject* m_GameObjects[3];
		Shader* m_Shader;
		Camera cam;
		bool firstMouse = true;
		float lastX = 0.0f, lastY = 0.0f;
		ShaderManager* m_ShaderManager;
		Mesh* mesh;

	public:
		Game(std::string title, unsigned int width, unsigned int height)
			: Application(title, width, height)
		{
			m_ShaderManager = new ShaderManager();
			systems.add<RotationSystem>();
			systems.configure();

			m_Shader = m_ShaderManager->makeInstance("res/shaders/TestShader.vert", "res/shaders/TestShader.frag");

			float aspectRatio = (float)width / (float)height;
			
			//cam.SetProjectionMatrix(glm::perspective(glm::radians(cam.Zoom), (float)width / (float)height, cam.m_NearPlane, cam.m_FarPlane));
			
			cam.SetProjectionMatrix(glm::ortho(-1.0f * aspectRatio, 1.0f * aspectRatio, -1.0f, 1.0f, cam.m_NearPlane, cam.m_FarPlane));
		}

		virtual void OnInit() override
		{
			std::vector<Vertex> vertices;
			std::vector<unsigned int> indices;
			PrimitiveUtils::GenerateCube(vertices, indices);

			mesh = new Mesh(vertices, indices, nullptr);

			VertexArray* vao;
			VertexBuffer* vbo;
			IndexBuffer* ibo;


			// Setup First Object
			PrimitiveUtils::GenerateCube(vertices, indices);
			vbo = new VertexBuffer(vertices.data(), vertices.size() * sizeof(Vertex));
			vbo->SetLayout({
				{ VertexDataType::VEC3F, "position" },
				{ VertexDataType::VEC3F, "normal" },
				{ VertexDataType::VEC2F, "texcoord" },

				});

			ibo = new IndexBuffer(indices.data(), indices.size());

			vao = new VertexArray();
			vao->Bind();
			vao->AddVertexBuffer(*vbo);
			vao->AddIndexBuffer(*ibo);

			m_GameObjects[0] = new GameObject(entities);
			m_GameObjects[0]->AddComponent<TestTransform>(glm::translate(glm::mat4(1.0f), glm::vec3(-1.25f, 0.0f, 0.0f)));
			m_GameObjects[0]->AddComponent<TestMesh>(vao);
			
			// Setup Second Object
			/*PrimitiveUtils::GenerateSphere(vertices, indices, 30, 30);
			vbo = new VertexBuffer(vertices.data(), vertices.size() * sizeof(Vertex));
			vbo->SetLayout({
				{ VertexDataType::VEC3F, "position" }
			});

			ibo = new IndexBuffer(indices.data(), indices.size());

			vao = new VertexArray();
			vao->Bind();
			vao->AddVertexBuffer(*vbo);
			vao->AddIndexBuffer(*ibo);

			m_GameObjects[1] = new GameObject(entities);
			m_GameObjects[1]->AddComponent<TestTransform>(glm::translate(glm::mat4(1.0f), glm::vec3(1.25f, 0.0f, 0.0f)));
			m_GameObjects[1]->AddComponent<TestMesh>(vao);

			// Setup Third Object
			PrimitiveUtils::GenerateCapsule(vertices, indices, 20, 20);
			vbo = new VertexBuffer(vertices.data(), vertices.size() * sizeof(Vertex));
			vbo->SetLayout({
				{ VertexDataType::VEC3F, "position" }
			});

			ibo = new IndexBuffer(indices.data(), indices.size());

			vao = new VertexArray();
			vao->Bind();
			vao->AddVertexBuffer(*vbo);
			vao->AddIndexBuffer(*ibo);

			m_GameObjects[2] = new GameObject(entities);
			m_GameObjects[2]->AddComponent<TestTransform>(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)));
			m_GameObjects[2]->AddComponent<TestMesh>(vao);*/
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
				//mesh->Draw();



				entityx::ComponentHandle<TestTransform> transform;
				entityx::ComponentHandle<TestMesh> mesh;
				
				for (entityx::Entity entity : entities.entities_with_components(transform, mesh)) {
					transform = entity.component<TestTransform>();
					mesh = entity.component<TestMesh>();

					model = transform->transform;
					m_Shader->SetMat4("model", model);

					Renderer::Render(mesh->VAO, m_Shader);
				};
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