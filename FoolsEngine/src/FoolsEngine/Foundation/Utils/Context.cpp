#include "FE_pch.h"
#include "FoolsEngine/Foundation/Memory/Allocator.h"

namespace fe::Context
{
	namespace Allocators
	{
		namespace Global
		{
			Allocator* GeneralPurpose	= new MallocAllocator();
			Allocator* Permanent		= new MonotonicAllocator();
			Allocator* Temporary		= new MonotonicAllocator();
		}

		namespace System
		{
			Allocator* GeneralPurpose	= new MallocAllocator();
			Allocator* Permanent		= new MonotonicAllocator();
			Allocator* OutputAllocator	= new MonotonicAllocator();
			Allocator* Temporary		= new MonotonicAllocator();
		}
	}
}