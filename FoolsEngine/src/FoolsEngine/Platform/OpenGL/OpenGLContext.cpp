#include "FE_pch.h"
#include "OpenGLContext.h"

#include <glad/glad.h>

namespace fe
{
	OpenGLContext::OpenGLContext(GLFWwindow* window)
		: m_Window(window)
	{
		FE_CORE_ASSERT(window, "Window pointer is null during OpenGLContext construction!");
	}

	void OpenGLContext::Init()
	{
		FE_PROFILER_FUNC();

		glfwMakeContextCurrent(m_Window);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		FE_CORE_ASSERT(status, "Failed to initialize glad - modern OpenGL loader!");
	}

	void OpenGLContext::SwapBuffers()
	{
		FE_PROFILER_FUNC();

		glfwSwapBuffers(m_Window);
	}

	void OpenGLContext::ClearBuffer()
	{
		FE_PROFILER_FUNC();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
}