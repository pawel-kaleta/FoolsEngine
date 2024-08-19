#include "FE_pch.h"
#include "Window.h"

#ifdef FE_PLATFORM_WINDOWS
	#include "FoolsEngine/Platform/Win32/Win32Window.h"
#endif // FE_PLATFORM_WINDOWS

namespace fe
{
	Scope<Window> Window::Create(const WindowAttributes& attr)
	{
		FE_PROFILER_FUNC();

	#ifdef FE_PLATFORM_WINDOWS
		return CreateScope<Win32Window>(attr);
	#else
		FE_CORE_ASSERT(false, "Unknown platform!");
		#error
	#endif
	}
}