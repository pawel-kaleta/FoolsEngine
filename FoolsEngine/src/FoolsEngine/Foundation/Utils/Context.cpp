#include "FE_pch.h"
#include "FoolsEngine/Foundation/Memory/Allocators/Allocator.h"
#include "FoolsEngine/Foundation/Memory/Allocators/ArenaAlloc.h"
#include "FoolsEngine/Foundation/Memory/Allocators/MallocAlloc.h"
#include "FoolsEngine/Foundation/Memory/Allocators/MonotonicAlloc.h"

#include "FoolsEngine/Renderer/1 - GAPI/Context.h"
#include "FoolsEngine/Renderer/1 - GAPI/Resource.h"


namespace fe::Context
{
	namespace Allocators
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

	namespace GPU
	{
		using namespace Render;
		GAPI::GID ScrachBuffer;
		U32 ScrachBufferSize;
	}

	void InitAllocators(UInt mainLoopArenaSize)
	{
		Allocators::Permanent.Init();
		Allocators::MainLoopArena.Buffer = Allocators::Permanent.AllocateRaw(mainLoopArenaSize);
		Allocators::MainLoopArena.Clear();
	}

	void InitGPU(U32 scrachBufferSize)
	{
		GPU::ScrachBufferSize = scrachBufferSize;
		GPU::ScrachBuffer = Render::GAPI::CreateBuffer();
		Render::GAPI::AllocateCommitBuffer(GPU::ScrachBuffer, scrachBufferSize);
	}
}