#pragma once

namespace fe {
	bool FE_ENABLE_ASSERTIONS = false;
	bool FE_ENABLE_CORE_ASSERTIONS = false;
}

#ifdef FE_PLATFORM_WINDOWS



extern fe::Application* fe::CreateApplication();

int main(int argc, char** argv)
{
	fe::Log::Init();
	
#ifdef FE_INTERNAL_BUILD
	fe::Tester::Test();
#endif // FE_INTERNAL_BUILD

	FE_PROFILER_SESSION_START("Startup", "Logs/ProfileData_Startup.json");
	auto app = fe::CreateApplication();
	FE_PROFILER_SESSION_END();

	FE_PROFILER_SESSION_START("Runtime", "Logs/ProfileData_Runtime.json");
	app->Run();
	FE_PROFILER_SESSION_END();

	FE_PROFILER_SESSION_START("Shutdown", "Logs/ProfileData_Shutdown.json");
	delete app;
	FE_PROFILER_SESSION_END();
}

#endif
