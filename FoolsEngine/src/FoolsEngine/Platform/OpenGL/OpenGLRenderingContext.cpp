#include "FE_pch.h"
#include <glad/glad.h>
#include "OpenGLRenderingContext.h"
#include "OpenGLRendererAPI.h"

namespace fe
{
	OpenGLRenderingContext::OpenGLRenderingContext(GLFWwindow* window)
		: m_Window(window)
	{
		FE_PROFILER_FUNC();
		FE_CORE_ASSERT(window, "Window pointer is null during OpenGLContext construction!");
	}

	void GLAPIENTRY
		MessageCallback(GLenum source,
			GLenum type,
			GLuint id,
			GLenum severity,
			GLsizei length,
			const GLchar* message,
			const void* userParam)
	{
		fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
			(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
			type, severity, message);
	}

	void OpenGLRenderingContext::Init()
	{
		FE_PROFILER_FUNC();

		glfwMakeContextCurrent(m_Window);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		FE_CORE_ASSERT(status, "Failed to initialize glad - modern OpenGL loader!");

		FE_LOG_CORE_INFO("Rendering platform info:");
		FE_LOG_CORE_INFO("	Vendor:		{0}", glGetString(GL_VENDOR));
		FE_LOG_CORE_INFO("	Renderer:	{0}", glGetString(GL_RENDERER));
		FE_LOG_CORE_INFO("	Version:	{0}", glGetString(GL_VERSION));

		// During init, enable debug output
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(MessageCallback, 0);

		FE_LOG_CORE_INFO("OpenGL Rendering Context created.");
	}

	void OpenGLRenderingContext::MakeCurrent()
	{
		FE_CORE_ASSERT(false, "Multiple rendering contexts not supported!");
	}

	void OpenGLRenderingContext::SwapBuffers()
	{
		FE_PROFILER_FUNC();

		glfwSwapBuffers(m_Window);
	}
}