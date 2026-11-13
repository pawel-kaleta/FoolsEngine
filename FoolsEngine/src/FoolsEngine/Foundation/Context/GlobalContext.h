#pragma once

#include "FoolsEngine/Foundation/Debug/Logger.h"
#include "FoolsEngine/Platform/Window.h"

#include "FoolsEngine/Platform/PlatformBase.h"

namespace fe::GlobalContext
{
	namespace Alloc
	{
		//extern PageAllocator	Page;
		//extern VirtualAllocator	Virtual;
	}

	namespace Log
	{
		extern DistSink Core;
		extern DistSink App;

		namespace LeafSinks
		{
			extern FileSink FileCore;
			extern FileSink FileApp;
			extern FileSink FileCommon;

			extern ConsoleSink ConsoleCommon;

			extern RingbufferSink EditorCore;
			extern RingbufferSink EditorApp;
			extern RingbufferSink EditorCommon;
		}
	}

	namespace Platform
	{
		extern PlatformBase Base;
	}

	namespace Window
	{
		extern fe::Window* Base;
	}

	namespace GPU
	{
		extern Render::GAPI::Platform GAPI;
	}

}