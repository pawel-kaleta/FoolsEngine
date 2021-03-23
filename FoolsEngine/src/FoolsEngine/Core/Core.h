#pragma once

#ifdef FE_PLATFORM_WINDOWS
#else
	#error FoolsEngine does not support this platform!
#endif

#define BIT_FLAG(x) (1 << x)