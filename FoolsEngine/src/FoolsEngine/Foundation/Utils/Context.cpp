#include "FE_pch.h"
#include "FoolsEngine/Foundation/Memory/Allocator.h"

namespace fe::Context
{
	namespace Allocators
	{
		Allocator* GeneralPurpose	= new MallocAllocator();
		Allocator* Default			= GeneralPurpose;
		Allocator* Permanent		= new MonotonicAllocator();
		Allocator* Temporary		= new MonotonicAllocator();
		Allocator* SystemOutput		= new MonotonicAllocator();
	}
}