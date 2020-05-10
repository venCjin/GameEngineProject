#include <Engine.h>
#include <EntryPoint.h>

#include "Core/Scene.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "Renderer/BatchRenderer.h"
#include "Renderer/ModelManager.h"
#include "Renderer/MaterialManager.h"
#include "Renderer/TextureArray.h"
#include "Renderer/Technique.h"

#define SCENE_FILE 1

namespace sixengine {
	
	class Game : public Application
	{
	private:
		#if SCENE_FILE
		Scene m_Scene;
		#else
		GameObject* objects[90][90];
		GameObject* anim[7][7];
		GameObject *m_SceneRoot, *m_UIRoot;
		Shader *m_BasicShader, *m_BasicShader2;
		Camera cam, camUI;

		ShaderManager* m_ShaderManager;
		ModelManager* m_ModelManager;
		TextureArray* m_TextureArray;
		MaterialManager* m_MaterialManager;
		#endif
		BatchRenderer* m_BatchRenderer;

	public:
		Game(std::string title, unsigned int width, unsigned int height)
			: Application(title, width, height)
			#if SCENE_FILE
			, m_Scene(width, height)
		{
			#else
		{
			m_TextureArray = new TextureArray(2048, 2048);
			m_MaterialManager = new MaterialManager();
			m_ModelManager = new ModelManager();
			m_ShaderManager = new ShaderManager();

			float aspectRatio = (float)width / (float)height;
			cam.MakePerspective(aspectRatio);
			camUI.MakeOrtho(width, height);

			BatchRenderer::Initialize(m_ModelManager, m_TextureArray);
			#endif
			m_BatchRenderer = BatchRenderer::Instance();
		}
		
		~Game()
		{}

		virtual void OnInit() override
		{
			#if SCENE_FILE
			m_Scene.LoadScene("res/scenes/br.scene");
			#else
			/// SETUP ASSETS
			/// =========================================================
			m_BasicShader = m_ShaderManager->AddShader("res/shaders/Basic.glsl");
			m_BasicShader2 = m_ShaderManager->AddShader("res/shaders/Animation.glsl");

			StaticPBR* staticM = new StaticPBR(m_BasicShader, &cam);
			AnimationPBR* animatedM = new AnimationPBR(m_BasicShader2, &cam);

			m_BatchRenderer->AddTechnique(staticM);
			m_BatchRenderer->AddTechnique(animatedM);

			m_TextureArray->AddTexture("res/textures/test/Bricks.jpg");
			m_TextureArray->AddTexture("res/textures/test/Wood1.jpg");
			m_TextureArray->AddTexture("res/textures/test/Wood2.jpg");
			m_TextureArray->AddTexture("res/models/par/textures/parasiteZombie_diffuse.png");
			m_TextureArray->AddTexture("res/models/par/textures/parasiteZombie_normal.png");
			m_TextureArray->AddTexture("res/models/par/textures/parasiteZombie_specular.png");
			m_TextureArray->CreateTextureArray();

			m_MaterialManager->CreateMaterial(
				m_ShaderManager->Get("Basic"),
				glm::vec4(m_TextureArray->GetTexture("Bricks")),
				"BrickBasic1");

			m_MaterialManager->CreateMaterial(
				m_ShaderManager->Get("Basic"),
				glm::vec4(m_TextureArray->GetTexture("Wood1")),
				"Wood1Basic1"); 

			m_MaterialManager->CreateMaterial(
				m_ShaderManager->Get("Basic"),
				glm::vec4(m_TextureArray->GetTexture("Wood2")),
				"Wood2Basic1");

			m_MaterialManager->CreateMaterial(
				m_ShaderManager->Get("Animation"),
				glm::vec4(m_TextureArray->GetTexture("parasiteZombie_diffuse"), 
					m_TextureArray->GetTexture("parasiteZombie_normal"), 
					m_TextureArray->GetTexture("parasiteZombie_specular"), 0.0f),
				"parasiteZombie");

			m_ModelManager->AddModel("res/models/primitives/cube.obj");
			m_ModelManager->AddModel("res/models/primitives/sphere.obj");
			m_ModelManager->AddModel("res/models/primitives/cone.obj");
			m_ModelManager->AddModel("res/models/primitives/cylinder.obj");
			m_ModelManager->AddModel("res/models/primitives/teapot.obj");
			m_ModelManager->AddModel("res/models/par/par.dae");
			m_ModelManager->CreateVAO();

			// SETUP SCENE
			std::map<unsigned int, std::string> randomM;
			randomM[0] = "cube";
			randomM[1] = "sphere";
			randomM[2] = "cone";
			randomM[3] = "cylinder";
			randomM[4] = "teapot";

			std::map<unsigned int, std::string> randomT;
			randomT[0] = "BrickBasic1";
			randomT[1] = "Wood1Basic1";
			randomT[2] = "Wood2Basic1";

			m_SceneRoot = new GameObject(m_EntityManager);
			srand(NULL);
			
			for (int i = 0; i < 90; i++)
			{
				for (int j = 0; j < 90; j++)
				{
					objects[i][j] = new GameObject(m_EntityManager);
					objects[i][j]->AddComponent<Transform>(objects[i][j], glm::mat4(1.0f),
						glm::translate(glm::mat4(1.0f), glm::vec3(2.0f * i, 0.0f, 2.0f * j)));
					unsigned int rM = rand() % 5;
					objects[i][j]->AddComponent<Mesh>(m_ModelManager->GetModel(randomM[rM]));

					unsigned int rT = rand() % 3;
					objects[i][j]->AddComponent<Material>(*m_MaterialManager->Get(randomT[rT]));

					m_SceneRoot->AddChild(objects[i][j]);
				}
			}

			for (int i = 0; i < 7; i++)
			{
				for (int j = 0; j < 7; j++)
				{
					anim[i][j] = new GameObject(m_EntityManager);

					glm::mat4 mat(1.0f);
					mat = glm::translate(mat, glm::vec3(-30.0 + 5.0f * j, 0.0f, 0.0f + i * 5.0f));
					mat = glm::scale(mat, glm::vec3(0.05f, 0.05f, 0.05f));

					anim[i][j]->AddComponent<Transform>(anim[i][j], glm::mat4(1.0f), mat);
					anim[i][j]->AddComponent<Mesh>(m_ModelManager->GetModel("par"));
					anim[i][j]->AddComponent<Material>(*m_MaterialManager->Get("parasiteZombie"));

					m_SceneRoot->AddChild(anim[i][j]);
				}
			}
			#endif
			
			glEnable(GL_DEPTH_TEST);

			m_BatchRenderer->Configure();

			#if SCENE_FILE
			m_Scene.Render(true);
			#else
			m_SceneRoot->Render(true);
			#endif
			m_BatchRenderer->Render();
		}

		virtual void OnUpdate(float dt) override
		{		
			{
				PROFILE_SCOPE("ON UPDATE")

				{
					//PROFILE_SCOPE("ECS")
					m_SystemManager.UpdateAll(dt);
				}

				{
					//PROFILE_SCOPE("SUBMIT COMMANDS")
					#if SCENE_FILE
					m_Scene.Render();
					#else
					m_SceneRoot->Render();
					#endif
				}

				{
					PROFILE_SCOPE("RENDER")
					m_BatchRenderer->Render();
				}
			}
		}
	};

}

sixengine::Application* sixengine::CreateApplication()
{
	return new Game("App", 1280, 720);
}