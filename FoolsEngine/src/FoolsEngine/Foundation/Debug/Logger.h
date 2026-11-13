#pragma once

#include "LoggingLevel.h"

#include "Profiler.h"

#include "FoolsEngine/Foundation/Containers/String.h"
#include "FoolsEngine/Foundation/Utils/Core.h"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include "spdlog/sinks/dist_sink.h"
#include "spdlog/sinks/ringbuffer_sink.h"

#include <memory>


namespace fe
{
	class Sink
	{
	public:
		std::shared_ptr<spdlog::sinks::sink> mSink;
	};

	class RingbufferSink : public Sink
	{
	public:
		void Init(UInt msgCount)
		{
			mSink = std::make_shared<spdlog::sinks::ringbuffer_sink_mt>(msgCount);
			mSink->set_pattern("%^%M:%S:%f | %v%$");
		}

		void InitCommon(UInt msgCount)
		{
			mSink = std::make_shared<spdlog::sinks::ringbuffer_sink_mt>(msgCount);
			mSink->set_pattern("%^%M:%S:%f | %-2n | %v%$");
		}
	};

	class ConsoleSink : public Sink
	{
	public:
		void Init()
		{
			mSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
			mSink->set_pattern("%^%M:%S:%f | %v%$");
		}

		void InitCommon()
		{
			mSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
			mSink->set_pattern("%^%M:%S:%f | %-2n | %v%$");
		}
	};

	class FileSink : public Sink
	{
	public:
		void Init(CString filepath)
		{
			mSink = std::make_shared<spdlog::sinks::basic_file_sink_st>(filepath.Data, true);
			mSink->set_pattern("%T:%f | %-8l | %v");
		}

		void InitCommon(CString filepath)
		{
			mSink = std::make_shared<spdlog::sinks::basic_file_sink_st>(filepath.Data, true);
			mSink->set_pattern("%T:%f | %-2n | %-8l | %v");
		}
	};

	class DistSink : public Sink
	{
	public:
		void Init()
		{
			mSink = std::make_shared<spdlog::sinks::dist_sink_mt>();
		}

		void AddSink(Sink& sink)
		{	
			std::dynamic_pointer_cast<spdlog::sinks::dist_sink_mt>(mSink)->add_sink(sink.mSink);
		}
	};
	
	class Logger
	{
	public:
		std::shared_ptr<spdlog::logger> mLogger;

		LoggingLevel mLoggingLevel;
		char mSuffixes[256];
		UInt mSuffixesCount;

		void Init(CString name)
		{
			mSuffixes[255] = 'X';

			mLoggingLevel = LoggingLevel::Trace;
			mSuffixesCount = 0;

			mLogger = std::make_shared<spdlog::logger>(name.Data);
			mLogger->set_level(spdlog::level::trace);
		}

		void AddSink(Sink& sink)
		{
			mLogger->sinks().push_back(sink.mSink);
		}

		template <typename... Args>
		void Trace(String msg, Args &&...args) {
#ifdef FE_INTERNAL_BUILD
			if (mLoggingLevel > LoggingLevel::Trace)
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