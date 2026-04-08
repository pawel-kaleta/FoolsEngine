#pragma once

#include "Allocator.h"

namespace fe
{
	class MonotonicAlloc : public Allocator
	{
	public:
		Allocator* UpStreamAlloc = nullptr;
		std::pmr::monotonic_buffer_resource MBR;

		void Init(Allocator* upStreamAlloc)
		{
			new(&MBR) std::pmr::monotonic_buffer_resource((STD_PMR_Allocator<Allocator>*)upStreamAlloc);
			UpStreamAlloc = upStreamAlloc;
		}
		void Init()
		{
			new(&MBR) std::pmr::monotonic_buffer_resource();
			UpStreamAlloc = nullptr;
		}
		void Deinit()
		{
			MBR.~monotonic_buffer_resource();
			UpStreamAlloc = nullptr;
		}

		virtual Splice<Byte> AllocateRaw(UInt bytes) final override
		{
			Splice<Byte> mem_reg;
			mem_reg.Elements = (Byte*)MBR.allocate(bytes);
			mem_reg.Count = bytes;
			return mem_reg;
		}
		virtual Splice<Byte> AllocateRaw(UInt bytes, UInt alignment) final override
		{
			Splice<Byte> mem_reg;
			mem_reg.Elements = (Byte*)MBR.allocate(bytes, alignment);
			mem_reg.Count = bytes;
			return mem_reg;
		}
		virtual void DeallocateRaw(Splice<Byte> memReg) final override { FE_LOG_CORE_WARN("Deallocation in MonotonicAllocator"); }

		template <UInt Size, UInt Alignment>
		Array<Byte, Size>* AllocateRaw() { return (Array<Byte, Size>*)MBR.allocate(Size, Alignment); }

		template <UInt Alignment>
		Splice<Byte> AllocateRaw(UInt size) { return AllocateRaw(size, Alignment); }

		template <UInt Size>
		void DeallocateRaw(Byte* ptr) { FE_LOG_CORE_WARN("Deallocation in MonotonicAllocator"); }

		void Clear() { MBR.release(); }
	};
}