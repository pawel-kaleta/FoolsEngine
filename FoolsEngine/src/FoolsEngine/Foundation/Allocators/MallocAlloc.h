#pragma once

#include "Allocator.h"
#include "FoolsEngine/Foundation/Debug/Asserts.h"
#include "FoolsEngine/Foundation/Utils/BitOperations.h"

#include <cstdlib>

namespace fe
{
	class MallocAlloc : public Allocator
	{
	public:
		virtual Splice<Byte> AllocateRaw(UInt bytes) override final
		{
			Splice<Byte> result;
			result.Elements = (Byte*) std::malloc(bytes);
			result.Count = bytes;
			return result;
		}
		
		virtual Splice<Byte> AllocateRaw(UInt bytes, UInt alignment) override final
		{
			FE_CORE_ASSERT(alignment <= 8, "MallocAllocator does not align to anything more then 8");
			return AllocateRaw(bytes); //we dont allign, because delete() takes alignment and we dont require it in our interface
		}
		virtual void DeallocateRaw(Splice<Byte> memReg) override final { std::free(memReg.Elements); }

		virtual Splice<Byte> ReallocateRaw(Splice<Byte> allocation, Splice<Byte> moveRegion, UInt newSize, UInt alignment) override final
		{
			FE_CORE_ASSERT(AlignTo(allocation.begin(), alignment) == allocation.begin(), "Cannot realloc to a different alignment");

			Splice<Byte> result;
			result.Elements = (Byte*)std::realloc(allocation.begin(), newSize);
			result.Count = newSize;
			return result;
		};

		template <UInt Alignment>
		Splice<Byte> ReallocateRaw(Splice<Byte> allocation, Splice<Byte> moveRegion, UInt newSize)
		{
			return ReallocateRaw(allocation, allocation, newSize, Alignment);
		};

		template <UInt Alignment>
		Splice<Byte> ReallocateRaw(Splice<Byte> allocation, UInt newSize)
		{
			return ReallocateRaw(allocation, allocation, newSize, Alignment);
		};

		template <UInt Size, UInt Alignment>
		Array<Byte, Size>* AllocateRaw() { return AllocateRaw(Size, Alignment); }

		template <UInt Alignment>
		Splice<Byte> AllocateRaw(UInt bytes) { return AllocateRaw(bytes, Alignment); }

		template <UInt Size>
		void DeallocateRaw(Byte* ptr) { std::free(ptr); }
	};
}