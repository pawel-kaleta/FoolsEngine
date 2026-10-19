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

	InitLog().Init();

	fe::Context::Log::Core.Trace("TEST"_FE);
	{
		fe::Context::Log::ScopeCore aaa("Prefix"_FE);
		fe::Context::Log::ScopeCore ccc(""_FE);

		fe::Context::Log::Core.Trace("TEST 2 {}"_FE, "ty");

		{
			fe::Context::Log::ScopeCore  bbb("asd"_FE, fe::LoggingLevel::Debug);

			fe::Context::Log::Core.Trace("TEST 3 {}"_FE, "sdf");
			fe::Context::Log::Core.Debug("TEST 4 {} {}"_FE, "ererer", 67);
			fe::Context::Log::Core.Trace("TEST 3 xxxm"_FE);
		}
	}

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
