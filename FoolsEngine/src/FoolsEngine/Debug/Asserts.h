#pragma once

#include "FE_pch.h"
#include "FoolsEngine/Debug/Profiler.h"
#include "FoolsEngine/Debug/Log.h"
#include <filesystem>

#ifdef FE_INTERNAL_BUILD
	// to do: change FE_ENABLE_ASSERTIONS into a global variable to be able to turn on and off assertions at runtime
	#ifdef FE_ENABLE_ASSERTIONS
		#define FE_DEBUG_BREAK() __debugbreak()
	#else
		#define FE_DEBUG_BREAK()
	#endif // FE_ENABLE_ASSERTIONS

	#define FE_ASSERT(x, ...) { if (!(x)) { FE_LOG_ERROR("ASSERTION FAILED: in file {0} in function {1} at line {2}.", std::filesystem::path(__FILE__).filename().string(), FE_FUNC_SIG, __LINE__);\
	                                        FE_LOG_ERROR((std::string)("Assertion Message: ") + (std::string)__VA_ARGS__);\
	                                        FE_DEBUG_BREAK(); } }

	#define FE_CORE_ASSERT(x, ...) { if (!(x)) { FE_LOG_CORE_ERROR("ASSERTION FAILED: in file {0} in function {1} at line {2}.", std::filesystem::path(__FILE__).filename().string(), FE_FUNC_SIG, __LINE__);\
	                                             FE_LOG_CORE_ERROR((std::string)("Assertion Message: ") + (std::string)__VA_ARGS__);\
	                                             FE_DEBUG_BREAK(); } }
#else
	#define FE_ASSERT(x, ...)
	#define FE_CORE_ASSERT(x, ...)
#endif // FE_INTERNAL_BUILD