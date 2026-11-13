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
		DistSink Core;
		DistSink App;

		namespace LeafSinks
		{
			FileSink FileCore;
			FileSink FileApp;
			FileSink FileCommon;

			ConsoleSink ConsoleCommon;

			RingbufferSink EditorCore;
			RingbufferSink EditorApp;
			RingbufferSink EditorCommon;
		}
	}

	namespace Platform
	{
		PlatformBase Base;
	}

	namespace Window
	{
		fe::Window* Base;
	}

	namespace GPU
	{
		Render::GAPI::Platform GAPI;
	}
}

