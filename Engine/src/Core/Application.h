#pragma once

#include <string>
#include <memory>
#include <Core/Window.h>
#include <Core/Events/ApplicationEvent.h>
#include <Core/Timer.h>
#include <Core/Input.h>
#include <ECS/ECS.h>

#include "ImGui/ImGuiLayer.h"
#include "imgui.h"

#include "Renderer/PrimitiveUtils.h"
#include "Renderer/Gizmo.h"

namespace sixengine {
	/// If you want to measure performace with MSI Afterburner and RivaTunerStatisticServer
	//#define AFTERBURNER 0

	class Application : public EntityComponentSystem
	{
	private:
		static Application* s_Instance;
		std::unique_ptr<Window> m_Window;
		bool m_Running = true, m_Minimized = false;
		Timer* m_Timer;
		Input* m_Input;
	#if defined(DEBUG) || defined(AFTERBURNER)
		ImGuiLayer* m_ImGui;
	#endif // DEBUG

	public:
		static Gizmo* attack;

	//public:
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
	#if defined(DEBUG) || defined(AFTERBURNER)
		inline ImGuiLayer& GetImGuiLayer() { return *m_ImGui; }
	#endif // DEBUG
		
		static inline Application& Get() { return *s_Instance; }

	private:
		bool OnWindowResize(WindowResizeEvent& e);
		bool OnWindowClose(WindowCloseEvent& e);
	};

	// Implemented by CLIENT
	Application* CreateApplication();

}