#include <Engine.h>
#include <EntryPoint.h>

namespace sixengine {
	class Game : public Application
	{
	private:
		VertexArray* m_VAO;
		Shader* m_Shader;
		Camera cam;
		float lastX = 0.0f, lastY = 0.0f;

	public:
		Game(std::string title, unsigned int width, unsigned int height)
			: Application(title, width, height)
		{
			m_VAO = new VertexArray();
			m_Shader = new Shader("res/shaders/TestShader.vert", "res/shaders/TestShader.frag");
			cam.SetProjectionMatrix(glm::perspective(glm::radians(cam.Zoom), (float)width / (float)height, cam.m_NearPlane, cam.m_FarPlane));
		}

		virtual void OnInit() override
		{
			VertexBuffer* vbo;
			IndexBuffer* ibo;

			std::array<Vertex, 4> vertices {
				Vertex{ 0.5f,  0.5f, 0.0f}, // top right
				Vertex{ 0.5f, -0.5f, 0.0f}, // bottom right
				Vertex{-0.5f, -0.5f, 0.0f}, // bottom left
				Vertex{-0.5f,  0.5f, 0.0f}  // top left 
			};

			std::array<unsigned int, 6> indices {
				0, 1, 3,
				1, 2, 3
			};

			m_VAO->Bind();

			vbo = new VertexBuffer(vertices.data(), vertices.size() * sizeof(Vertex));
			vbo->SetLayout({
				{ VertexDataType::VEC3F, "position" }
			});

			ibo = new IndexBuffer(indices.data(), indices.size());

			m_VAO->AddVertexBuffer(*vbo);
			m_VAO->AddIndexBuffer(*ibo);
		}

		virtual void OnUpdate() override
		{
			Renderer::Clear(0.3f, 0.3f, 0.3f);

			m_Shader->Bind();
			m_Shader->SetMat4("ProjViewMat", cam.GetProjectionMatrix() * cam.GetViewMatrix());

			Renderer::Render(m_VAO, m_Shader);
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
			switch (e.GetKeyCode()) 
			{
			case 'W':
				cam.ProcessKeyboard(CameraMovement::FORWARD, dt);
				break;
			case 'S':
				cam.ProcessKeyboard(CameraMovement::BACKWARD, dt);
				break;
			case 'A':
				cam.ProcessKeyboard(CameraMovement::LEFT, dt);
				break;
			case 'D':
				cam.ProcessKeyboard(CameraMovement::RIGHT, dt);
				break;
			case 340:   // GLFW_KEY_LEFT_SHIFT
				cam.ProcessKeyboard(CameraMovement::UP, dt);
				break;
			case 32:   // GLFW_KEY_SPACE
				cam.ProcessKeyboard(CameraMovement::DOWN, dt);
				break;
			default:
				break;
			}
			LOG_INFO("{0}", e);
			return true;
		}
	
		bool OnMouseMoved(MouseMovedEvent& e)
		{
			LOG_INFO("{0}", e);

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