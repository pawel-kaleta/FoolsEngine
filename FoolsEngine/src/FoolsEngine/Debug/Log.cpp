#include "FE_pch.h"
#include "FoolsEngine/Debug/Log.h"

namespace fe
{
	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

	void Log::Init()
	{
		// https://github.com/gabime/spdlog/wiki

		std::vector<spdlog::sink_ptr> loggingTargets;

#ifdef FE_INTERNAL_BUILD
		loggingTargets.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		loggingTargets.back()->set_pattern("%^%T:%f | %-3n | %v%$");
#endif // FE_INTERNAL_BUILD

		loggingTargets.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("Logs/FoolsEngine.log", true));
		loggingTargets.back()->set_pattern("%T:%f | %-8l | %-3n | %v");

		s_CoreLogger   = std::make_shared<spdlog::logger>("FE", begin(loggingTargets), end(loggingTargets));
		s_ClientLogger = std::make_shared<spdlog::logger>("App", begin(loggingTargets), end(loggingTargets));

		spdlog::register_logger(s_CoreLogger);
		spdlog::register_logger(s_ClientLogger);

#ifdef FE_DEBUG
		s_CoreLogger  ->set_level(spdlog::level::trace);
		s_ClientLogger->set_level(spdlog::level::trace);
#endif // DEBUG

#ifdef FE_RELEASE
		s_CoreLogger->set_level(spdlog::level::debug);
		s_ClientLogger->set_level(spdlog::level::trace);
#endif // RELEASE

#ifdef FE_PUBLISH
		s_CoreLogger->set_level(spdlog::level::info);
		s_ClientLogger->set_level(spdlog::level::trace);
#endif // PUBLISH

		s_CoreLogger  ->flush_on(spdlog::level::trace);
		s_ClientLogger->flush_on(spdlog::level::trace);

		FE_LOG_CORE_INFO("Logging innitialized!");
		FE_LOG_INFO("Logging innitialized!");
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
				FE_LOG_CORE_ERROR("Unrecognized lvl of logging! Possible values: 0-6. Received value: lvl={0}", lvl);
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
			FE_LOG_INFO("Logging level set to 0 - all.");
			s_ClientLogger->set_level(spdlog::level::trace);
			break;
		case 1:
			FE_LOG_INFO("Logging level set to 1 - debug or higher.");
			s_ClientLogger->set_level(spdlog::level::debug);
			break;
		case 2:
			FE_LOG_INFO("Logging level set to 2 - info or higher.");
			s_ClientLogger->set_level(spdlog::level::info);
			break;
		case 3:
			FE_LOG_INFO("Logging level set to 3 - warning or higher.");
			s_ClientLogger->set_level(spdlog::level::warn);
			break;
		case 4:
			FE_LOG_INFO("Logging level set to 4 - error or higher.");
			s_ClientLogger->set_level(spdlog::level::err);
			break;
		case 5:
			FE_LOG_INFO("Logging level set to 5 - fatal error or higher.");
			s_ClientLogger->set_level(spdlog::level::critical);
			break;
		case 6:
			FE_LOG_INFO("Logging level set to 6 - nothing.");
			s_ClientLogger->set_level(spdlog::level::off);
			break;
		default:
			FE_LOG_ERROR("Unrecognized lvl of logging! Possible values: 0-6. Received value: lvl={0}", lvl);
			s_ClientLogger->set_level(currentLoggingLvl);
			break;
		}
	}

}