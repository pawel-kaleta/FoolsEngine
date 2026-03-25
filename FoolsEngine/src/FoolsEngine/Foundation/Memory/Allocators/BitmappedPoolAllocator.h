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

		template <typename T, UInt Count>
		Array<T, Count>* Allocate()
		{
			auto result = Allocate<Count * sizeof(T), alignof(T)>();
			return (Array<T, Count>*)result;
		}

		template <typename T>
		T* Allocate() { return (T*)Allocate<sizeof(T), alignof(T)>(); }

		template <typename T>
		Splice<T> Allocate(UInt Count)
		{
			Splice<Byte> result = Allocate<alignof(T)>(sizeof(T) * Count);
			result.Count = Count;
			return *(Splice<T>*) & result;
		}

		template <typename T>
		void Deallocate(T* ptr) { Deallocate<sizeof(T)>((Byte*)ptr); }

		template <typename T, UInt Count>
		void Deallocate(Array<T, Count>* ptr) { Deallocate<sizeof(T)* Count>((Byte*)ptr); }

		template <typename T>
		void Deallocate(Splice<T> splice)
		{
			splice.Count *= sizeof(T);
			Deallocate(*(Splice<Byte>*) & splice);
		}
	protected:
		virtual Splice<Byte> Allocate(UInt bytes) override final
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
		virtual Splice<Byte> Allocate(UInt bytes, UInt alignment) override final
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
		virtual void Deallocate(Splice<Byte> memReg) override final
		{
			FE_CORE_ASSERT(DoesOwn(memReg), "This BitmappedPoolAllocator does not own this MemReg");

			UInt index = (memReg.Elements - m_Regions) / memRegSize;
			U64 flag_mask = (U64)1 << (63 - index);
			m_BitMapping &= flag_mask;
		}

		template <UInt Size, UInt Alignment>
		Array<Byte, Size>* Allocate()
		{
			static_assert(Alignment > memRegAlignment, "Overalignment in BitmappedPoolAllocator");
			static_assert(Size > memRegSize, "This BitmappedPoolAllocator cannot accomdate allocation of this size!");
			return (Array<Byte, Size>*)Allocate(Size, Alignment).Elements;
		}

		template <UInt Alignment>
		Splice<Byte> Allocate(UInt size)
		{
			static_assert(Alignment > memRegAlignment, "Overalignment in BitmappedPoolAllocator");
			return Allocate(size, Alignment);
		}

		template <UInt Size>
		void Deallocate(Byte* ptr)
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