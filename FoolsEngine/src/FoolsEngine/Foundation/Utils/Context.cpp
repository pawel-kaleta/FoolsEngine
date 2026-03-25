#include "FE_pch.h"
#include "FoolsEngine/Foundation/Memory/Allocator.h"

namespace fe::Context
{
	namespace Allocators
	{
		TypedAlloc<MallocAllocator>		GeneralPurpose(new MallocAllocator());
		TypedAlloc<MonotonicAllocator>	Permanent(new MonotonicAllocator());

		TypedAlloc<Allocator> Default		= *(TypedAlloc<Allocator>*) & GeneralPurpose;
		TypedAlloc<Allocator> Auxiliary		= *(TypedAlloc<Allocator>*) & GeneralPurpose;
		TypedAlloc<Allocator> Temporary		= *(TypedAlloc<Allocator>*) & Permanent;
		TypedAlloc<Allocator> Output	= *(TypedAlloc<Allocator>*) & Permanent;
	}
}