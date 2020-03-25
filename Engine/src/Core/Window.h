#pragma once

#include <string>
#include <functional>

struct GLFWwindow;
class Event;

namespace sixengine {

	using EventCallbackFn = std::function<void(Event&)>;

	struct WindowData
	{
		std::string Title;
		unsigned int Width, Height;
		bool VSync;
		EventCallbackFn EventCallback;
	};

	class Window
	{
	private:
		GLFWwindow* m_Window;
		WindowData m_Data;

	public:
		Window(std::string title, unsigned int width, unsigned int height);
		~Window();

		void OnUpdate();

		inline unsigned int GetWidth() const { return m_Data.Width; }
		inline unsigned int GetHeight() const { return m_Data.Height; }
		std::pair<float, float> GetWindowPos() const;

		// Window attributes
		inline void SetEventCallback(const EventCallbackFn& callback) { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled);
		bool IsVSync() const;

		inline GLFWwindow* GetWindow() const { return m_Window; }

	private:
		void Init();
		void Shutdown();
	};

}