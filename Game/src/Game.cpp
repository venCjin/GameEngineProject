#include <Engine.h>
#include <EntryPoint.h>

#include "Renderer/PrimitiveUtils.h"
#include "Core/Timer.h"
#include "Core/ResourceManager.h"
#include "Core/ShaderManager.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/Components/Billboard.h"
#include "Gameplay/Systems/BillboardSystem.h"
#include <Gameplay\Components\UIRoot.h>
#include <Gameplay\Systems\UIRendererSystem.h>

#include "Core/Scene.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

//#include <fstream>

namespace sixengine {
	
	class Game : public Application
	{
	private:
		Scene m_Scene;

	public:
		Game(std::string title, unsigned int width, unsigned int height)
			: Application(title, width, height), m_Scene(width, height)
		{
		}
		
		~Game()
		{}

		virtual void OnInit() override
		{
			m_Scene.LoadScene("res/scenes/test.scene");
			
			glEnable(GL_DEPTH_TEST);
		}

		virtual void OnUpdate(float dt) override
		{		
			{
				//PROFILE_SCOPE("UPDATE")
				m_SystemManager.UpdateAll(dt);
			}

			{
				//PROFILE_SCOPE("RENDER")
				Renderer::Clear(0.3f, 0.3f, 0.3f);
				
				m_Scene.Render();
			}
		}
	};

}

sixengine::Application* sixengine::CreateApplication()
{
	return new Game("App", 1280, 720);
}