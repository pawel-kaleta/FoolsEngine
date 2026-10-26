#include "FoolsEngine/Platform/PlatformBase.h"

#include "FoolsEngine/Platform/Win32/Win32Window.h"
#include "FoolsEngine/Platform/Events/Event.h"
#include "FoolsEngine/Foundation/Debug/Profiler.h"
#include "FoolsEngine/Foundation/Debug/Asserts.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

namespace fe
{
	void GLFWErrorCallback(int error, const char* msg)
	{
		Context::Log::Core.Error("GLFW Error ({0}): {1}"_FE, error, msg);
		FE_CORE_ASSERT(false, "");
	}

	bool PlatformBaseWindows::Create()
	{
		FE_PROFILER_FUNC();
		
		{
			FE_PROFILER_SCOPE("glfwInit()");
			if (!glfwInit())
			{
				FE_CORE_ASSERT(false, "GLFW initialization failed!");
				return false;
			}
		}
		{
			FE_PROFILER_SCOPE("glfwSetErrorCallback()");
			glfwSetErrorCallback(GLFWErrorCallback);
		}

		return true;
	}

	void PlatformBaseWindows::Terminate()
	{
		FE_PROFILER_FUNC();
		glfwTerminate();
	}

	void PlatformBaseWindows::SetEventCallbacks(Win32Window* window)
	{
		FE_PROFILER_FUNC();
		
		Context::Log::Core.Info("Setting events callbacks:"_FE);

		Context::Log::Core.Info("    WindowResizeEvent"_FE);
		glfwSetWindowSizeCallback(window->m_Window, [](GLFWwindow* window, int width, int height)
			{
				Win32Window& win32_window = *(Win32Window*)glfwGetWindowUserPointer(window);
				win32_window.mAttributes.Width = width;
				win32_window.mAttributes.Height = height;

				FE_NEW_EVENT(win32_window.mEventCallback, event, Events::WindowResizeEvent, width, height);
			});

		Context::Log::Core.Info("    WindowCloseEvent"_FE);
		glfwSetWindowCloseCallback(window->m_Window, [](GLFWwindow* window)
			{
				Win32Window& win32_window = *(Win32Window*)glfwGetWindowUserPointer(window);
				FE_NEW_EVENT(win32_window.mEventCallback, event, Events::WindowCloseEvent);
			});

		Context::Log::Core.Info("    WindowFocusChangeEvents"_FE);
		glfwSetWindowFocusCallback(window->m_Window, [](GLFWwindow* window, int focus)
			{
				Win32Window& win32_window = *(Win32Window*)glfwGetWindowUserPointer(window);
				if (focus)
				{
					FE_NEW_EVENT(win32_window.mEventCallback, event, Events::WindowGainedFocusEvent);
				}
				else
				{
					FE_NEW_EVENT(win32_window.mEventCallback, event, Events::WindowLostFocusEvent);
				}
			});

		Context::Log::Core.Info("    KeyEvents"_FE);
		glfwSetKeyCallback(window->m_Window, [](GLFWwindow* window, int key, int scancode, int action, int modes)
			{
				Win32Window& win32_window = *(Win32Window*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					FE_NEW_EVENT(win32_window.mEventCallback, event, Events::KeyPressedEvent, key, 0);
					break;
				}
				case GLFW_RELEASE:
				{
					FE_NEW_EVENT(win32_window.mEventCallback, event, Events::KeyReleasedEvent, key);
					break;
				}
				case GLFW_REPEAT:
				{
					FE_NEW_EVENT(win32_window.mEventCallback, event, Events::KeyPressedEvent, key, 1);
					break;
				}
				}
			});

		Context::Log::Core.Info("    KeyTypedEvent"_FE);
		glfwSetCharCallback(window->m_Window, [](GLFWwindow* window, unsigned int keycode)
			{
				Win32Window& win32_window = *(Win32Window*)glfwGetWindowUserPointer(window);
				FE_NEW_EVENT(win32_window.mEventCallback, event, Events::KeyTypedEvent, keycode);
			});

		Context::Log::Core.Info("    MouseButtonEvents"_FE);
		glfwSetMouseButtonCallback(window->m_Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				Win32Window& win32_window = *(Win32Window*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					FE_NEW_EVENT(win32_window.mEventCallback, event, Events::MouseButtonPressedEvent, button);
					break;
				}
				case GLFW_RELEASE:
				{
					FE_NEW_EVENT(win32_window.mEventCallback, event, Events::MouseButtonReleasedEvent, button);
					break;
				}
				}
			});

		Context::Log::Core.Info("    MouseScrolledEvent"_FE);
		glfwSetScrollCallback(window->m_Window, [](GLFWwindow* window, double x_offset, double y_offset)
			{
				Win32Window& win32_window = *(Win32Window*)glfwGetWindowUserPointer(window);
				FE_NEW_EVENT(win32_window.mEventCallback, event, Events::MouseScrolledEvent, (float)x_offset, (float)y_offset);
			});

		Context::Log::Core.Info("    MouseMovedEvent"_FE);
		glfwSetCursorPosCallback(window->m_Window, [](GLFWwindow* window, double x_position, double y_position)
			{
				Win32Window& win32_window = *(Win32Window*)glfwGetWindowUserPointer(window);
				FE_NEW_EVENT(win32_window.mEventCallback, event, Events::MouseMovedEvent, (float)x_position, (float)y_position);
			});
	}
}