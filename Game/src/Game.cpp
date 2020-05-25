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

#include "Renderer/Techniques/AnimationPBR.h"
#include "Renderer/Techniques/StaticPBR.h"
#include "Renderer/Techniques/UI.h"

#include "Gameplay/Systems/AnimationSystem.h"
#include <Core\CameraSystem\FlyingCameraSystem.h>

#define SCENE_FILE 0

namespace sixengine {

	class Game : public Application
	{
	private:
		#if SCENE_FILE
		Scene m_Scene;
		#else
		GameObject *m_SceneRoot, *m_UIRoot;
		Shader *m_BasicShader, *m_BasicShader2, *m_FontShader;

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
			
			m_SystemManager.AddSystem<AnimationSystem>();
			m_SystemManager.AddSystem<FlyingCameraSystem>();

			/// SETUP ASSETS
			/// =========================================================
			m_BasicShader = m_ShaderManager->AddShader("res/shaders/PBR.glsl");
			m_BasicShader2 = m_ShaderManager->AddShader("res/shaders/Animation.glsl");
			m_FontShader = m_ShaderManager->AddShader("res/shaders/Font.glsl");

			GameObject* go = new GameObject(*Application::Get().GetEntityManager());
			go->AddComponent<Transform>(go);
			go->AddComponent<Camera>(go);
			go->GetComponent<Camera>()->SetPerspective((float)Application::Get().GetWindow().GetWidth()/ (float)Application::Get().GetWindow().GetHeight());
			go->AddComponent<FlyingCamera>();

			Camera::ActiveCamera = go->GetComponent<Camera>().Get();

			go = new GameObject(*Application::Get().GetEntityManager());
			go->AddComponent<Transform>(go);
			go->AddComponent<Camera>(go);
			go->GetComponent<Camera>()->SetOrthogonal((float)Application::Get().GetWindow().GetWidth(), (float)Application::Get().GetWindow().GetHeight());

			Font* font = new Font("res/fonts/DroidSans.ttf");
			UI* ui = new UI(m_FontShader, go->GetComponent<Camera>().Get());
			ui->AddFont(font);

			StaticPBR* staticM = new StaticPBR(m_BasicShader, Camera::ActiveCamera);
			AnimationPBR* animatedM = new AnimationPBR(m_BasicShader2, Camera::ActiveCamera);

			m_BatchRenderer->AddTechnique(staticM);
			m_BatchRenderer->AddTechnique(animatedM);
			m_BatchRenderer->AddTechnique(ui);

			m_TextureArray->AddTexture("res/textures/test/Bricks.jpg");
			m_TextureArray->AddTexture("res/textures/test/Wood1.jpg");
			m_TextureArray->AddTexture("res/textures/test/Wood2.jpg");
			m_TextureArray->AddTexture("res/models/par/textures/parasiteZombie_diffuse.png");
			m_TextureArray->AddTexture("res/models/par/textures/parasiteZombie_normal.png");
			m_TextureArray->AddTexture("res/models/par/textures/parasiteZombie_specular.png");
			m_TextureArray->CreateTextureArray();

			m_MaterialManager->CreateMaterial(
				m_ShaderManager->Get("Font"),
				glm::vec4(0),
				"FontMaterial");

			m_MaterialManager->CreateMaterial(
				m_ShaderManager->Get("PBR"),
				glm::vec4(m_TextureArray->GetTexture("Bricks")),
				"BrickBasic1");

			m_MaterialManager->CreateMaterial(
				m_ShaderManager->Get("PBR"),
				glm::vec4(m_TextureArray->GetTexture("Wood1")),
				"Wood1Basic1"); 

			m_MaterialManager->CreateMaterial(
				m_ShaderManager->Get("PBR"),
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

			m_ModelManager->GetModel("par")->LoadAnimation("res/models/par/par.dae", "samba");
			m_ModelManager->GetModel("par")->LoadAnimation("res/models/par/par_walk.dae", "walk");
				
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
			m_UIRoot = new GameObject(m_EntityManager);
			srand(NULL);
			
			GameObject* obj;
			obj = new GameObject(m_EntityManager);
			obj->AddComponent<Transform>(obj);
			obj->GetComponent<Transform>()->SetWorldPosition(5.0f, 10.0f, 0.0f);
			obj->AddComponent<Text>("Testowy tekst, do testowania.", glm::vec3(0.0f, 1.0f, 1.0f), 1.0f);
			obj->AddComponent<Material>(*m_MaterialManager->Get("FontMaterial"));
			m_UIRoot->AddChild(obj);

			obj = new GameObject(m_EntityManager);
			obj->AddComponent<Transform>(obj);
			obj->GetComponent<Transform>()->SetWorldPosition(5.0, 690.0f, 0.0f);

			obj->AddComponent<Text>("Sixengine 0.?", glm::vec3(1.0f, 0.0f, 1.0f), 0.3f);
			obj->AddComponent<Material>(*m_MaterialManager->Get("FontMaterial"));
			m_UIRoot->AddChild(obj);
			
			for (int i = 0; i < 50; i++)
			{
				for (int j = 0; j < 2; j++)
				{
					obj = new GameObject(m_EntityManager);
					obj->AddComponent<Transform>(obj);
					obj->GetComponent<Transform>()->SetWorldPosition(2.0f * i, 0.0f, 2.0f * j);
					unsigned int rM = rand() % 5;
					obj->AddComponent<Mesh>(m_ModelManager->GetModel(randomM[rM]));

					unsigned int rT = rand() % 3;
					obj->AddComponent<Material>(*m_MaterialManager->Get(randomT[rT]));

					m_SceneRoot->AddChild(obj);
				}
			}

			for (int i = 0; i < 5; i++)
			{
				for (int j = 0; j < 5; j++)
				{
					obj = new GameObject(m_EntityManager);

					obj->AddComponent<Transform>(obj);
					obj->GetComponent<Transform>()->SetWorldPosition(5.0f * i, 0.0f, 5.0f * j);
					obj->GetComponent<Transform>()->SetLocalScale(0.05f, 0.05f, 0.05f);
					obj->AddComponent<Mesh>(m_ModelManager->GetModel("par"));
					obj->AddComponent<Material>(*m_MaterialManager->Get("parasiteZombie"));
					obj->AddComponent<Animation>();					

					m_SceneRoot->AddChild(obj);
				}
			}
			#endif
			
			glEnable(GL_DEPTH_TEST);

			m_BatchRenderer->Configure();

			#if SCENE_FILE
			m_Scene.Render(true);
			#else
			m_SceneRoot->Render(true);
			m_UIRoot->Render(true);
			#endif
			m_BatchRenderer->Render();
		}

		virtual void OnUpdate(float dt) override
		{
			m_SystemManager.UpdateAll(dt);
		}

		virtual void OnRender(float dt) override
		{
			m_SceneRoot->Render();
			m_UIRoot->Render();
			m_BatchRenderer->Render();
		}
	};

}

sixengine::Application* sixengine::CreateApplication()
{
	return new Game("App", 1280, 720);
}