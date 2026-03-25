#include "FE_pch.h"
#include "FoolsEngine/Foundation/Memory/Allocators/Allocator.h"
#include "FoolsEngine/Foundation/Memory/Allocators/MallocAllocator.h"
#include "FoolsEngine/Foundation/Memory/Allocators/MonotonicAllocator.h"


namespace fe::Global::StableAllocs
{
	MallocAlloc*	GeneralPurpose	= new MallocAlloc();
	MonotonicAlloc*	Permanent		= new MonotonicAlloc();
}

namespace fe::Context
{
	namespace Allocators
	{
		Allocator* Default		= (Allocator*) fe::Global::StableAllocs::GeneralPurpose;
		Allocator* Auxiliary	= (Allocator*) fe::Global::StableAllocs::GeneralPurpose;
		Allocator* Temporary	= (Allocator*) fe::Global::StableAllocs::Permanent;
		Allocator* Output		= (Allocator*) fe::Global::StableAllocs::Permanent;
	}
}