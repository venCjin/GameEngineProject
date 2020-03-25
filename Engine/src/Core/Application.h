#pragma once

#include <string>
#include <memory>
#include <Core\Window.h>
#include <Core\Events\ApplicationEvent.h>
#include <Core\Timer.h>

namespace sixengine {

	class Application
	{
	private:
		static Application* s_Instance;
		std::unique_ptr<Window> m_Window;
		bool m_Running = true, m_Minimized = false;
		Timer* m_Timer;

	public:
		Application(std::string = "Six Engine", unsigned int width = 1280, unsigned int height = 720);
		virtual ~Application();

		void Run();

		virtual void OnInit() {}
		virtual void OnShutdown() {}
		virtual void OnUpdate() {}

		virtual void OnEvent(Event& event);

		inline Window& GetWindow() { return *m_Window; }
		inline Timer& GetTimer() { return *m_Timer; }
		
		static inline Application& Get() { return *s_Instance; }

	private:
		bool OnWindowResize(WindowResizeEvent& e);
		bool OnWindowClose(WindowCloseEvent& e);
	};

	// Implemented by CLIENT
	Application* CreateApplication();
}