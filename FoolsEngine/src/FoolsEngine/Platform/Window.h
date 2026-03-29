#pragma once

#include "Win32/Win32Window.h"

namespace fe
{
#ifdef FE_PLATFORM_WINDOWS
	using Window = Win32Window;
#else
#error FoolsEngine does not support this platform!
#endif
}