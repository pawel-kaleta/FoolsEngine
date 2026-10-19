#include "Init.h"

#include "Context.h"

#include "FoolsEngine/Foundation/Memory/String.h"

#include "FoolsEngine/Renderer/1 - GAPI/Context.h"
#include "FoolsEngine/Renderer/1 - GAPI/Resource.h"

namespace fe::Context
{
	void InitAlloc(UInt mainLoopArenaSize)
	{
		Alloc::Permanent.Init();
		Alloc::MainLoopArena.Buffer = Alloc::Permanent.AllocateRaw(mainLoopArenaSize);
		Alloc::MainLoopArena.Clear();
	}

	void InitGPU(U32 scrachBufferSize)
	{
		GPU::ScrachBufferSize = scrachBufferSize;
		GPU::ScrachBuffer = Render::GAPI::CreateBuffer();
		Render::GAPI::AllocateCommitBuffer(GPU::ScrachBuffer, scrachBufferSize);
	}

	void InitLog()
	{
		Log::Core.Init();
	}
}

void InitLog::Init(fe::UInt mainLoopArenaSize)
{
	using namespace fe;

	Context::InitLog();
	Context::Log::ScopeCore init("Initialization"_FE, LoggingLevel::Trace);
	Context::Log::ScopeCore context("Context"_FE);
	Context::Log::Core.Info("Logging initialized");
	Context::InitAlloc(mainLoopArenaSize);
	Context::Log::Core.Info("Allocators initialized");
}