#pragma once

#include "FoolsEngine/Foundation/Debug/Logger.h"
#include "FoolsEngine/Platform/Window.h"

#include "FoolsEngine/Platform/PlatformBase.h"

namespace fe
{
	struct GlobalContextStruct
	{
		struct
		{
			//PageAllocator	Page;
			//VirtualAllocator	Virtual;
		} Alloc;

		struct
		{
			Logstream FE;
			Logstream AP;

			struct
			{
				struct
				{
					FileSink FE;
					FileSink AP;
					FileSink Common;
				} File;

				struct
				{
					ConsoleSink Common;
				} Console;

				struct
				{
					RingbufferSink FE;
					RingbufferSink AP;
					RingbufferSink Common;
				} Editor;
			} Sinks;
		} Log;

		struct
		{
			PlatformBase Base;
		} Platform;

		struct
		{
			fe::Window* Base;
		} Window;

		struct
		{
			Render::GAPI::Platform GAPI;
		} GPU;
	};
}