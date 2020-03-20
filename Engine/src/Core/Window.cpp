#include "pch.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Window.h"

namespace sixengine {

	static void GLFWErrorCallback(int error, const char* description)
	{
		LOG_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	static bool s_GLFWInitialized = false;


	Window::Window(std::string title, unsigned int width, unsigned int height)
	{
		m_Data.Title = title;
		m_Data.Width = width;
		m_Data.Height = height;
		LOG_CORE_INFO("Creating window {0} ({1}, {2})", m_Data.Title, m_Data.Width, m_Data.Height);
		Init();
	}

	Window::~Window()
	{
	}

	void Window::OnUpdate()
	{
		glfwPollEvents();
		glfwSwapBuffers(m_Window);

		float time = glfwGetTime();
		float delta = time - m_LastFrameTime;
		m_LastFrameTime = time;
	}

	std::pair<float, float> Window::GetWindowPos() const
	{
		int x, y;
		glfwGetWindowPos(m_Window, &x, &y);
		return { x, y };
	}

	void Window::SetVSync(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.VSync = enabled;
	}

	bool Window::IsVSync() const
	{
		return m_Data.VSync;
	}

	void Window::Init()
	{
		if (!s_GLFWInitialized)
		{
			int success = glfwInit();
			CORE_ASSERT(success, "Could not intialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);

			s_GLFWInitialized = true;
		}

		m_Window = glfwCreateWindow((int)m_Data.Width, (int)m_Data.Height, m_Data.Title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(m_Window);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		CORE_ASSERT(status, "Failed to initialize Glad!");
		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);
	}

	void Window::Shutdown()
	{
		glfwTerminate();
	}

}