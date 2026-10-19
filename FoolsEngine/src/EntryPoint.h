#pragma once

#include "FoolsEngine/Foundation/Debug/Profiler.h"

#include "FoolsEngine/Application/Application.h"

#include "FoolsEngine/Foundation/Utils/Context.h"
#include "FoolsEngine/Foundation/Memory/String.h"

#include "FoolsEngine/Foundation/Utils/Init.h"

#ifdef FE_PLATFORM_WINDOWS

namespace fe
{
	extern Application* CreateApplication(const ApplicationCommandLineArgs& args);
}

int main(int argc, char** argv)
{
	using namespace fe;

	InitLog().Init(24 * 1024);

	//fe::Log::Init();

	fe::Application* app;
	{
		Context::Log::ScopeCore startup("STARTUP"_FE, LoggingLevel::Trace);
	
		FE_PROFILER_SESSION_START("Startup", "Logs/ProfileData_Startup.json");
		app = fe::CreateApplication({ argc, argv });
		app->Startup();
		FE_PROFILER_SESSION_END();
	}

	{
		Context::Log::ScopeCore startup("RUN"_FE, LoggingLevel::Debug);

		app->Run();
	}

	{
		Context::Log::ScopeCore startup("SHUTDOWN"_FE, LoggingLevel::Trace);

		FE_PROFILER_SESSION_START("Shutdown", "Logs/ProfileData_Shutdown.json");
		app->ShutDown();
		delete app;
		FE_PROFILER_SESSION_END();
	}
}

#endif
