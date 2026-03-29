#pragma once

#include "FoolsEngine/Foundation/Memory/Allocators/Allocator.h"
#include "FoolsEngine/Foundation/Memory/Allocators/MallocAlloc.h"
#include "FoolsEngine/Foundation/Memory/Allocators/MonotonicAlloc.h"
#include "FoolsEngine/Foundation/Utils/Context.h"


namespace fe
{
	struct ResourceManager
	{
		TypedAlloc<MallocAlloc> DefaultAlloc;
		TypedAlloc<MallocAlloc> AuxilaryAlloc;
		TypedAlloc<MonotonicAlloc> PermanentAlloc;
	};
}