#include "pch.h"
#include "Application.h"

#include <GLFW/glfw3.h>

namespace sixengine {

	Application* Application::s_Instance = nullptr;

	Application::Application(std::string title, unsigned int width, unsigned int height)
	{
		s_Instance = this;

		m_Window = std::make_unique<Window>(title, width, height);
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
				
			}
			m_Window->OnUpdate();
		}
		OnShutdown();
	}

}