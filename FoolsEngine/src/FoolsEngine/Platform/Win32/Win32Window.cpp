#include "Win32Window.h"

#include "FoolsEngine/Platform/Events/Event.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

// fe::InputCodes are compatibile with GLFW so no keycode conversion is needed

namespace fe
{
	Win32Window::Win32Window(const WindowAttributes& attr, GLFWwindow* nativeWindow)
	{
		FE_PROFILER_FUNC();
		
		m_GAPI = attr.GAPI;
		Init(attr, nativeWindow);
	}

	Win32Window::~Win32Window()
	{
		FE_PROFILER_FUNC();

		ShutDown();
	}

	void Win32Window::OnUpdate()
	{
		FE_PROFILER_FUNC();
		Context::Log::Core.Trace("Win10Window::OnUpdate()"_FE);

		// events polling
		{
			FE_PROFILER_SCOPE("GLFW events polling");
			Context::Log::Core.Trace("GLFW events polling"_FE);
			glfwPollEvents();
		}
	}

	void Win32Window::Init(const WindowAttributes& attr, GLFWwindow* nativeWindow)
	{
		FE_PROFILER_FUNC();

		m_Data.Title = attr.Title;
		m_Data.Width = attr.Width;
		m_Data.Height = attr.Height;

		// window creation
		//{
		//	FE_PROFILER_SCOPE("GLFW_CreateWindow");
		//	FE_LOG_CORE_INFO("Creating Window {0} ({1}, {2})", attr.Title, attr.Width, attr.Height);
		//
		//	#ifdef FE_INTERNAL_BUILD
		//	glfwWindowHint(GLFW_CONTEXT_DEBUG, GLFW_TRUE);
		//	#endif // FE_INTERNAL_BUILD
		//
		//	m_Window = glfwCreateWindow((int)attr.Width, (int)attr.Height, attr.Title.c_str(), nullptr, nullptr);
		//}		
		m_Window = nativeWindow;
		glfwShowWindow(nativeWindow);
		glfwSetWindowUserPointer(nativeWindow, &m_Data);
	}

	void Win32Window::ShutDown()
	{
		FE_PROFILER_FUNC();
		Context::Log::Core.Info("Closing Window."_FE);

		{
			FE_PROFILER_SCOPE("glfwDestroyWindow()");
			glfwDestroyWindow(m_Window);
		}
	}

	void Win32Window::SetVSync(bool enabled)
	{
		FE_PROFILER_FUNC();

		if (m_Data.VSync == enabled)
			return;

		GLFWwindow* backup_current_context = glfwGetCurrentContext();

		if (backup_current_context != m_Window)
			glfwMakeContextCurrent(m_Window);
		
		if (enabled)
		{
			glfwSwapInterval(1);
			Context::Log::Core.Info("VSync enabled."_FE);
		}
		else
		{
			glfwSwapInterval(0);
			Context::Log::Core.Info("VSync disabled."_FE);
		}
		m_Data.VSync = enabled;

		if (backup_current_context != m_Window)
			glfwMakeContextCurrent(backup_current_context);
	}
}