#include "MonotonicAlloc.h"


#include "FoolsEngine/Foundation/Utils/Context.h"

namespace fe
{
	void MonotonicAlloc::DeallocateRaw(Splice<Byte> memReg) { Context::Log::Core.Warn("Deallocation in MonotonicAllocator"_FE); }
	
	Splice<Byte> MonotonicAlloc::ReallocateRaw(Splice<Byte> allocation, Splice<Byte> moveRegion, UInt newSize, UInt alignment)
	{
		FE_CORE_ASSERT(AlignTo(allocation.begin(), alignment) == allocation.begin(), "Cannot realloc to a different alignment");
	
		Splice<Byte> result;
		result.Elements = (Byte*)MBR.allocate(newSize, alignment);
		result.Count = newSize;
	
		if (result.Elements && result.Count)
			std::memcpy(result.begin(), allocation.begin(), min(moveRegion.Count, newSize));
	
		return result;
	}
}