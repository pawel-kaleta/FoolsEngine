#include "FE_pch.h"
#include "FoolsEngine/Foundation/Memory/Allocators/Allocator.h"
#include "FoolsEngine/Foundation/Memory/Allocators/ArenaAlloc.h"
#include "FoolsEngine/Foundation/Memory/Allocators/MallocAlloc.h"
#include "FoolsEngine/Foundation/Memory/Allocators/MonotonicAlloc.h"

namespace fe
{
}

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

	void Init(UInt mainLoopArenaSize)
	{
		Allocators::Permanent.Init();
		Allocators::MainLoopArena.Buffer = Allocators::Permanent.AllocateRaw(mainLoopArenaSize);
		Allocators::MainLoopArena.Clear();
	}
}