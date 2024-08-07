#pragma once

#ifdef FE_PLATFORM_WINDOWS

namespace fe
{
	extern Application* CreateApplication();
}

int main(int argc, char** argv)
{
	fe::Log::Init();
	
	FE_LOG_CORE_INFO("/////////////////////////////////////////////////////");
	FE_LOG_CORE_INFO("////////////////////// STARTUP //////////////////////");
	FE_LOG_CORE_INFO("/////////////////////////////////////////////////////");

	fe::Log::SetClientLoggingLevel(spdlog::level::trace);
	fe::Log::SetCoreLoggingLevel(spdlog::level::trace);

	FE_PROFILER_SESSION_START("Startup", "Logs/ProfileData_Startup.json");
	auto app = fe::CreateApplication();
	app->Startup();
	FE_PROFILER_SESSION_END();

	FE_LOG_CORE_INFO("/////////////////////////////////////////////////////");
	FE_LOG_CORE_INFO("////////////////////// RUNTIME //////////////////////");
	FE_LOG_CORE_INFO("/////////////////////////////////////////////////////");

	fe::Log::SetClientLoggingLevel(spdlog::level::debug);
	fe::Log::SetCoreLoggingLevel(spdlog::level::debug);

	app->Run();

	FE_LOG_CORE_INFO("/////////////////////////////////////////////////////");
	FE_LOG_CORE_INFO("///////////////////// SHUTDOWN //////////////////////");
	FE_LOG_CORE_INFO("/////////////////////////////////////////////////////");

	fe::Log::SetClientLoggingLevel(spdlog::level::trace);
	fe::Log::SetCoreLoggingLevel(spdlog::level::trace);

	FE_PROFILER_SESSION_START("Shutdown", "Logs/ProfileData_Shutdown.json");
	app->ShutDown();
	delete app;
	FE_PROFILER_SESSION_END();
}

#endif
