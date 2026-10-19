#include "FoolsEngine/Foundation/Memory/Allocators/Allocator.h"
#include "FoolsEngine/Foundation/Memory/Allocators/ArenaAlloc.h"
#include "FoolsEngine/Foundation/Memory/Allocators/MallocAlloc.h"
#include "FoolsEngine/Foundation/Memory/Allocators/MonotonicAlloc.h"

#include "FoolsEngine/Foundation/Debug/Logger.h"

#include "FoolsEngine/Renderer/1 - GAPI/Context.h"
#include "FoolsEngine/Renderer/1 - GAPI/Resource.h"

#include "Init.h"

namespace fe::Context
{
	namespace Alloc
	{
		//PageAllocator					Page;
		//VirtualAllocator				Virtual;
		TypedAlloc<MonotonicAlloc>		Permanent;
		TypedAlloc<MallocAlloc>			GeneralPurpose;
		TypedAlloc<ArenaAlloc>			MainLoopArena;
		PMAlloc						*	Default		= (PMAlloc*) & GeneralPurpose;
		PMAlloc						*	Auxiliary	= (PMAlloc*) & GeneralPurpose;
		PMAlloc						*	Output		= (PMAlloc*) & GeneralPurpose;
	}

	namespace Log
	{
		Logger Core;
	}

	namespace GPU
	{
		using namespace Render;
		GAPI::GID ScrachBuffer;
		U32 ScrachBufferSize;
	}

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

void InitLog::Init()
{
	fe::Context::InitLog();
}