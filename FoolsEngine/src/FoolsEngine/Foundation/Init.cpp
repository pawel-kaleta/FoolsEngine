#include "Init.h"

#include "Context/Context.h"

#include "FoolsEngine/Foundation/Containers/String.h"

#include "FoolsEngine/Platform/Win32/Win32Window.h"

#include "FoolsEngine/Renderer/1 - GAPI/Context.h"
#include "FoolsEngine/Renderer/1 - GAPI/Resource.h"

#include <GLFW/glfw3.h>


void InitContext::InitGlobal()
{
	using namespace fe;

	GlobalContext = new GlobalContextStruct();

	GlobalContext->Alloc.Permanent.Init(GlobalContext->Alloc.GeneralPurpose.P());
	
	Context = GlobalContext->Alloc.Permanent.Allocate<ContextStruct>();


	GlobalContext->Log.Sinks.File.FE.Init(CString("Logs/Core.log", 14));
	GlobalContext->Log.Sinks.File.AP.Init(CString("Logs/App.log", 13));
	GlobalContext->Log.Sinks.File.Common.InitCommon(CString("Logs/Common.log", 16));

	GlobalContext->Log.Sinks.Console.Common.InitCommon();

	GlobalContext->Log.Sinks.Editor.FE.Init(128);
	GlobalContext->Log.Sinks.Editor.AP.Init(128);
	GlobalContext->Log.Sinks.Editor.Common.InitCommon(256);

	GlobalContext->Log.FE.Init();
	GlobalContext->Log.AP.Init();

	GlobalContext->Log.FE.AddSink(GlobalContext->Log.Sinks.File.FE);
	GlobalContext->Log.FE.AddSink(GlobalContext->Log.Sinks.File.Common);
	GlobalContext->Log.FE.AddSink(GlobalContext->Log.Sinks.Console.Common);
	GlobalContext->Log.FE.AddSink(GlobalContext->Log.Sinks.Editor.FE);
	GlobalContext->Log.FE.AddSink(GlobalContext->Log.Sinks.Editor.Common);

	GlobalContext->Log.AP.AddSink(GlobalContext->Log.Sinks.File.AP);
	GlobalContext->Log.AP.AddSink(GlobalContext->Log.Sinks.File.Common);
	GlobalContext->Log.AP.AddSink(GlobalContext->Log.Sinks.Console.Common);
	GlobalContext->Log.AP.AddSink(GlobalContext->Log.Sinks.Editor.AP);
	GlobalContext->Log.AP.AddSink(GlobalContext->Log.Sinks.Editor.Common);

	Context->Log.FE.Init(CString("FE", 3), GlobalContext->Log.FE);
	Context->Log.AP.Init(CString("AP", 3), GlobalContext->Log.AP);

	ContextScope::Log::FE init_scope("Global Init"_FE, LoggingLevel::Trace);
	FE_PROFILER_SESSION_START("INIT", "Logs/ProfileData_Init.json");

	{
		ContextScope::Log::FE log_scope("Logging"_FE);
		Context->Log.FE.Info("Logging initialized");
	}

	{
		FE_PROFILER_SCOPE("Allocators");

		ContextScope::Log::FE log_scope("Alloc"_FE);

		Context->Alloc.Permanent.Init(GlobalContext->Alloc.Permanent.P());
		Context->Log.FE.Info("Permanent initialized");

		//GlobalContext->Alloc.MainLoopArena.Clear();
		//Context->Alloc.MainLoopArena.Clear();
		//Context->Log.FE.Info("MainLoopArena initialized");

		Context->Alloc.Default		= (PMAlloc*)Context->Alloc.GeneralPurpose.P();
		Context->Alloc.Auxiliary	= (PMAlloc*)Context->Alloc.GeneralPurpose.P();
		Context->Alloc.Output		= (PMAlloc*)Context->Alloc.GeneralPurpose.P();
	}

	{
		FE_PROFILER_SCOPE("Platform");
		ContextScope::Log::FE log_scope("Platform"_FE);
		GlobalContext->Platform.Base.Create();
	}

	{
		FE_PROFILER_SCOPE("Render Context");
		ContextScope::Log::FE log_scope("Render"_FE);

		{
			FE_PROFILER_SCOPE("Window");
			ContextScope::Log::FE scope("Window"_FE);
			
			if (FE_PARAMS_HEADLESS)
			{
				glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
				Context->Log.FE.Info("Headless selected - window invisible"_FE);
			}

			Win32Window* window = Context->Alloc.Permanent.Allocate<Win32Window>();
			window->Init({ FE_PARAMS_NAME, 1900, 1000, fe::Render::GAPI::Platform::FE_PARAMS_GAPI });
			window->Create();
			Context->Log.FE.Info("Window created"_FE);

			GlobalContext->Platform.Base.SetEventCallbacks(window);
			GlobalContext->Window.Base = window;
		}

		{
			FE_PROFILER_SCOPE("GAPI");
			ContextScope::Log::FE scope("GAPI"_FE);

			GlobalContext->GPU.GAPI = Render::GAPI::Platform::FE_PARAMS_GAPI;

			Render::GAPI::Create();

			{
				FE_PROFILER_SCOPE("Screach Buffer creation");
				Context->GPU.ScratchBuffer = Render::GAPI::CreateBuffer();
				Render::GAPI::AllocateCommitBuffer(Context->GPU.ScratchBuffer, FE_PARAMS_GPU_SCRATCH_BUFFER_SIZE);

				Context->Log.FE.Info("GPU ScratchBuffer initialized"_FE);
			}
		}
	}

	FE_PROFILER_SESSION_END();
}

void InitContext::DeinitGlobal()
{
	using namespace fe;

	FE_CORE_ASSERT(false, "Is this ever called?"); //deinit monotonic allocs!

	// Rendering, Window and Platform Layer
	{
		FE_PROFILER_SCOPE("Window");

		GlobalContext->Window.Base->~Window(); // abstraction, safer to destroy
	}

	{
		FE_PROFILER_SCOPE("PlatformBase");

		GlobalContext->Platform.Base.Terminate();
	}

	// Allocators
	{
		FE_PROFILER_SCOPE("Allocators");
	}
}

void InitContext::Init()
{
	using namespace fe;

	FE_CORE_ASSERT(false, "Not implemented");
}