#pragma once

#include "Allocator.h"

namespace fe
{
	class MonotonicAlloc : public Allocator
	{
	public:
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