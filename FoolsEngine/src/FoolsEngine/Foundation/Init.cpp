#include "Init.h"

#include "Context/Context.h"
#include "Context/GlobalContext.h"

#include "FoolsEngine/Foundation/Containers/String.h"

#include "FoolsEngine/Renderer/1 - GAPI/Context.h"
#include "FoolsEngine/Renderer/1 - GAPI/Resource.h"

void InitContext::InitGlobal()
{
	using namespace fe;

	Context::Log::Core.Init();

	FE_PROFILER_SESSION_START("INIT", "Logs/ProfileData_Init.json");

	Context::Log::ScopeCore init_scope("Context Initialization"_FE, LoggingLevel::Trace);
	
	{
		
		Context::Log::ScopeCore log_scope("Logging"_FE);
		Context::Log::Core.Info("Logging initialized");
	}

	{
		FE_PROFILER_SCOPE("Allocators");

		Context::Log::ScopeCore log_scope("Alloc"_FE);

		Context::Alloc::Permanent.Init();
		Context::Log::Core.Info("Permanent initialized");
		Context::Alloc::MainLoopArena.Buffer = Context::Alloc::Permanent.AllocateRaw(24 * 1024);
		Context::Alloc::MainLoopArena.Clear();
		Context::Log::Core.Info("MainLoopArena initialized");
	}

	{
		FE_PROFILER_SCOPE("Platform");

		GlobalContext::Platform::Base.Create();
	}

	FE_PROFILER_SESSION_END();
}

void InitContext::Init()
{
	using namespace fe;

	Context::Log::ScopeCore init_scope("Context Initialization"_FE, LoggingLevel::Trace);

	{
		Context::Log::ScopeCore log_scope("RenderContext"_FE);

		//Render::GAPI::Create(); // should be non-main context

		Context::GPU::ScratchBufferSize = 1024 * 64;
		Context::GPU::ScratchBuffer = Render::GAPI::CreateBuffer();
		Render::GAPI::AllocateCommitBuffer(Context::GPU::ScratchBuffer, 1024 * 64);
		Context::Log::Core.Info("MainLoopArena initialized");
	}
}