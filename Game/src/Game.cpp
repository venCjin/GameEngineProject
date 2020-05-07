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

#include "Renderer/BatchRenderer.h"
#include "Renderer/ModelManager.h"
#include "Renderer/TextureArray.h"
#include "Renderer/Material.h"


namespace sixengine {
	
	class Game : public Application
	{
	private:
		Scene m_Scene;
		GameObject *obj1, *obj2, *obj3;
		GameObject* objects[100][100];
		GameObject *m_SceneRoot, *m_UIRoot;
		Shader *m_BasicShader, *m_BasicShader2;
		Camera cam, camUI;
		std::vector<glm::mat4> transforms;

		ShaderManager* m_ShaderManager;
		ModelManager* m_ModelManager;
		TextureArray* m_TextureArray;
		BatchRenderer* m_BatchRenderer;

	public:
		Game(std::string title, unsigned int width, unsigned int height)
			: Application(title, width, height), m_Scene(width, height)
		{
			m_TextureArray = new TextureArray(2048, 2048);
			m_ModelManager = new ModelManager();
			m_ShaderManager = new ShaderManager();

			m_SystemManager.AddSystem<RotationSystem>();

			float aspectRatio = (float)width / (float)height;
			cam.MakePerspective(aspectRatio);
			camUI.MakeOrtho(width, height);

			BatchRenderer::Initialize(m_ModelManager, m_TextureArray, &cam);
			m_BatchRenderer = BatchRenderer::Instance();
		}
		
		~Game()
		{}

		virtual void OnInit() override
		{
			m_TextureArray->AddTexture("res/textures/test/Bricks.jpg");
			m_TextureArray->AddTexture("res/textures/test/Wood1.jpg");
			m_TextureArray->AddTexture("res/textures/test/Wood2.jpg");
			m_TextureArray->CreateTextureArray();

			std::map<unsigned int, std::string> randomT;
			randomT[0] = "Bricks";
			randomT[1] = "Wood1";
			randomT[2] = "Wood2";

			std::map<unsigned int, std::string> randomM;
			randomM[0] = "cube";
			randomM[1] = "sphere";
			randomM[2] = "cone";
			randomM[3] = "cylinder";
			randomM[4] = "teapot";

			//m_Shader = m_ShaderManager->AddShader("res/shaders/TestShader.vert"");
			//m_UIShader = m_ShaderManager->AddShader("res/shaders/UIShader.vert", "res/shaders/UIShader.frag");
			m_BasicShader = m_ShaderManager->AddShader("res/shaders/Basic.glsl");
			m_BasicShader2 = m_ShaderManager->AddShader("res/shaders/Basic2.glsl");

			m_ModelManager->AddModel("res/models/primitives/cube.obj");
			m_ModelManager->AddModel("res/models/primitives/sphere.obj");
			m_ModelManager->AddModel("res/models/primitives/cone.obj");
			m_ModelManager->AddModel("res/models/primitives/cylinder.obj");
			m_ModelManager->AddModel("res/models/primitives/teapot.obj");
			m_ModelManager->CreateVAO();

			m_SceneRoot = new GameObject(m_EntityManager);
			srand(NULL);
			for (int i = 0; i < 100; i++)
			{
				for (int j = 0; j < 100; j++)
				{

					objects[i][j] = new GameObject(m_EntityManager);
					objects[i][j]->AddComponent<Transform>(objects[i][j], glm::mat4(1.0f),
						glm::translate(glm::mat4(1.0f), glm::vec3(2.0f * i, 0.0f, 2.0f * j)));
					unsigned int rM = rand() % 5;
					objects[i][j]->AddComponent<Mesh>(m_ModelManager->GetModel(randomM[rM]));
					unsigned int rT = rand() % 3;

					m_SceneRoot->AddChild(objects[i][j]);

					if (i == 0 && j == 0) continue;
					objects[i][j]->AddComponent<Material>(m_BasicShader, glm::vec4(m_TextureArray->GetTexture(randomT[rT]), 0.0f, 0.0f, 0.0f));
				}
			}

			objects[0][0]->AddComponent<Material>(m_BasicShader2, glm::vec4(m_TextureArray->GetTexture(randomT[0]), 0.0f, 0.0f, 0.0f));

			glEnable(GL_DEPTH_TEST);

			m_SceneRoot->Render(true);
			m_BatchRenderer->Render();
		}

		virtual void OnUpdate(float dt) override
		{		
			{
				//PROFILE_SCOPE("UPDATE")
				m_SystemManager.UpdateAll(dt);
			}

			{
				PROFILE_SCOPE("SUBMIT COMMANDS")
				m_SceneRoot->Render();
			}
			
			{
				PROFILE_SCOPE("RENDER")
				m_BatchRenderer->Render();

				//Renderer::Clear(0.3f, 0.3f, 0.3f);
				//m_BasicShader->SetMat4("view", cam.GetViewMatrix());

				/*for (int i = 0; i < 100; i++)
				{
					for (int j = 0; j < 100; j++)
					{
						m_BasicShader->SetMat4("model", objects[i][j]->GetComponent<Transform>()->GetWorld());
						m_BasicShader->SetFloat("layer", objects[i][j]->GetComponent<Material>()->GetTexture());
						Renderer::Render(objects[i][j]->GetComponent<Mesh>()->GetModel()->VAO);
					}
				}*/
			}

		}
	};

}

sixengine::Application* sixengine::CreateApplication()
{
	return new Game("App", 1280, 720);
}