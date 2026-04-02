#include "FE_pch.h"
#include "FoolsEngine/Foundation/Memory/Allocators/Allocator.h"
#include "FoolsEngine/Foundation/Memory/Allocators/MallocAlloc.h"
#include "FoolsEngine/Foundation/Memory/Allocators/MonotonicAlloc.h"


namespace fe::StableAllocs
{
	MallocAlloc*	GeneralPurpose	= new MallocAlloc();
	MonotonicAlloc*	Permanent		= new MonotonicAlloc();
}

namespace fe::Context
{
	namespace Allocators
	{
		TypedAlloc<Allocator>* Default		= (TypedAlloc<Allocator>*) fe::StableAllocs::GeneralPurpose;
		TypedAlloc<Allocator>* Auxiliary	= (TypedAlloc<Allocator>*) fe::StableAllocs::GeneralPurpose;
		TypedAlloc<Allocator>* Temporary	= (TypedAlloc<Allocator>*) fe::StableAllocs::GeneralPurpose;
		TypedAlloc<Allocator>* Output		= (TypedAlloc<Allocator>*) fe::StableAllocs::GeneralPurpose;
	}
}