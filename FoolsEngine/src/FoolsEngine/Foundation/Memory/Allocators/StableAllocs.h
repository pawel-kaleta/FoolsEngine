#pragma once

#include "Allocator.h"
#include "MallocAlloc.h"
#include "MonotonicAlloc.h"

class PageAllocator;
class VirtualAllocator;

namespace fe
{
	struct StableAllocs
	{
		static TypedAlloc<MallocAlloc>* GeneralPurpose;
		static TypedAlloc<MonotonicAlloc>* Permanent;

		static PageAllocator* Page;
		static VirtualAllocator* Virtual;

		void GetExlusiveArena() {};
	};
}