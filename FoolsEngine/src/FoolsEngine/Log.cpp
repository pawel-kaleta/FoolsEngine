#include "Log.h"
#include <vector>

namespace fe
{
	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

	void Log::Init()
	{
		// https://github.com/gabime/spdlog/wiki/3.-Custom-formatting

		std::vector<spdlog::sink_ptr> loggingTargets;

		loggingTargets.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		loggingTargets.back()->set_pattern("%^[%T:%f] %-3n: %v%$");

		loggingTargets.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("Logs/FoolsEngine.log", true));
		loggingTargets.back()->set_pattern("[%T:%f] %-8l %-3n: %v");

		s_CoreLogger   = std::make_shared<spdlog::logger>("FE", begin(loggingTargets), end(loggingTargets));
		s_ClientLogger = std::make_shared<spdlog::logger>("App", begin(loggingTargets), end(loggingTargets));

		spdlog::register_logger(s_CoreLogger);
		spdlog::register_logger(s_ClientLogger);

		s_CoreLogger  ->set_level(spdlog::level::trace);
		s_ClientLogger->set_level(spdlog::level::trace);

		s_CoreLogger  ->flush_on(spdlog::level::trace);
		s_ClientLogger->flush_on(spdlog::level::trace);

		FE_LOG_CORE_INFO("Logging innitialized!");
		FE_LOG_CLIENT_INFO("Logging innitialized!");
	}

	void Log::SetCoreLoggingLevel(int lvl)
	{
		spdlog::level::level_enum currentLoggingLvl = s_CoreLogger->level();
		s_CoreLogger->set_level(spdlog::level::trace);
		switch (lvl)
		{
			case 0:
				FE_LOG_CORE_INFO("Logging level set to 0 - all.");
				s_CoreLogger->set_level(spdlog::level::trace);
				break;
			case 1:
				FE_LOG_CORE_INFO("Logging level set to 1 - debug or higher.");
				s_CoreLogger->set_level(spdlog::level::debug);
				break;
			case 2:
				FE_LOG_CORE_INFO("Logging level set to 2 - info or higher.");
				s_CoreLogger->set_level(spdlog::level::info);
				break;
			case 3:
				FE_LOG_CORE_INFO("Logging level set to 3 - warning or higher.");
				s_CoreLogger->set_level(spdlog::level::warn);
				break;
			case 4:
				FE_LOG_CORE_INFO("Logging level set to 4 - error or higher.");
				s_CoreLogger->set_level(spdlog::level::err);
				break;
			case 5:
				FE_LOG_CORE_INFO("Logging level set to 5 - fatal error or higher.");
				s_CoreLogger->set_level(spdlog::level::critical);
				break;
			case 6:
				FE_LOG_CORE_INFO("Logging level set to 6 - nothing.");
				s_CoreLogger->set_level(spdlog::level::off);
				break;
			default:
				FE_LOG_CORE_ERROR("Unrecognized lvl of logging! Possible values: 0-6. Received value:");
				FE_LOG_CORE_ERROR(lvl);
				s_CoreLogger->set_level(currentLoggingLvl);
				break;
		}
	}

	void Log::SetClientLoggingLevel(int lvl)
	{
		spdlog::level::level_enum currentLoggingLvl = s_ClientLogger->level();
		s_ClientLogger->set_level(spdlog::level::trace);
		switch (lvl)
		{
		case 0:
			FE_LOG_CLIENT_INFO("Logging level set to 0 - all.");
			s_ClientLogger->set_level(spdlog::level::trace);
			break;
		case 1:
			FE_LOG_CLIENT_INFO("Logging level set to 1 - debug or higher.");
			s_ClientLogger->set_level(spdlog::level::debug);
			break;
		case 2:
			FE_LOG_CLIENT_INFO("Logging level set to 2 - info or higher.");
			s_ClientLogger->set_level(spdlog::level::info);
			break;
		case 3:
			FE_LOG_CLIENT_INFO("Logging level set to 3 - warning or higher.");
			s_ClientLogger->set_level(spdlog::level::warn);
			break;
		case 4:
			FE_LOG_CLIENT_INFO("Logging level set to 4 - error or higher.");
			s_ClientLogger->set_level(spdlog::level::err);
			break;
		case 5:
			FE_LOG_CLIENT_INFO("Logging level set to 5 - fatal error or higher.");
			s_ClientLogger->set_level(spdlog::level::critical);
			break;
		case 6:
			FE_LOG_CLIENT_INFO("Logging level set to 6 - nothing.");
			s_ClientLogger->set_level(spdlog::level::off);
			break;
		default:
			FE_LOG_CLIENT_ERROR("Unrecognized lvl of logging! Possible values: 0-6. Received value:");
			FE_LOG_CLIENT_ERROR(lvl);
			s_ClientLogger->set_level(currentLoggingLvl);
			break;
		}
	}

	void Log::UnitTest()
	{
		FE_LOG_CORE_DEBUG("--------------------------------------------------------");
		FE_LOG_CORE_DEBUG("----------------------Logging test----------------------");
		FE_LOG_CORE_FATAL("Fatal test");
		FE_LOG_CLIENT_FATAL("Fatal test");
		FE_LOG_CORE_ERROR("Error test");
		FE_LOG_CLIENT_ERROR("Error test");
		FE_LOG_CORE_WARN("Warn test");
		FE_LOG_CLIENT_WARN("Warn test");
		FE_LOG_CORE_INFO("Info test");
		FE_LOG_CLIENT_INFO("Info test");
		FE_LOG_CORE_DEBUG("Debug test");
		FE_LOG_CLIENT_DEBUG("Debug test");
		FE_LOG_CORE_TRACE("Trace test");
		FE_LOG_CLIENT_TRACE("Trace test");
		fe::Log::SetCoreLoggingLevel(5);
		fe::Log::SetCoreLoggingLevel(10);
		fe::Log::SetCoreLoggingLevel(0);
		fe::Log::SetClientLoggingLevel(4);
		fe::Log::SetClientLoggingLevel(11);
		fe::Log::SetClientLoggingLevel(0);
		FE_LOG_CORE_DEBUG("----------------------Logging test----------------------");
		FE_LOG_CORE_DEBUG("--------------------------------------------------------");
	}
}