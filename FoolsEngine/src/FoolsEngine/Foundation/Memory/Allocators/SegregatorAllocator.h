#pragma once

#include "Allocator.h"

namespace fe
{
	template <class SmallAllocator, class BigAllocator, UInt threshold>
	class SegragatorAlloc : public Allocator
	{
	public:
		SmallAllocator m_SmallAllocator;
		BigAllocator m_BigAllocator;

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
			if (bytes <= threshold)
				return m_SmallAllocator.Allocate(bytes);

			return m_BigAllocator.Allocate(bytes);
		}
		virtual Splice<Byte> Allocate(UInt bytes, UInt alignment) override final
		{
			if (bytes <= threshold)
				return m_SmallAllocator.Allocate(bytes, alignment);

			return m_BigAllocator.Allocate(bytes, alignment);
		}
		virtual void Deallocate(Splice<Byte> memReg) override final
		{
			if (memReg.Count <= threshold)
				m_SmallAllocator.Deallocate(memReg);
			else
				m_BigAllocator.Deallocate(memReg);
		}

		template <UInt Size, UInt Alignment = 8>
		Array<Byte, Size>* Allocate()
		{
			if constexpr (Size <= threshold)
				return m_SmallAllocator.Allocate<Size, Alignment>();
			else
				return m_BigAllocator.Allocate<Size, Alignment>();
		}

		template <UInt Alignment>
		Splice<Byte> Allocate(UInt bytes) { return Allocate(bytes, Alignment); }

		template <UInt Size>
		void Deallocate(Byte* ptr)
		{
			if constexpr (Size <= threshold)
				return m_SmallAllocator.Deallocate<Size>(ptr);
			else
				return m_BigAllocator.Deallocate<Size>(ptr);
		}

		bool DoesOwn(Splice<Byte> memReg)
		{
			if (memReg.Count <= threshold)
				return m_SmallAllocator.DoesOwn(memReg);
			else
				return m_BigAllocator.DoesOwn(memReg);
		}

		template <UInt Size>
		bool DoesOwn(Byte* ptr)
		{
			Splice<Byte> mem_reg;
			mem_reg.Count = Size;
			mem_reg.Elements = ptr;
			if constexpr (Size <= threshold)
				return m_SmallAllocator.DoesOwn(mem_reg);
			else
				return m_BigAllocator.DoesOwn(mem_reg);
		}
	};
}