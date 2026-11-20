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

		FE_CORE_ASSERT(false, "Allocators are now pointers sometimes and don't exist");
		Context->Alloc.Permanent->Init();
		Context->Log.FE.Info("Permanent initialized");
		Context->Alloc.MainLoopArena->Clear();
		Context->Log.FE.Info("MainLoopArena initialized");

		Context->Alloc.Default = (PMAlloc*)  Context->Alloc.GeneralPurpose;
		Context->Alloc.Auxiliary = (PMAlloc*)Context->Alloc.GeneralPurpose;
		Context->Alloc.Output = (PMAlloc*)Context->Alloc.GeneralPurpose;
	}

	{
		FE_PROFILER_SCOPE("Platform");
		Context::Log::ScopeCore log_scope("Platform"_FE);
		GlobalContext::Platform::Base.Create();
	}

	{
		FE_PROFILER_SCOPE("Render Context");
		Context::Log::ScopeCore log_scope("Render"_FE);

		{
			FE_PROFILER_SCOPE("Window");
			Context::Log::ScopeCore scope("Window"_FE);
			
			if (FE_PARAMS_HEADLESS)
			{
				glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
				Context::Log::Core.Info("Headless selected - window invisible"_FE);
			}

			Win32Window* window = Context::Alloc::Permanent.Allocate<Win32Window>();
			window->Init({ FE_PARAMS_NAME, 1900, 1000, fe::Render::GAPI::Platform::FE_PARAMS_GAPI });
			window->Create();
			Context::Log::Core.Info("Window created"_FE);

			GlobalContext::Platform::Base.SetEventCallbacks(window);
			GlobalContext::Window::Base = window;
		}

		{
			FE_PROFILER_SCOPE("GAPI");
			Context::Log::ScopeCore scope("GAPI"_FE);

			GlobalContext::GPU::GAPI = Render::GAPI::Platform::FE_PARAMS_GAPI;

			Render::GAPI::Create();

			{
				FE_PROFILER_SCOPE("Screach Buffer creation");
				Context::GPU::ScratchBuffer = Render::GAPI::CreateBuffer();
				Render::GAPI::AllocateCommitBuffer(Context::GPU::ScratchBuffer, FE_PARAMS_GPU_SCRATCH_BUFFER_SIZE);

				Context::Log::Core.Info("GPU ScratchBuffer initialized"_FE);
			}
		}
	}

	FE_PROFILER_SESSION_END();
}

void InitContext::DeinitGlobal()
{
	using namespace fe;

	// Rendering, Window and Platform Layer
	{
		FE_PROFILER_SCOPE("Window");

		GlobalContext::Window::Base->~Window(); // abstraction, safer to destroy
	}

	{
		FE_PROFILER_SCOPE("PlatformBase");

		GlobalContext::Platform::Base.Terminate();
	}

	// Allocators
	{
		FE_PROFILER_SCOPE("Allocators");
	}
}

void InitContext::Init()
{
	using namespace fe;



	
}