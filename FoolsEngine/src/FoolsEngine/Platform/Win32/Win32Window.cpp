#include "FE_pch.h"
#include "Win32Window.h"

#include "FoolsEngine/Platform/Events/Event.h"

#include <glad/glad.h>

// fe::InputCodes are compatibile with GLFW so no keycode conversion is needed

namespace fe
{
	Win32Window::Win32Window(const WindowAttributes& attr)
	{
		FE_PROFILER_FUNC();
		
		m_GAPI = attr.GAPI;
		Init(attr);
	}

	Win32Window::~Win32Window()
	{
		FE_PROFILER_FUNC();

		ShutDown();
	}

	void Win32Window::OnUpdate()
	{
		FE_PROFILER_FUNC();
		FE_LOG_CORE_TRACE("Win10Window::OnUpdate()");

		// events polling
		{
			FE_PROFILER_SCOPE("GLFW events polling");
			FE_LOG_CORE_TRACE("GLFW events polling");
			glfwPollEvents();
		}
		
		m_CurrentRenderingContext->SwapBuffers();
	}

	void Win32Window::Init(const WindowAttributes& attr)
	{
		FE_PROFILER_FUNC();

		m_Data.Title = attr.Title;
		m_Data.Width = attr.Width;
		m_Data.Height = attr.Height;

		// window creation
		{
			FE_PROFILER_SCOPE("GLFW_CreateWindow");
			FE_LOG_CORE_INFO("Creating Window {0} ({1}, {2})", attr.Title, attr.Width, attr.Height);

#ifdef FE_INTERNAL_BUILD
			glfwWindowHint(GLFW_CONTEXT_DEBUG, GLFW_TRUE);
#endif // FE_INTERNAL_BUILD

			m_Window = glfwCreateWindow((int)attr.Width, (int)attr.Height, attr.Title.c_str(), nullptr, nullptr);
			s_GLFWWindowCount++;
		}		

		glfwSetWindowUserPointer(m_Window, &m_Data);
	}

	void Win32Window::ShutDown()
	{
		FE_PROFILER_FUNC();
		FE_LOG_CORE_INFO("Closing Window.");

		{
			FE_PROFILER_SCOPE("glfwDestroyWindow()");
			glfwDestroyWindow(m_Window);
			s_GLFWWindowCount--;
		}
	}

	void Win32Window::SetVSync(bool enabled)
	{
		FE_PROFILER_FUNC();

		GLFWwindow* backup_current_context = glfwGetCurrentContext();

		if (backup_current_context != m_Window)
			glfwMakeContextCurrent(m_Window);
		
		if (enabled)
		{
			glfwSwapInterval(1);
			FE_LOG_CORE_INFO("VSync enabled.");
		}
		else
		{
			glfwSwapInterval(0);
			FE_LOG_CORE_INFO("VSync disabled.");
		}
		m_Data.VSync = enabled;

		if (backup_current_context != m_Window)
			glfwMakeContextCurrent(backup_current_context);
	}
}