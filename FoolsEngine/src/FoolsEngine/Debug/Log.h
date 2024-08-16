#pragma once

//#include "FoolsEngine/Core/Core.h"

// https://github.com/gabime/spdlog/wiki
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/fmt/ostr.h>

namespace fe {

	class Log
	{
	public:
		static void Init();

		static std::shared_ptr<spdlog::logger> GetCoreLogger()   { return s_CoreLogger; }
		static std::shared_ptr<spdlog::logger> GetClientLogger() { return s_ClientLogger; }

		static void SetCoreLoggingLevel(int lvl);
		static void SetClientLoggingLevel(int lvl);

		static spdlog::level::level_enum GetCoreLoggingLevel() { return s_CoreLogger->level(); }
		static spdlog::level::level_enum GetClientLoggingLevel() { return s_ClientLogger->level(); }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;

		Log();
	};
}

// TO DO: Local logger
// 
// auto localLogger = FE_LOCALLOG_CORE_INIT("MyLocalLogger");
// FE_LOCALLOG_CORE_DEBUG("MyLocalLogger", ...)


// Macros for logging core engine issues
#define FE_LOG_CORE_FATAL(...) fe::Log::GetCoreLogger()->critical(__VA_ARGS__)
#define FE_LOG_CORE_ERROR(...) fe::Log::GetCoreLogger()->error   (__VA_ARGS__)
#define FE_LOG_CORE_WARN(...)  fe::Log::GetCoreLogger()->warn    (__VA_ARGS__)
#define FE_LOG_CORE_INFO(...)  fe::Log::GetCoreLogger()->info    (__VA_ARGS__)
#define FE_LOG_CORE_DEBUG(...) fe::Log::GetCoreLogger()->debug   (__VA_ARGS__)
#define FE_LOG_CORE_TRACE(...) fe::Log::GetCoreLogger()->trace   (__VA_ARGS__)

// Macros for logging client app issues
#define FE_LOG_FATAL(...) fe::Log::GetClientLogger()->critical(__VA_ARGS__)
#define FE_LOG_ERROR(...) fe::Log::GetClientLogger()->error   (__VA_ARGS__)
#define FE_LOG_WARN(...)  fe::Log::GetClientLogger()->warn    (__VA_ARGS__)
#define FE_LOG_INFO(...)  fe::Log::GetClientLogger()->info    (__VA_ARGS__)
#define FE_LOG_DEBUG(...) fe::Log::GetClientLogger()->debug   (__VA_ARGS__)
#define FE_LOG_TRACE(...) fe::Log::GetClientLogger()->trace   (__VA_ARGS__)
