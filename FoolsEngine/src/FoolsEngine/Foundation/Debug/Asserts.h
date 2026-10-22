#pragma once

#include "Profiler.h"

#include "FoolsEngine/Foundation/Debug/Logger.h"

#include <filesystem>

namespace fe
{
	extern bool EnableAssertionBreak;
	extern bool EnableCoreAssertionBreak;

	namespace Context::Log
	{
		extern Logger Core;
	}
}

#ifdef FE_INTERNAL_BUILD
	#define FE_ASSERTION_BREAK()      if (fe::EnableAssertionBreak) __debugbreak();
	#define FE_CORE_ASSERTION_BREAK() if (fe::EnableCoreAssertionBreak) __debugbreak();

	#define FE_ASSERT(x, ...) if (!(x)) {\
		fe::Context::Log::Core.Error("Application asserts not implemented!");\
		fe::Context::Log::Core.Error("CORE ASSERTION FAILED:");\
		fe::Context::Log::Core.mLogger->error(spdlog::fmt_lib::runtime("File:     {}"), std::filesystem::path(__FILE__).filename().string());\
		fe::Context::Log::Core.mLogger->error(spdlog::fmt_lib::runtime("Function: {}"), FE_FUNC_SIG);\
		fe::Context::Log::Core.mLogger->error(spdlog::fmt_lib::runtime("Line:     {}"), __LINE__);\
		fe::Context::Log::Core.mLogger->error(spdlog::fmt_lib::runtime("Message:  {}"), __VA_ARGS__);\
		FE_ASSERTION_BREAK(); }
	#define FE_CORE_ASSERT(x, ...) if (!(x)) {\
		fe::Context::Log::Core.Error("CORE ASSERTION FAILED:");\
		fe::Context::Log::Core.mLogger->error(spdlog::fmt_lib::runtime("File:     {}"), std::filesystem::path(__FILE__).filename().string());\
		fe::Context::Log::Core.mLogger->error(spdlog::fmt_lib::runtime("Function: {}"), FE_FUNC_SIG);\
		fe::Context::Log::Core.mLogger->error(spdlog::fmt_lib::runtime("Line:     {}"), __LINE__);\
		fe::Context::Log::Core.mLogger->error(spdlog::fmt_lib::runtime("Message:  {}"), __VA_ARGS__);\
		FE_CORE_ASSERTION_BREAK(); }
#else
	#define FE_ASSERTION_BREAK()
	#define FE_CORE_ASSERTION_BREAK()
	#define FE_ASSERT(x, ...)
	#define FE_CORE_ASSERT(x, ...)
#endif
// USAGE:
// FE_ASSERT( if_statement == true, "assertion message");
// FE_ASSERT( if_statement == true, "assertion message {0}", message_argument);