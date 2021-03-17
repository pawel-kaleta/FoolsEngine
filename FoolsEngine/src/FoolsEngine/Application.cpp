#include "FE_pch.h"
#include "FoolsEngine/Application.h"
#include "Debug/Profiler.h"

namespace fe {

	Application::Application()
	{
		FE_PROFILER_SCOPE_FUNC();
	}

	Application::~Application()
	{
		FE_PROFILER_SCOPE_FUNC();
	}

	void Application::Run()
	{
		FE_PROFILER_SCOPE_FUNC();
		while (true);
	}
}