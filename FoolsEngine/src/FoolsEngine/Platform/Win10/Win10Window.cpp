#include "FE_pch.h"
#include "Win10Window.h"

#include <glad/glad.h>

#include "FoolsEngine\Events\Event.h"


// fe::InputCodes are compatibile with GLFW so no keycode conversion is needed

namespace fe
{
	bool Win10Window::s_GLFWInitialized = false;
	uint16_t Win10Window::s_GLFWWindowCount = 0;

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

	void Win10Window::CreateRenderingContext(GDIType GDI)
	{
		FE_PROFILER_FUNC();
		FE_LOG_CORE_INFO("Creating rendering context");

		if (m_RenderingContexts.count(GDI))
		{
			FE_CORE_ASSERT(false, "Rendering context of this GDI was already created for this window!");
			return;
		};

		m_RenderingContexts[GDI] = RenderingContext::Create(GDIType::OpenGL, m_Window);
		m_CurrentRenderingContext = m_RenderingContexts.at(GDI).get();
		m_CurrentRenderingContext->Init();

		SetVSync(false);
	}

	void Win10Window::MakeRenderingContextCurrent(GDIType GDI)
	{
		FE_PROFILER_FUNC();
		FE_CORE_ASSERT(false, "Multiple rendering contexts not yet supported. Context is being made current upon creation.");
	}

	void Win10Window::OnUpdate()
	{
		FE_PROFILER_FUNC();

		// events polling
		{
			FE_PROFILER_SCOPE("GLFW events polling");
			glfwPollEvents();
		}
		
		m_CurrentRenderingContext->SwapBuffers();
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

			if (!glfwInit())
			{
				FE_CORE_ASSERT(false, "GLFW initialization failed!");
			}

			glfwSetErrorCallback(GLFWErrorCallback);
			s_GLFWInitialized = true;
		}

		// window creation
		{
			FE_PROFILER_SCOPE("GLFW_CreateWindow");
			FE_LOG_CORE_INFO("Creating Window {0} ({1}, {2})", attr.Title, attr.Width, attr.Height);

			m_Window = glfwCreateWindow((int)attr.Width, (int)attr.Height, attr.Title.c_str(), nullptr, nullptr);
			s_GLFWWindowCount++;
		}		

		glfwSetWindowUserPointer(m_Window, &m_Data);

		SetGLFWEventsCallbacks();
	}

	void Win10Window::SetGLFWEventsCallbacks()
	{
		FE_PROFILER_FUNC();

		FE_LOG_CORE_DEBUG("Setting events callbacks for window.");

		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				WinData& data = *(WinData*)glfwGetWindowUserPointer(window); // data = m_Data
				data.Width = width;
				data.Height = height;

				FE_NEW_EVENT(data.EventCallback, event, Events::WindowResizeEvent, width, height);
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WinData& data = *(WinData*)glfwGetWindowUserPointer(window); // data = m_Data
				FE_NEW_EVENT(data.EventCallback, event, Events::WindowCloseEvent);
				//FE_LOG_CORE_TRACE("EVENT CREATION");
				//FE_LOG_CORE_TRACE(event.GetEventType());
			});

		glfwSetWindowFocusCallback(m_Window, [](GLFWwindow* window, int focus)
			{
				WinData& data = *(WinData*)glfwGetWindowUserPointer(window); // data = m_Data
				if (focus)
				{
					FE_NEW_EVENT(data.EventCallback, event, Events::WindowGainedFocusEvent);
				}
				else
				{
					FE_NEW_EVENT(data.EventCallback, event, Events::WindowLostFocusEvent);
				}
			});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int modes)
			{
				WinData& data = *(WinData*)glfwGetWindowUserPointer(window); // data = m_Data

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

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)
			{
				WinData& data = *(WinData*)glfwGetWindowUserPointer(window); // data = m_Data
				FE_NEW_EVENT(data.EventCallback, event, Events::KeyTypedEvent, keycode);
			});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				WinData& data = *(WinData*)glfwGetWindowUserPointer(window); // data = m_Data

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

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double x_offset, double y_offset)
			{
				WinData& data = *(WinData*)glfwGetWindowUserPointer(window); // data = m_Data
				FE_NEW_EVENT(data.EventCallback, event, Events::MouseScrolledEvent, (float)x_offset, (float)y_offset);
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double x_position, double y_position)
			{
				WinData& data = *(WinData*)glfwGetWindowUserPointer(window); // data = m_Data
				FE_NEW_EVENT(data.EventCallback, event, Events::MouseMovedEvent, (float)x_position, (float)y_position);
			});

		FE_LOG_CORE_INFO("Events callbacks for Win10Window setted up.");
	}

	void Win10Window::ShutDown()
	{
		FE_PROFILER_FUNC();
		FE_LOG_CORE_INFO("Closing Window.");

		{
			FE_PROFILER_SCOPE("glfwDestroyWindow()");
			glfwDestroyWindow(m_Window);
			s_GLFWWindowCount--;
		}

		// TO DO: windows counting system to manage glfwInit() and glfwTerminate()
		if (s_GLFWWindowCount == 0)
		{
			FE_PROFILER_SCOPE("glfwTerminate()");
			glfwTerminate();
		}
	}

	void Win10Window::SetVSync(bool enabled)
	{
		FE_PROFILER_FUNC();

		if (enabled)
		{
			glfwSwapInterval(1);
			FE_LOG_CORE_INFO("VSync enablled.");
		}
		else
		{
			glfwSwapInterval(0);
			FE_LOG_CORE_INFO("VSync disabled.");
		}
		m_Data.VSync = enabled;
	}
}