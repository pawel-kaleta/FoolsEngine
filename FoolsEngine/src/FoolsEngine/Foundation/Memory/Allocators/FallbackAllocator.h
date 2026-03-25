#pragma once

#include "Allocator.h"

namespace fe
{
	template <class Primary, class Fallback>
	class FallbackAlloc : public Allocator
	{
	public:
		Primary m_Primary;
		Fallback m_Fallback;

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
		virtual Splice<Byte> Allocate(UInt bytes) final override
		{
			Splice<Byte> mem_reg = m_Primary.Allocate(bytes);
			if (!mem_reg.Elements)
				mem_reg = m_Fallback.Allocate(bytes);

			return mem_reg;
		}
		virtual Splice<Byte> Allocate(UInt bytes, UInt alignment) final override
		{
			Splice<Byte> mem_reg = m_Primary.Allocate(bytes, alignment);
			if (!mem_reg.Data)
				mem_reg = m_Fallback.Allocate(bytes, alignment);

			return mem_reg;
		}
		virtual void Deallocate(Splice<Byte> memReg) final override
		{
			if (m_Primary.DoesOwn(memReg))
				m_Primary.Deallocate(memReg);
			else
				m_Fallback.Deallocate(memReg);
		}

		template <UInt Size, UInt Alignment>
		Array<Byte, Size>* Allocate()
		{
			Array<Byte, Size>* result = m_Primary.Allocate<Size, Alignment>();
			if (!result)
				result = m_Fallback.Allocate<Size, Alignment>();

			return result;
		}

		template <UInt Alignment>
		Splice<Byte> Allocate(UInt bytes)
		{
			Splice<Byte> mem_reg = m_Primary.Allocate<Alignment>(bytes);
			if (!mem_reg.Elements)
				mem_reg = m_Fallback.Allocate<Alignment>(bytes);

			return mem_reg;
		}

		template <UInt Size>
		void Deallocate(Byte* ptr)
		{
			Splice<Byte> mem_reg;
			mem_reg.Elements = ptr;
			mem_reg.Count = Size;
			if (m_Primary.DoesOwn(mem_reg))
				m_Primary.Deallocate<Size>(ptr);
			else
				m_Fallback.Deallocate<Size>(ptr);
		}

		bool DoesOwn(Splice<Byte> memReg) { return m_Primary.DoesOwn(memReg) || m_Fallback.DoesOwn(memReg); }
	};
}