#include "FoolsEngine/Foundation/Debug/Logger.h"
#include "FoolsEngine/Foundation/Allocators/Allocator.h"
#include "FoolsEngine/Platform/PlatformBase.h"
#include "FoolsEngine/Platform/Window.h"

namespace fe::GlobalContext
{
	namespace Alloc
	{
		//PageAllocator					Page;
		//VirtualAllocator				Virtual;
	}

	namespace Log
	{
		Sink Core;
		Sink App;
	}

	namespace Window
	{
		fe::Window* Base;
	}

	namespace GPU
	{
		
	}

	namespace Platform
	{
		PlatformBase Base;
	}
}

