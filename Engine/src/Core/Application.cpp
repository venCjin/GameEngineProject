#include "pch.h"
#include "Application.h"
#include "Core.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Windows.h>
#include <Core/Profile.h>

namespace sixengine {

	Application* Application::s_Instance = nullptr;

	Application::Application(std::string title, unsigned int width, unsigned int height)
	{
		s_Instance = this;

		m_Timer = Timer::Instance();
		m_Input = Input::Get();

		m_Window = std::make_unique<Window>(title, width, height);
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
		m_Window->SetVSync(false);
	}

	Application::~Application()
	{

	}

	void Application::Run()
	{
		double renderAccumulator = 0.0;

		OnInit();
		while (m_Running)
		{
			{
				PROFILE_SCOPE("GAME UPDATE\n")

					m_Timer->Tick();
				m_Timer->Reset();

				if (!m_Minimized)
				{
					m_Window->ProcessInput();

					m_Input->OnPreUpdate();

					OnUpdate(m_Timer->DeltaTime());

					m_Input->OnPostUpdate();
				}

				OnRender(m_Timer->DeltaTime());

				while (m_Timer->TimeSinceReset() < sixengine::FRAMERATE_60);

				m_Window->SwapBuffers();
				/*if (renderAccumulator < sixengine::FRAMERATE_60)
				{
					OnRender(m_Timer->DeltaTime());
					m_Window->SwapBuffers();
					renderAccumulator = 0.0;
				}
				else
					renderAccumulator += m_Timer->DeltaTime();
	*/
			}
		}
		OnShutdown();
	}

	void Application::OnEvent(Event& event)
	{
		Input::Get()->OnEvent(event);

		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		//LOG_CORE_INFO("{0}", e);
		int width = e.GetWidth(), height = e.GetHeight();
		if (width == 0 || height == 0)
		{
			m_Minimized = true;
			return false;
		}
		m_Minimized = false;
		glViewport(0, 0, width, height);
		return false;
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		//LOG_CORE_INFO("{0}", e);
		m_Running = false;
		return true;
	}

}