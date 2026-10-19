#pragma once

#include "Allocator.h"
#include "FoolsEngine/Foundation/Utils/Core.h"
#include "FoolsEngine/Foundation/Debug/Log.h"

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

		virtual Splice<Byte> ReallocateRaw(Splice<Byte> allocation, Splice<Byte> moveRegion, UInt newSize, UInt alignment) override final
		{
			FE_CORE_ASSERT(AlignTo(allocation.begin(), alignment) == allocation.begin(), "Cannot realloc to a different alignment");

			Splice<Byte> result;
			result.Elements = (Byte*)MBR.allocate(newSize, alignment);
			result.Count = newSize;

			if (result.Elements && result.Count)
				std::memcpy(result.begin(), allocation.begin(), min(moveRegion.Count, newSize));

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
		Array<Byte, Size>* AllocateRaw() { return (Array<Byte, Size>*)MBR.allocate(Size, Alignment); }

		template <UInt Alignment>
		Splice<Byte> AllocateRaw(UInt size) { return AllocateRaw(size, Alignment); }

		template <UInt Size>
		void DeallocateRaw(Byte* ptr) { FE_LOG_CORE_WARN("Deallocation in MonotonicAllocator"); }

		void Clear() { MBR.release(); }
	};
}