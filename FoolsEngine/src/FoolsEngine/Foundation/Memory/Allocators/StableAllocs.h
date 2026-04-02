#pragma once

#include "Allocator.h"
#include "MallocAlloc.h"
#include "MonotonicAlloc.h"

class PageAllocator;
class VirtualAllocator;

namespace fe::StableAllocs
{
	extern TypedAlloc<MallocAlloc>* GeneralPurpose;
	extern TypedAlloc<MonotonicAlloc>* Permanent;

	extern PageAllocator* Page;
	extern VirtualAllocator* Virtual;
	
	void GetExlusiveArena() {};
}