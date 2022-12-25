#include "FE_pch.h"
#include <glad/glad.h>
#include "OpenGLContext.h"
#include "OpenGLRendererAPI.h"

namespace fe
{
	OpenGLContext::OpenGLContext(GLFWwindow* window)
		: m_Window(window)
	{
		FE_PROFILER_FUNC();
		FE_CORE_ASSERT(window, "Window pointer is null during OpenGLContext construction!");
	}

	void OpenGLContext::Init()
	{
		FE_PROFILER_FUNC();

		glfwMakeContextCurrent(m_Window);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		FE_CORE_ASSERT(status, "Failed to initialize glad - modern OpenGL loader!");

		FE_LOG_CORE_INFO("Rendering platform info:");
		FE_LOG_CORE_INFO("	Vendor:		{0}", glGetString(GL_VENDOR));
		FE_LOG_CORE_INFO("	Renderer:	{0}", glGetString(GL_RENDERER));
		FE_LOG_CORE_INFO("	Version:	{0}", glGetString(GL_VERSION));

		Renderer::SetAPI(RendererAPI::NativeAPI::OpenGL);

		FE_LOG_CORE_INFO("OpenGLRenderAPI initialized.");
	}

	void OpenGLContext::SwapBuffers()
	{
		FE_PROFILER_FUNC();

		glfwSwapBuffers(m_Window);
	}
}