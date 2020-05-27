#include <Engine.h>
#include <EntryPoint.h>

#include "Core/Scene.h"

#include "Renderer/BatchRenderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

namespace sixengine {

	class Game : public Application
	{
	private:
		Scene m_Scene;
		BatchRenderer* m_BatchRenderer;

	public:
		Game(std::string title, unsigned int width, unsigned int height)
			: Application(title, width, height), m_Scene(width, height)
			{
				m_BatchRenderer = BatchRenderer::Instance();
			}

		~Game()
		{}

		virtual void OnInit() override
		{
			m_Scene.LoadScene("res/scenes/exported.scene");

			m_BatchRenderer->Configure();

			m_Scene.Render(true);
			
			m_BatchRenderer->Render();
		}

		virtual void OnUpdate(float dt) override
		{
			if (Input::IsKeyPressed(KeyCode::F9))
			{
				Application::Get().GetWindow().SwitchCursorVisibility();
			}

			{
				PROFILE_SCOPE("ON UPDATE")

				{
					//PROFILE_SCOPE("ECS")
					m_SystemManager.UpdateAll(dt);
				}

				{
					//PROFILE_SCOPE("SUBMIT COMMANDS")
					m_Scene.Render();
				}

				{
					//PROFILE_SCOPE("RENDER")
					m_BatchRenderer->Render();
				}

				{
					//PROFILE_SCOPE("DRAW GIZMOS")
					m_Scene.DrawGizmos();
				}
			}
		}
	};

}

sixengine::Application* sixengine::CreateApplication()
{
	return new Game("App", 1280, 720);
}