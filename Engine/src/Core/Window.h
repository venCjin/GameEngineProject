#pragma once

#include <string>
#include <utility>
struct GLFWwindow;

namespace sixengine {

	struct WindowData
	{
		std::string Title;
		unsigned int Width, Height;
		bool VSync;
	};

	class Window
	{
	private:
		GLFWwindow* m_Window;
		WindowData m_Data;
		float m_LastFrameTime = 0.0f;

	public:
		Window(std::string title, unsigned int width, unsigned int height);
		~Window();

		void OnUpdate();

		inline unsigned int GetWidth() const { return m_Data.Width; }
		inline unsigned int GetHeight() const { return m_Data.Height; }
		std::pair<float, float> GetWindowPos() const;

		// Window attributes
		void SetVSync(bool enabled);
		bool IsVSync() const;

		inline GLFWwindow* GetWindow() const { return m_Window; }

	private:
		void Init();
		void Shutdown();
	};

}