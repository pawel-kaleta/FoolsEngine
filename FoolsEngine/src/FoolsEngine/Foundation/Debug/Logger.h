#pragma once

#include "LoggingLevel.h"

#include "FoolsEngine/Foundation/Memory/String.h"
#include "FoolsEngine/Foundation/Utils/Core.h"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <memory>

namespace fe
{
	struct String;

	class Sink
	{

	};

	class EditorSink : public Sink
	{

	};

	class ConsoleSink : public Sink
	{

	};

	class FileSink : public Sink
	{

	};
	
	class Logger
	{
	public:
		std::shared_ptr<spdlog::logger> mLogger;

		LoggingLevel mLoggingLevel;
		char mSuffixes[256];
		UInt mSuffixesCount;

		void Init()
		{
			mSuffixes[255] = 'X';

			mLoggingLevel = LoggingLevel::Trace;
			mSuffixesCount = 0;

			auto loggingTarget = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
			loggingTarget->set_pattern("%^%M:%S:%f | %-3n | %v%$");

			mLogger = std::make_shared<spdlog::logger>("FE");
			mLogger->sinks().push_back(loggingTarget);

			mLogger->set_level(spdlog::level::trace);
		}

		template <typename... Args>
		void Trace(String msg, Args &&...args) {
#ifdef FE_INTERNAL_BUILD
			if (mLoggingLevel > LoggingLevel::Debug)
				return;

			UInt msg_begin = mSuffixesCount + 1;
			UInt copy_count = min(256 - msg_begin - 1, msg.Count); // 1 for '\0' at the end
			mSuffixes[mSuffixesCount] = ' ';
			std::memcpy(& mSuffixes[msg_begin], msg.Data, msg.Count);
			mSuffixes[msg_begin + copy_count] = '\0';

			mLogger->trace(spdlog::fmt_lib::runtime(mSuffixes), args...);
#endif // FE_INTERNAL_BUILD
		}

		template <typename... Args>
		void Debug(String msg, Args &&...args) {
#ifdef FE_INTERNAL_BUILD
			if (mLoggingLevel > LoggingLevel::Debug)
				return;

			UInt msg_begin = mSuffixesCount + 1;
			UInt copy_count = min(256 - msg_begin - 1, msg.Count); // 1 for '\0' at the end
			mSuffixes[mSuffixesCount] = ' ';
			std::memcpy(&mSuffixes[msg_begin], msg.Data, msg.Count);
			mSuffixes[msg_begin + copy_count] = '\0';

			mLogger->debug(spdlog::fmt_lib::runtime(mSuffixes), std::forward<Args>(args)...);
#endif // FE_INTERNAL_BUILD
		}
		
		template <typename... Args>
		void Info(String msg, Args &&...args) {
#ifdef FE_INTERNAL_BUILD
			if (mLoggingLevel > LoggingLevel::Info)
				return;

			UInt msg_begin = mSuffixesCount + 1;
			UInt copy_count = min(256 - msg_begin - 1, msg.Count); // 1 for '\0' at the end
			mSuffixes[mSuffixesCount] = ' ';
			std::memcpy(&mSuffixes[msg_begin], msg.Data, msg.Count);
			mSuffixes[msg_begin + copy_count] = '\0';

			mLogger->info(spdlog::fmt_lib::runtime(mSuffixes), args...);
#endif // FE_INTERNAL_BUILD
		}

		template <typename... Args>
		void Warn(String msg, Args &&...args) {
#ifdef FE_INTERNAL_BUILD
			if (mLoggingLevel > LoggingLevel::Warn)
				return;

			UInt msg_begin = mSuffixesCount + 1;
			UInt copy_count = min(256 - msg_begin - 1, msg.Count); // 1 for '\0' at the end
			mSuffixes[mSuffixesCount] = ' ';
			std::memcpy(&mSuffixes[msg_begin], msg.Data, msg.Count);
			mSuffixes[msg_begin + copy_count] = '\0';

			mLogger->warn(spdlog::fmt_lib::runtime(mSuffixes), args...);
#endif // FE_INTERNAL_BUILD
		}

		template <typename... Args>
		void Error(String msg, Args &&...args) {
#ifdef FE_INTERNAL_BUILD
			if (mLoggingLevel > LoggingLevel::Error)
				return;

			UInt msg_begin = mSuffixesCount + 1;
			UInt copy_count = min(256 - msg_begin - 1, msg.Count); // 1 for '\0' at the end
			mSuffixes[mSuffixesCount] = ' ';
			std::memcpy(&mSuffixes[msg_begin], msg.Data, msg.Count);
			mSuffixes[msg_begin + copy_count] = '\0';

			mLogger->error(spdlog::fmt_lib::runtime(mSuffixes), args...);
#endif // FE_INTERNAL_BUILD
		}

		template <typename... Args>
		void Fatal(String msg, Args &&...args) {
#ifdef FE_INTERNAL_BUILD
			if (mLoggingLevel > LoggingLevel::Fatal)
				return;

			UInt msg_begin = mSuffixesCount + 1;
			UInt copy_count = min(256 - msg_begin - 1, msg.Count); // 1 for '\0' at the end
			mSuffixes[mSuffixesCount] = ' ';
			std::memcpy(&mSuffixes[msg_begin], msg.Data, msg.Count);
			mSuffixes[msg_begin + copy_count] = '\0';

			mLogger->critical(spdlog::fmt_lib::runtime(mSuffixes), args...);
#endif // FE_INTERNAL_BUILD
		}
		
	};
}