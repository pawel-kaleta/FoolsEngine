#include "FE_pch.h"
#include "FoolsEngine/Application.h"
#include "Debug/Profiler.h"

#include "GLFW/glfw3.h"

namespace fe {

	Application::Application()
	{
		FE_PROFILER_FUNC();

		if (!glfwInit())
		{
			FE_LOG_CORE_FATAL("GLFW failed to initialise!");
			// Initialization failed
		}
	}

	Application::~Application()
	{
		FE_PROFILER_FUNC();

		glfwTerminate();
	}

	void Application::Run()
	{
		FE_PROFILER_FUNC();
		
		GLFWwindow* window = glfwCreateWindow(640, 480, "TEST TITLE", NULL, NULL);
		if (!window)
		{
			FE_LOG_CORE_FATAL("GLFW failed to create window or OpenGL context!");
		}

		while (true);
	}
}