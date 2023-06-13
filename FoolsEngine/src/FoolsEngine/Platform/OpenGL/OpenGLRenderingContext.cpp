#include "FE_pch.h"
#include <glad/glad.h>
#include "OpenGLRenderingContext.h"
#include "OpenGLRendererAPI.h"

namespace fe
{
	void OpenGLMessageCallback(
		GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		const void* userParam)
	{
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:         FE_LOG_CORE_FATAL(message); return;
		case GL_DEBUG_SEVERITY_MEDIUM:       FE_LOG_CORE_ERROR(message); return;
		case GL_DEBUG_SEVERITY_LOW:          FE_LOG_CORE_WARN(message); return;
		case GL_DEBUG_SEVERITY_NOTIFICATION: FE_LOG_CORE_DEBUG(message); return;
		}
	}

	OpenGLRenderingContext::OpenGLRenderingContext(GLFWwindow* window)
		: m_Window(window), m_GDI(GDIType::OpenGL)
	{
		FE_PROFILER_FUNC();
		FE_CORE_ASSERT(window, "Window pointer is null during OpenGLContext construction!");
	}

	void OpenGLRenderingContext::Init()
	{
		FE_PROFILER_FUNC();

		glfwMakeContextCurrent(m_Window);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		FE_CORE_ASSERT(status, "Failed to initialize glad - modern OpenGL loader!");

		FE_LOG_CORE_INFO("Rendering platform info:");
		FE_LOG_CORE_INFO("	Vendor:		{0}", (const char*)glGetString(GL_VENDOR));
		FE_LOG_CORE_INFO("	Renderer:	{0}", (const char*)glGetString(GL_RENDERER));
		FE_LOG_CORE_INFO("	Version:	{0}", (const char*)glGetString(GL_VERSION));

		FE_CORE_ASSERT(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5), "Minimal required OpenGL version is 4.5!");

#ifdef FE_INTERNAL_BUILD
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(OpenGLMessageCallback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE); 
#endif // FE_INTERNAL_BUILD

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