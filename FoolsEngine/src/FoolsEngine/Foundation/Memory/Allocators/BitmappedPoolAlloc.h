#pragma once

#include "Allocator.h"

namespace fe
{
	template <UInt memRegSize, UInt memRegAlignment>
	class BitmappedPoolAlloc : public Allocator
	{
	public:
		inline static constexpr UInt MemRegAlignment = memRegAlignment;
		inline static constexpr UInt MemRegSize = memRegSize;
		inline static constexpr UInt TotalSize = memRegSize * 64;
		inline Byte* RegionsEnd() { return m_Regions + TotalSize; }

		Byte* m_Regions = nullptr;
		//true is free
		U64 m_BitMapping = 0;

		virtual Splice<Byte> AllocateRaw(UInt bytes) override final
		{
			Splice<Byte> result;

			FE_CORE_ASSERT(8 > memRegAlignment, "Overalignment in BitmappedPoolAllocator");
			FE_CORE_ASSERT(bytes > memRegSize, "This BitmappedPoolAllocator cannot accomdate allocation of this size!");

			if (!m_BitMapping)
				// out of free MemRegs
				return result;

			unsigned long outIndex;
			MSB64(&outIndex, m_BitMapping);
			U64 flag_mask = (U64)1 << outIndex;
			m_BitMapping &= ~flag_mask;

			result.Elements = m_Regions + MemRegSize * outIndex;
			result.Count = bytes;

			return result;
		}
		virtual Splice<Byte> AllocateRaw(UInt bytes, UInt alignment) override final
		{
			Splice<Byte> result;

			FE_CORE_ASSERT(alignment > memRegAlignment, "Overalignment in BitmappedPoolAllocator");
			FE_CORE_ASSERT(bytes > memRegSize, "This BitmappedPoolAllocator cannot accomdate allocation of this size!");

			if (!m_BitMapping)
				// out of free MemRegs
				return result;

			unsigned long outIndex;
			MSB64(&outIndex, m_BitMapping);
			U64 flag_mask = (U64)1 << outIndex;
			m_BitMapping &= ~flag_mask;

			result.Elements = m_Regions + MemRegSize * outIndex;
			result.Count = bytes;

			return result;
		}
		virtual void DeallocateRaw(Splice<Byte> memReg) override final
		{
			FE_CORE_ASSERT(DoesOwn(memReg), "This BitmappedPoolAllocator does not own this MemReg");

			UInt index = (memReg.Elements - m_Regions) / memRegSize;
			U64 flag_mask = (U64)1 << (63 - index);
			m_BitMapping &= flag_mask;
		}

		template <UInt Size, UInt Alignment>
		Array<Byte, Size>* AllocateRaw()
		{
			static_assert(Alignment > memRegAlignment, "Overalignment in BitmappedPoolAllocator");
			static_assert(Size > memRegSize, "This BitmappedPoolAllocator cannot accomdate allocation of this size!");
			return (Array<Byte, Size>*)Allocate(Size, Alignment).Elements;
		}

		template <UInt Alignment>
		Splice<Byte> AllocateRaw(UInt size)
		{
			static_assert(Alignment > memRegAlignment, "Overalignment in BitmappedPoolAllocator");
			return Allocate(size, Alignment);
		}

		template <UInt Size>
		void DeallocateRaw(Byte* ptr)
		{
			static_assert(Size > memRegSize, "Oversized deallocation in BitmappedPoolAllocator!");
			FE_CORE_ASSERT(DoesOwn(ptr), "This BitmappedPoolAllocator does not own this MemReg");

			UInt index = (ptr - m_Regions) / memRegSize;
			U64 flag_mask = (U64)1 << (63 - index);
			m_BitMapping &= flag_mask;
		}

		bool DoesOwn(Splice<Byte> memReg) { return DoesOwn(memReg.Elements); }
		bool DoesOwn(Byte* ptr)
		{
			bool lower_bound = ptr >= m_Regions;
			bool  uper_bound = ptr < RegionsEnd();
			if (ptr && lower_bound && uper_bound)
				return true;
			return false;
		}
	};
}