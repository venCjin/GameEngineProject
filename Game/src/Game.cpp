#include <Engine.h>
#include <EntryPoint.h>
#include "Renderer/PrimitiveUtils.h"
#include "Core/Timer.h"


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
			std::vector<Vertex> vertices;
			std::vector<unsigned int> indices;

			PrimitiveUtils::GenerateSphere(vertices, indices, 30, 30);


			m_VAO->Bind();

			vbo = new VertexBuffer(vertices.data(), vertices.size()*sizeof(Vertex));
			vbo->SetLayout({
				{ VertexDataType::VEC3F, "position" }
				});
			ibo = new IndexBuffer(indices.data(), indices.size());

			m_VAO->AddVertexBuffer(*vbo);
			m_VAO->AddIndexBuffer(*ibo);



		}

		virtual void OnUpdate() override
		{			
			
			glm::mat4 projection = glm::perspective(glm::radians(1.0f), (float)1280 / (float)720, 0.1f, 1000.0f);
			glm::mat4 view = glm::lookAt(glm::vec3(0, 0, -90), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
			glm::mat4 model = glm::mat4(1);
			model = glm::rotate(model, glm::radians(1.0f), glm::vec3(0, 1.0f, 0));

			
			m_Shader->SetMat4("projection", projection);
			m_Shader->SetMat4("view", view);
			m_Shader->SetMat4("model", model);

			Renderer::Clear(0.3f, 0.3f, 0.3f);
			Renderer::Render(m_VAO, m_Shader);
		}
	};
}

sixengine::Application* sixengine::CreateApplication()
{
	return new Game("App", 1280, 720);
}