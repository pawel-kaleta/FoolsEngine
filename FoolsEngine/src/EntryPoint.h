#pragma once

#include "FoolsEngine/Foundation/Debug/Profiler.h"

#include "FoolsEngine/Application/Application.h"

#ifdef FE_PLATFORM_WINDOWS

namespace fe
{
	extern Application* CreateApplication(const ApplicationCommandLineArgs& args);
}

int main(int argc, char** argv)
{
	fe::Log::Init();
	
	FE_LOG_CORE_INFO("/////////////////////////////////////////////////////");
	FE_LOG_CORE_INFO("////////////////////// STARTUP //////////////////////");
	FE_LOG_CORE_INFO("/////////////////////////////////////////////////////");

	fe::Log::SetClientLoggingLevel(fe::Log::LoggingLevel::Trace);
	fe::Log::SetCoreLoggingLevel(fe::Log::LoggingLevel::Trace);

	FE_PROFILER_SESSION_START("Startup", "Logs/ProfileData_Startup.json");
	auto app = fe::CreateApplication({ argc, argv });
	app->Startup();
	FE_PROFILER_SESSION_END();

	FE_LOG_CORE_INFO("/////////////////////////////////////////////////////");
	FE_LOG_CORE_INFO("////////////////////// RUNTIME //////////////////////");
	FE_LOG_CORE_INFO("/////////////////////////////////////////////////////");

	fe::Log::SetClientLoggingLevel(fe::Log::LoggingLevel::Debug);
	fe::Log::SetCoreLoggingLevel(fe::Log::LoggingLevel::Debug);

	app->Run();

	FE_LOG_CORE_INFO("/////////////////////////////////////////////////////");
	FE_LOG_CORE_INFO("///////////////////// SHUTDOWN //////////////////////");
	FE_LOG_CORE_INFO("/////////////////////////////////////////////////////");

	fe::Log::SetClientLoggingLevel(fe::Log::LoggingLevel::Trace);
	fe::Log::SetCoreLoggingLevel(fe::Log::LoggingLevel::Trace);

	FE_PROFILER_SESSION_START("Shutdown", "Logs/ProfileData_Shutdown.json");
	app->ShutDown();
	delete app;
	FE_PROFILER_SESSION_END();
}

#endif
