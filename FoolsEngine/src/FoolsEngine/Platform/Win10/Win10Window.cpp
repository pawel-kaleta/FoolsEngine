#include "FE_pch.h"
#include "FoolsEngine/Core/Window.h"
#include "Win10Window.h"

namespace fe
{
	static bool s_GLFWInitialized = false;

	Win10Window::Win10Window(const WindowAttributes& attr)
	{
		FE_PROFILER_FUNC();

		Init(attr);
	}

	Win10Window::~Win10Window()
	{
		FE_PROFILER_FUNC();

		ShutDown();
	}

	void Win10Window::OnUpdate()
	{
		FE_PROFILER_FUNC();

		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}

	void Win10Window::GLFWErrorCallback(int error, const char* msg)
	{
		FE_LOG_CORE_ERROR("GLFW Error ({0}): {1}", error, msg);
	}

	void Win10Window::Init(const WindowAttributes& attr)
	{
		FE_PROFILER_FUNC();

		m_Data.Title = attr.Title;
		m_Data.Width = attr.Width;
		m_Data.Height = attr.Height;

		if (!s_GLFWInitialized)
		{
			FE_PROFILER_SCOPE("GLFW_Initialization");
			FE_LOG_CORE_INFO("Initializing GLFW");

			bool isInit = glfwInit();
			FE_CORE_ASSERT(isInit, "GLFW initialization failed!");
			glfwSetErrorCallback(GLFWErrorCallback);
			s_GLFWInitialized = true;
		}

		// window creation
		{
			FE_PROFILER_SCOPE("GLFW_CreateWindow");
			FE_LOG_CORE_INFO("Creating Window {0} ({1}, {2})", attr.Title, attr.Width, attr.Height);

			m_Window = glfwCreateWindow((int)attr.Width, (int)attr.Height, attr.Title.c_str(), nullptr, nullptr);
		}

		glfwMakeContextCurrent(m_Window);
		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);

		SetGLFWEventsCallbacks();
	}

	void Win10Window::SetGLFWEventsCallbacks()
	{
		FE_PROFILER_FUNC();
	}

	void Win10Window::ShutDown()
	{
		FE_PROFILER_FUNC();
		glfwDestroyWindow(m_Window);

		glfwTerminate();
	}

	void Win10Window::SetVSync(bool enabled)
	{
		FE_PROFILER_FUNC();

		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.VSync = enabled;
	}
}