#include "FE_pch.h"
#include "Log.h"

// https://github.com/gabime/spdlog/wiki

namespace fe
{
	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

	void Log::Init()
	{
		std::vector<spdlog::sink_ptr> loggingTargets;

#ifdef FE_DEBUG
		loggingTargets.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		loggingTargets.back()->set_pattern("%^%M:%S:%f | %-3n | %-8l | %v%$");
#endif // FE_DEBUG

		loggingTargets.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("Logs/FoolsEngine.log", true));
		loggingTargets.back()->set_pattern("%T:%f | %-3n | %-8l | %v");

		s_CoreLogger   = std::make_shared<spdlog::logger>("FE", begin(loggingTargets), end(loggingTargets));
		s_ClientLogger = std::make_shared<spdlog::logger>("App", begin(loggingTargets), end(loggingTargets));

		spdlog::register_logger(s_CoreLogger);
		spdlog::register_logger(s_ClientLogger);

#ifdef FE_DEBUG
		s_CoreLogger->set_level(spdlog::level::debug);
		s_ClientLogger->set_level(spdlog::level::debug);
#endif // FE_DEBUG

#ifdef FE_RELEASE
		s_CoreLogger->set_level(spdlog::level::info);
		s_ClientLogger->set_level(spdlog::level::info);
#endif // FE_RELEASE

#ifdef FE_PUBLISH
		s_CoreLogger->set_level(spdlog::level::warn);
		s_ClientLogger->set_level(spdlog::level::warn);
#endif // FE_PUBLISH

		s_CoreLogger  ->flush_on(spdlog::level::trace);
		s_ClientLogger->flush_on(spdlog::level::trace);

		FE_LOG_CORE_INFO("Logging innitialized!");
		FE_LOG_INFO("Logging innitialized!");
	}

	void Log::SetCoreLoggingLevel(LoggingLevel lvl)
	{
		spdlog::level::level_enum currentLoggingLvl = s_CoreLogger->level();
		switch (lvl)
		{
		case LoggingLevel::Trace:
			s_CoreLogger->set_level(spdlog::level::trace);
			break;
		case LoggingLevel::Debug:
			s_CoreLogger->set_level(spdlog::level::debug);
			break;
		case LoggingLevel::Info:
			s_CoreLogger->set_level(spdlog::level::info);
			break;
		case LoggingLevel::Warn:
			s_CoreLogger->set_level(spdlog::level::warn);
			break;
		case LoggingLevel::Error:
			s_CoreLogger->set_level(spdlog::level::err);
			break;
		case LoggingLevel::Fatal:
			s_CoreLogger->set_level(spdlog::level::critical);
			break;
		case LoggingLevel::None:
			s_CoreLogger->set_level(spdlog::level::off);
			break;
		default:
			FE_LOG_CORE_ERROR("Unrecognized lvl of logging! Possible values: 0-6. Received value: lvl={0}", lvl.ToInt());
			s_CoreLogger->set_level(currentLoggingLvl);
			break;
		}
	}

	void Log::SetClientLoggingLevel(LoggingLevel lvl)
	{
		spdlog::level::level_enum currentLoggingLvl = s_ClientLogger->level();
		switch (lvl)
		{
		case LoggingLevel::Trace:
			s_ClientLogger->set_level(spdlog::level::trace);
			break;
		case LoggingLevel::Debug:
			s_ClientLogger->set_level(spdlog::level::debug);
			break;
		case LoggingLevel::Info:
			s_ClientLogger->set_level(spdlog::level::info);
			break;
		case LoggingLevel::Warn:
			s_ClientLogger->set_level(spdlog::level::warn);
			break;
		case LoggingLevel::Error:
			s_ClientLogger->set_level(spdlog::level::err);
			break;
		case LoggingLevel::Fatal:
			s_ClientLogger->set_level(spdlog::level::critical);
			break;
		case LoggingLevel::None:
			s_ClientLogger->set_level(spdlog::level::off);
			break;
		default:
			FE_LOG_ERROR("Unrecognized lvl of logging! Received value: lvl={0}", lvl.ToInt());
			s_ClientLogger->set_level(currentLoggingLvl);
			break;
		}
	}

	Log::LoggingLevel Log::GetCoreLoggingLevel()
	{
		auto lvl = s_CoreLogger->level();
		switch (lvl)
		{
		case spdlog::level::trace:		return LoggingLevel::Trace;
		case spdlog::level::debug:		return LoggingLevel::Debug;
		case spdlog::level::info:		return LoggingLevel::Info;
		case spdlog::level::warn:		return LoggingLevel::Warn;
		case spdlog::level::err:		return LoggingLevel::Error;
		case spdlog::level::critical:	return LoggingLevel::Fatal;
		case spdlog::level::off:		return LoggingLevel::None;
		default:
			FE_CORE_ASSERT(false, "What?");
			return LoggingLevel::None;
		}
	}

	Log::LoggingLevel Log::GetClientLoggingLevel()
	{
		auto lvl = s_ClientLogger->level();
		switch (lvl)
		{
		case spdlog::level::trace:		return LoggingLevel::Trace;
		case spdlog::level::debug:		return LoggingLevel::Debug;
		case spdlog::level::info:		return LoggingLevel::Info;
		case spdlog::level::warn:		return LoggingLevel::Warn;
		case spdlog::level::err:		return LoggingLevel::Error;
		case spdlog::level::critical:	return LoggingLevel::Fatal;
		case spdlog::level::off:		return LoggingLevel::None;
		default:
			FE_CORE_ASSERT(false, "What?");
			return LoggingLevel::None;
		}
	}

}