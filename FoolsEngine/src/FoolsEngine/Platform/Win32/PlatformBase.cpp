#include "FE_pch.h"
#include "FoolsEngine/Platform/PlatformBase.h"

#include "FoolsEngine/Platform/Win32/Win32Window.h"
#include "FoolsEngine/Platform/Events/Event.h"
#include "FoolsEngine/Foundation/Debug/Profiler.h"
#include "FoolsEngine/Foundation/Debug/Asserts.h"
#include "FoolsEngine/Foundation/Debug/Log.h"


#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace fe
{
	void GLFWErrorCallback(int error, const char* msg)
	{
		FE_LOG_CORE_ERROR("GLFW Error ({0}): {1}", error, msg);
		FE_CORE_ASSERT(false, "");
	}

	bool PlatformBaseWindows::Create()
	{
		FE_PROFILER_FUNC();
		{
			FE_PROFILER_SCOPE("GLFW_Initialization");
			if (!glfwInit())
			{
				FE_CORE_ASSERT(false, "GLFW initialization failed!");
				return false;
			}
		}
		return true;

		glfwSetErrorCallback(GLFWErrorCallback);
	}

	void PlatformBaseWindows::Terminate()
	{
		FE_PROFILER_FUNC();
		glfwTerminate();
	}

	void PlatformBaseWindows::SetEventCallbacks(Win32Window* window)
	{
		FE_PROFILER_FUNC();
		
		FE_LOG_CORE_INFO("Setting events callbacks:");

		FE_LOG_CORE_INFO("	WindowResizeEvent");
		glfwSetWindowSizeCallback(window->m_Window, [](GLFWwindow* window, int width, int height)
			{
				Win32Window::WinData& data = *(Win32Window::WinData*)glfwGetWindowUserPointer(window); // data = m_Data
				data.Width = width;
				data.Height = height;

				FE_NEW_EVENT(data.EventCallback, event, Events::WindowResizeEvent, width, height);
			});

		FE_LOG_CORE_INFO("	WindowCloseEvent");
		glfwSetWindowCloseCallback(window->m_Window, [](GLFWwindow* window)
			{
				Win32Window::WinData& data = *(Win32Window::WinData*)glfwGetWindowUserPointer(window); // data = m_Data
				FE_NEW_EVENT(data.EventCallback, event, Events::WindowCloseEvent);
			});

		FE_LOG_CORE_INFO("	WindowFocusChangeEvents");
		glfwSetWindowFocusCallback(window->m_Window, [](GLFWwindow* window, int focus)
			{
				Win32Window::WinData& data = *(Win32Window::WinData*)glfwGetWindowUserPointer(window); // data = m_Data
				if (focus)
				{
					FE_NEW_EVENT(data.EventCallback, event, Events::WindowGainedFocusEvent);
				}
				else
				{
					FE_NEW_EVENT(data.EventCallback, event, Events::WindowLostFocusEvent);
				}
			});

		FE_LOG_CORE_INFO("	KeyEvents");
		glfwSetKeyCallback(window->m_Window, [](GLFWwindow* window, int key, int scancode, int action, int modes)
			{
				Win32Window::WinData& data = *(Win32Window::WinData*)glfwGetWindowUserPointer(window); // data = m_Data

				switch (action)
				{
				case GLFW_PRESS:
				{
					FE_NEW_EVENT(data.EventCallback, event, Events::KeyPressedEvent, key, 0);
					break;
				}
				case GLFW_RELEASE:
				{
					FE_NEW_EVENT(data.EventCallback, event, Events::KeyReleasedEvent, key);
					break;
				}
				case GLFW_REPEAT:
				{
					FE_NEW_EVENT(data.EventCallback, event, Events::KeyPressedEvent, key, 1);
					break;
				}
				}
			});

		FE_LOG_CORE_INFO("	KeyTypedEvent");
		glfwSetCharCallback(window->m_Window, [](GLFWwindow* window, unsigned int keycode)
			{
				Win32Window::WinData& data = *(Win32Window::WinData*)glfwGetWindowUserPointer(window); // data = m_Data
				FE_NEW_EVENT(data.EventCallback, event, Events::KeyTypedEvent, keycode);
			});

		FE_LOG_CORE_INFO("	MouseButtonEvents");
		glfwSetMouseButtonCallback(window->m_Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				Win32Window::WinData& data = *(Win32Window::WinData*)glfwGetWindowUserPointer(window); // data = m_Data

				switch (action)
				{
				case GLFW_PRESS:
				{
					FE_NEW_EVENT(data.EventCallback, event, Events::MouseButtonPressedEvent, button);
					break;
				}
				case GLFW_RELEASE:
				{
					FE_NEW_EVENT(data.EventCallback, event, Events::MouseButtonReleasedEvent, button);
					break;
				}
				}
			});

		FE_LOG_CORE_INFO("	MouseScrolledEvent");
		glfwSetScrollCallback(window->m_Window, [](GLFWwindow* window, double x_offset, double y_offset)
			{
				Win32Window::WinData& data = *(Win32Window::WinData*)glfwGetWindowUserPointer(window); // data = m_Data
				FE_NEW_EVENT(data.EventCallback, event, Events::MouseScrolledEvent, (float)x_offset, (float)y_offset);
			});

		FE_LOG_CORE_INFO("	MouseMovedEvent");
		glfwSetCursorPosCallback(window->m_Window, [](GLFWwindow* window, double x_position, double y_position)
			{
				Win32Window::WinData& data = *(Win32Window::WinData*)glfwGetWindowUserPointer(window); // data = m_Data
				FE_NEW_EVENT(data.EventCallback, event, Events::MouseMovedEvent, (float)x_position, (float)y_position);
			});
	}
}