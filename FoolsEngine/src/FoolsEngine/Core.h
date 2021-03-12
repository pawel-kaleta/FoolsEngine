#pragma once

#ifdef FE_PLATFORM_WINDOWS
	#ifdef FE_BUILD_DLL
		#define FE_API __declspec(dllexport)
	#else
		#define FE_API __declspec(dllimoirt)
	#endif
#else
	#error FoolsEngine does not support dzis platform!
#endif
