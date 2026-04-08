#include "FE_pch.h"
#include "FoolsEngine/Foundation/Memory/Allocators/Allocator.h"
#include "FoolsEngine/Foundation/Memory/Allocators/MallocAlloc.h"
#include "FoolsEngine/Foundation/Memory/Allocators/MonotonicAlloc.h"
#include "FoolsEngine/Foundation/Memory/Allocators/StableAllocs.h"

namespace fe
{
	TypedAlloc<MallocAlloc>*	StableAllocs::GeneralPurpose	= (TypedAlloc<MallocAlloc>*) new MallocAlloc();
	TypedAlloc<MonotonicAlloc>* StableAllocs::Permanent			= (TypedAlloc<MonotonicAlloc>*) new MonotonicAlloc();
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