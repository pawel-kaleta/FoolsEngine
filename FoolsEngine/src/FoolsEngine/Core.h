#pragma once

#ifdef FE_PLATFORM_WINDOWS
	#ifdef FE_BUILD_DLL
		#define FE_API
//__declspec(dllexport)
	#else
		#define FE_API
//__declspec(dllimport)
	#endif
#else
	#error FoolsEngine does not support this platform!
#endif

#define BIT_FLAG(x) (1 << x)