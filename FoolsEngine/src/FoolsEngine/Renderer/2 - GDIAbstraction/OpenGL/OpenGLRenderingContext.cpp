#include "FE_pch.h"
#include "OpenGLRenderingContext.h"

#include <map>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

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
		static std::map<GLenum, const GLchar*> Sources =
		{
			{GL_DEBUG_SOURCE_API, "API"},
			{GL_DEBUG_SOURCE_WINDOW_SYSTEM, "WINDOW_SYSTEM"},
			{GL_DEBUG_SOURCE_SHADER_COMPILER, "SHADER_COMPILER"},
			{GL_DEBUG_SOURCE_THIRD_PARTY, "THIRD_PARTY"},
			{GL_DEBUG_SOURCE_APPLICATION, "APPLICATION"},
			{GL_DEBUG_SOURCE_OTHER, "OTHER"}
		};

		static std::map<GLenum, const GLchar*> Types =
		{
			{GL_DEBUG_TYPE_ERROR, "ERROR"},
			{GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR, "DEPRECATED_BEHAVIOR"},
			{GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR, "UNDEFINED_BEHAVIOR"},
			{GL_DEBUG_TYPE_PORTABILITY, "PORTABILITY"},
			{GL_DEBUG_TYPE_PERFORMANCE, "PERFORMANCE"},
			{GL_DEBUG_TYPE_MARKER, "MARKER"},
			{GL_DEBUG_TYPE_PUSH_GROUP, "PUSH_GROUP"},
			{GL_DEBUG_TYPE_POP_GROUP, "POP_GROUP"},
			{GL_DEBUG_TYPE_OTHER, "OTHER"}
		};

		std::string logMessage = "OpenGL Message: ";
		logMessage += message;
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:         FE_LOG_CORE_FATAL(logMessage); break;
		case GL_DEBUG_SEVERITY_MEDIUM:       FE_LOG_CORE_ERROR(logMessage); break;
		case GL_DEBUG_SEVERITY_LOW:          FE_LOG_CORE_WARN(logMessage);  break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: FE_LOG_CORE_DEBUG(logMessage); break;
		}
		FE_LOG_CORE_DEBUG("Source: {0}, Type: {1}, ID: {2}", Sources[source], Types[type], id);

		FE_ASSERTION_BREAK();
	}

	OpenGLRenderingContext::OpenGLRenderingContext(void* window) :
		RenderingContext(GDIType::OpenGL, window)
	{
		FE_PROFILER_FUNC();
		FE_CORE_ASSERT(window, "Window pointer is null during OpenGLContext creation!");
	}

	void OpenGLRenderingContext::Init()
	{
		FE_PROFILER_FUNC();

		FE_CORE_ASSERT(m_ID != NullRenderingContextID && m_Type != GDIType::None && m_Window, "");

		glfwMakeContextCurrent(Window());
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		FE_CORE_ASSERT(status, "Failed to initialize glad - modern OpenGL loader!");

		FE_LOG_CORE_INFO("Rendering platform info:");
		FE_LOG_CORE_INFO("	Vendor:		{0}", (const char*)glGetString(GL_VENDOR));
		FE_LOG_CORE_INFO("	Renderer:	{0}", (const char*)glGetString(GL_RENDERER));
		FE_LOG_CORE_INFO("	Version:	{0}", (const char*)glGetString(GL_VERSION));
		//GL_PROF

		FE_CORE_ASSERT(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5), "Minimal required OpenGL version is 4.5!");

#ifdef FE_INTERNAL_BUILD
		glDebugMessageCallback(OpenGLMessageCallback, nullptr);
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_TRUE); 
#endif // FE_INTERNAL_BUILD

		FE_LOG_CORE_INFO("OpenGL Rendering Context created.");
	}

	void OpenGLRenderingContext::SwapBuffers()
	{
		FE_PROFILER_FUNC();

		glfwSwapBuffers(Window());
	}
}