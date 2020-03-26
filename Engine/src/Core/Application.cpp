#include "pch.h"
#include "Application.h"
#include "Core.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Windows.h>
namespace sixengine {

	Application* Application::s_Instance = nullptr;

	Application::Application(std::string title, unsigned int width, unsigned int height)
	{
		s_Instance = this;

		m_Timer = Timer::Instance();

		m_Window = std::make_unique<Window>(title, width, height);
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
		m_Window->SetVSync(false);
	}

	Application::~Application()
	{

	}

	void Application::Run()
	{
		OnInit();
		while (m_Running)
		{
			if (!m_Minimized)
			{
				OnUpdate(m_Timer->DeltaTime());
			}
			m_Window->OnUpdate();

			m_Timer->Tick();
			m_Timer->Reset();
		}
		OnShutdown();
	}

	void Application::OnEvent(Event& event)
	{
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