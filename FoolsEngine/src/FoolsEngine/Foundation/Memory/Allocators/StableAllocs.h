#pragma once

#include "Allocator.h"
#include "MallocAllocator.h"
#include "MonotonicAllocator.h"

class PageAllocator;
class VirtualAllocator;

namespace fe::StableAllocs
{
	extern MallocAlloc* GeneralPurpose;
	extern MonotonicAlloc* Permanent;
	extern PageAllocator* Page;
	extern VirtualAllocator* Virtual;
	
	void GetExlusiveArena() {};
}