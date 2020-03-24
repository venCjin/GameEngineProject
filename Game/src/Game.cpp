#include <Engine.h>
#include <EntryPoint.h>

namespace sixengine {
	class Game : public Application
	{
	private:
		VertexArray* m_VAO;
		Shader* m_Shader;

	public:
		Game(std::string title, unsigned int width, unsigned int height)
			: Application(title, width, height)
		{
			m_VAO = new VertexArray();
			m_Shader = new Shader("res/shaders/TestShader.vert", "res/shaders/TestShader.frag");
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
			Renderer::Render(m_VAO, m_Shader);
		}
	};
}

sixengine::Application* sixengine::CreateApplication()
{
	return new Game("App", 1280, 720);
}