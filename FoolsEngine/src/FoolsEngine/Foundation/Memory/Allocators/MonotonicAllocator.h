#pragma once

#include "Allocator.h"

namespace fe
{
	class MonotonicAlloc : public Allocator
	{
	public:
		std::pmr::monotonic_buffer_resource MBR;

		virtual Splice<Byte> Allocate(UInt bytes) final override
		{
			Splice<Byte> mem_reg;
			mem_reg.Elements = (Byte*)MBR.allocate(bytes);
			mem_reg.Count = bytes;
			return mem_reg;
		}
		virtual Splice<Byte> Allocate(UInt bytes, UInt alignment) final override
		{
			Splice<Byte> mem_reg;
			mem_reg.Elements = (Byte*)MBR.allocate(bytes, alignment);
			mem_reg.Count = bytes;
			return mem_reg;
		}
		virtual void Deallocate(Splice<Byte> memReg) final override { FE_LOG_CORE_WARN("Deallocation in MonotonicAllocator"); }

		template <UInt Size, UInt Alignment>
		Array<Byte, Size>* Allocate() { return (Array<Byte, Size>*)MBR.allocate(Size, Alignment); }

		template <UInt Alignment>
		Splice<Byte> Allocate(UInt size) { return Allocate(size, Alignment); }

		template <UInt Size>
		void Deallocate(Byte* ptr) { FE_LOG_CORE_WARN("Deallocation in MonotonicAllocator"); }

		void Clear() { MBR.release(); }
	};
}