#pragma once

#include "FoolsEngine/Debug/Profiler.h"
#include "FoolsEngine/Debug/Log.h"
#include <filesystem>

namespace fe {
	class Asserts
	{
	public:
		inline static bool EnableAssertions = false;
		inline static bool EnableCoreAssertions = false;
	};
}

#ifdef FE_INTERNAL_BUILD
	#define FE_ASSERTION_BREAK() if (fe::Asserts::EnableAssertions) __debugbreak();
	#define FE_CORE_ASSERTION_BREAK() if (fe::Asserts::EnableCoreAssertions) __debugbreak();
#elif
	#define FE_ASSERTION_BREAK()
	#define FE_CORE_ASSERTION_BREAK()
#endif

#define FE_ASSERT(x, ...) if (!(x)) { FE_LOG_ERROR("ASSERTION FAILED: in file {0} in function {1} at line {2}.", std::filesystem::path(__FILE__).filename().string(), FE_FUNC_SIG, __LINE__);\
									  FE_LOG_ERROR((std::string)("Assertion Message: ") + (std::string)__VA_ARGS__);\
									  FE_ASSERTION_BREAK(); }
#define FE_CORE_ASSERT(x, ...) if (!(x)) { FE_LOG_CORE_ERROR("CORE ASSERTION FAILED: in file {0} in function {1} at line {2}.", std::filesystem::path(__FILE__).filename().string(), FE_FUNC_SIG, __LINE__);\
										   FE_LOG_CORE_ERROR((std::string)("Assertion Message: ") + (std::string)__VA_ARGS__);\
										   FE_CORE_ASSERTION_BREAK(); }
// USAGE:
// FE_ASSERT( if_statement == true, "assertion message");
// FE_ASSERT( if_statement == true, "assertion message {0}", message_argument);