#include "Win32Window.h"

#include "FoolsEngine/Platform/Events/Event.h"
#include "FoolsEngine/Foundation/Context/Pile.h"
#include "FoolsEngine/Foundation/Containers/StringBuilder.h"

#include <GLFW/glfw3.h>

// fe::InputCodes are compatibile with GLFW so no keycode conversion is needed

namespace fe
{
	void Win32Window::Init(const Attributes& attr)
	{
		FE_PROFILER_FUNC();
		
		new (this) Win32Window();

		mAttributes = attr;
		m_Window = nullptr;
	}

	void Win32Window::Destroy()
	{
		FE_PROFILER_FUNC();

		Context::Log::Core.Info("Closing Window."_FE);

		{
			FE_PROFILER_SCOPE("glfwDestroyWindow()");
			glfwDestroyWindow(m_Window);
		}
	}

	void Win32Window::PollEvents()
	{
		FE_PROFILER_FUNC();
		Context::Log::Core.Trace("Win10Window::PollEvents()"_FE);
		glfwPollEvents();
	}

	void Win32Window::SwapBuffers()
	{
		FE_PROFILER_FUNC();
		Context::Log::Core.Trace("Win10Window::SwapBuffers()"_FE);
		glfwSwapBuffers(m_Window);
	}

	void Win32Window::Create()
	{
		FE_PROFILER_FUNC();


#ifdef FE_INTERNAL_BUILD
			glfwWindowHint(GLFW_CONTEXT_DEBUG, GLFW_TRUE);
#endif // FE_INTERNAL_BUILD

		const auto& attr = mAttributes;
		Pile p;
		
		{
			FE_PROFILER_SCOPE("glfwCreateWindow()");
			m_Window = glfwCreateWindow((int)attr.Width, (int)attr.Height, GetCString(attr.Title, &p).Data, nullptr, nullptr);
		}

		FE_CORE_ASSERT(m_Window, "GLFW Window creation failed!");
		
		glfwMakeContextCurrent(m_Window);
	
		glfwSetWindowUserPointer(m_Window, this);
	}

	void Win32Window::SetVSync(bool enabled)
	{
		FE_PROFILER_FUNC();

		if (mVSync == enabled)
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
		mVSync = enabled;

		if (backup_current_context != m_Window)
			glfwMakeContextCurrent(backup_current_context);
	}
}