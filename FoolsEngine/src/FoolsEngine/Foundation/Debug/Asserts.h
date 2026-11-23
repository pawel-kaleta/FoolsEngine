#pragma once

#include <format>
#include <source_location>


namespace fe
{
	extern bool EnableAssertionBreak;
	extern bool EnableCoreAssertionBreak;

	void HandleAssert(const std::source_location& location, const char* message);
}

#ifdef FE_INTERNAL_BUILD
	#define FE_ASSERTION_BREAK()      if (fe::EnableAssertionBreak) __debugbreak();
	#define FE_CORE_ASSERTION_BREAK() if (fe::EnableCoreAssertionBreak) __debugbreak();

	#define FE_ASSERT(x, ...) if (!(x)) {\
		FE_ASSERTION_BREAK(); }
		//fe::Context->Log.AP.Error("Application asserts not implemented!");\
		//fe::Context->Log.AP.Error("CORE ASSERTION FAILED:");\
		//fe::Context->Log.AP.mLogger->error(spdlog::fmt_lib::runtime("File:     {}"), std::filesystem::path(__FILE__).filename().string());\
		//fe::Context->Log.AP.mLogger->error(spdlog::fmt_lib::runtime("Function: {}"), FE_FUNC_SIG);\
		//fe::Context->Log.AP.mLogger->error(spdlog::fmt_lib::runtime("Line:     {}"), __LINE__);\
		//fe::Context->Log.AP.mLogger->error(spdlog::fmt_lib::runtime("Message:  {}"), __VA_ARGS__);\

	#define FE_CORE_ASSERT(x, ...) if (!(x)) {\
		fe::HandleAssert(std::source_location::current(), std::format(__VA_ARGS__).c_str());\
		FE_CORE_ASSERTION_BREAK(); }
		//fe::Context->Log.FE.Error("CORE ASSERTION FAILED:");\
		//fe::Context->Log.FE.mLogger->error(spdlog::fmt_lib::runtime("File:     {}"), std::filesystem::path(__FILE__).filename().string());\
		//fe::Context->Log.FE.mLogger->error(spdlog::fmt_lib::runtime("Function: {}"), FE_FUNC_SIG);\
		//fe::Context->Log.FE.mLogger->error(spdlog::fmt_lib::runtime("Line:     {}"), __LINE__);\
		//fe::Context->Log.FE.mLogger->error(spdlog::fmt_lib::runtime("Message:  {}"), __VA_ARGS__);\

#else
	#define FE_ASSERTION_BREAK()
	#define FE_CORE_ASSERTION_BREAK()
	#define FE_ASSERT(x, ...)
	#define FE_CORE_ASSERT(x, ...)
#endif
// USAGE:
// FE_ASSERT( if_statement == true, "assertion message");
// FE_ASSERT( if_statement == true, "assertion message {0}", message_argument);