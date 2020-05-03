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
#include "Renderer/ModelManager.h"
#include "Renderer/TextureArray.h"
#include "Renderer/PrimitiveUtils.h"


namespace sixengine {
	
	class Game : public Application
	{
	private:
		Scene m_Scene;
		GameObject *obj1, *obj2, *obj3;
		GameObject* m_SceneRoot, * m_UIRoot;
		Shader* m_Shader, * m_BasicShader, * m_UIShader;
		Camera cam, camUI;
		ShaderManager* m_ShaderManager;
		TextureArray* m_TextureManager;
		Model m_Model;
		std::vector<glm::mat4> transforms;

	public:
		Game(std::string title, unsigned int width, unsigned int height)
			: Application(title, width, height), m_Scene(width, height)
		{
			m_TextureManager = new TextureArray(2048, 2048);
			m_ShaderManager = new ShaderManager();
			m_SystemManager.AddSystem<RotationSystem>();
			m_Shader = m_ShaderManager->makeInstance("res/shaders/TestShader.vert", "res/shaders/TestShader.frag");
			m_BasicShader = m_ShaderManager->makeInstance("res/shaders/basic.vert", "res/shaders/basic.frag");
			m_UIShader = m_ShaderManager->makeInstance("res/shaders/UIShader.vert", "res/shaders/UIShader.frag");
			float aspectRatio = (float)width / (float)height;
			cam.MakePerspective(aspectRatio);
			camUI.MakeOrtho(width, height);
		}
		
		~Game()
		{}

		virtual void OnInit() override
		{
			m_TextureManager->AddTexture("res/textures/test/Bricks.jpg");
			m_TextureManager->AddTexture("res/textures/test/Wood1.jpg");
			m_TextureManager->AddTexture("res/textures/test/Wood2.jpg");
			m_TextureManager->CreateTextureArray();

			std::vector<Vertex> vertices;
			std::vector<unsigned int> indices;

			// Setup First (CENTER) Object
			PrimitiveUtils::GenerateCube(vertices, indices);

			obj1 = new GameObject(m_EntityManager);
			obj1->AddComponent<Transform>(obj1);
			obj1->AddComponent<TestMesh>(vertices, indices);
			obj1->AddComponent<Material>(m_BasicShader, m_TextureManager->GetTexture("Bricks"));
			
			// Setup Second (LEFT) Object2
			PrimitiveUtils::GenerateCube(vertices, indices);

			obj2 = new GameObject(m_EntityManager);
			obj2->AddComponent<Transform>(obj2, glm::mat4(1.0f),
				glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, 0.0f)));
			obj2->AddComponent<TestMesh>(vertices, indices);
			obj2->AddComponent<Material>(m_BasicShader, m_TextureManager->GetTexture("Wood1"));

			// Setup Second (RIGHT) Object
			PrimitiveUtils::GenerateCube(vertices, indices);

			obj3 = new GameObject(m_EntityManager);
			obj3->AddComponent<Transform>(obj3, glm::mat4(1.0f),
				glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 0.0f)));
			obj3->AddComponent<TestRotation>(glm::vec3(0.0f, 1.0f, 0.0f), 25.0f);
			obj3->AddComponent<TestMesh>(vertices, indices);
			obj3->AddComponent<Material>(m_BasicShader, m_TextureManager->GetTexture("Wood2"));

			glEnable(GL_DEPTH_TEST);


			m_BasicShader->Bind();

			m_TextureManager->Bind();
			m_BasicShader->SetInt("textureArray", 0);

			m_BasicShader->SetMat4("projection", cam.GetProjectionMatrix());
		}

		virtual void OnUpdate(float dt) override
		{		
			{
				//PROFILE_SCOPE("UPDATE")
				m_SystemManager.UpdateAll(dt);
			}
			
			{
				PROFILE_SCOPE("RENDER")
				Renderer::Clear(0.3f, 0.3f, 0.3f);

				m_BasicShader->SetMat4("view", cam.GetViewMatrix());

				m_BasicShader->SetMat4("model", obj1->GetComponent<Transform>()->GetWorld());
				m_BasicShader->SetFloat("layer", obj1->GetComponent<Material>()->GetTexture());
				Renderer::Render(obj1->GetComponent<TestMesh>()->VAO);

				m_BasicShader->SetMat4("model", obj2->GetComponent<Transform>()->GetWorld());
				m_BasicShader->SetFloat("layer", obj2->GetComponent<Material>()->GetTexture());
				Renderer::Render(obj2->GetComponent<TestMesh>()->VAO);
			
				m_BasicShader->SetMat4("model", obj3->GetComponent<Transform>()->GetWorld());
				m_BasicShader->SetFloat("layer", obj3->GetComponent<Material>()->GetTexture());
				Renderer::Render(obj3->GetComponent<TestMesh>()->VAO);
			}

		}
	};

}

sixengine::Application* sixengine::CreateApplication()
{
	return new Game("App", 1280, 720);
}