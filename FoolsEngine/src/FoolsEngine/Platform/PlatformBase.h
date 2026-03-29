#pragma once

namespace fe
{
	struct Win32Window;

	struct PlatformBaseWindows
	{
		bool Create();
		void Terminate();

		void SetEventCallbacks(Win32Window* window);
	};

#ifdef FE_PLATFORM_WINDOWS
	using PlatformBase = PlatformBaseWindows;
#else
#error FoolsEngine does not support this platform!
#endif
}