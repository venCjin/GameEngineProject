#pragma once

#include <string>
#include <memory>
#include <Core/Window.h>

namespace sixengine {

	class Application
	{
	private:
		static Application* s_Instance;
		std::unique_ptr<Window> m_Window;
		bool m_Running = true, m_Minimized = false;

	public:
		Application(std::string = "Six Engine", unsigned int width = 1280, unsigned int height = 720);
		virtual ~Application();

		void Run();

		virtual void OnInit() {}
		virtual void OnShutdown() {}
		virtual void OnUpdate() {}

		inline Window& GetWindow() { return *m_Window; }
		
		static inline Application& Get() { return *s_Instance; }
	};

	// Implemented by CLIENT
	Application* CreateApplication();
}