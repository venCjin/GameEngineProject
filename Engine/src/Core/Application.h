#pragma once

#include <string>
#include <memory>
#include <Core/Window.h>
#include <Core/Events/ApplicationEvent.h>
#include <Core/Timer.h>
#include <Core/Input.h>
#include <ECS/ECS.h>

namespace sixengine {

	class Application : public EntityComponentSystem
	{
	private:
		static Application* s_Instance;
		std::unique_ptr<Window> m_Window;
		bool m_Running = true, m_Minimized = false;
		Timer* m_Timer;
		Input* m_Input;

	public:
		Application(std::string = "Six Engine", unsigned int width = 1280, unsigned int height = 720);
		virtual ~Application();

		void Run();

		virtual void OnInit() {}
		virtual void OnShutdown() {}
		virtual void OnUpdate(float dt) {}
		virtual void OnRender(float dt) {}

		virtual void OnEvent(Event& event);

		inline Window& GetWindow() { return *m_Window; }
		inline Timer& GetTimer() { return *m_Timer; }
		inline Input& GetInput() { return *m_Input; }
		
		static inline Application& Get() { return *s_Instance; }

	private:
		bool OnWindowResize(WindowResizeEvent& e);
		bool OnWindowClose(WindowCloseEvent& e);
	};

	// Implemented by CLIENT
	Application* CreateApplication();

}