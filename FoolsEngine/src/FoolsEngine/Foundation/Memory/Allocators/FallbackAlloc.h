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

		virtual Splice<Byte> AllocateRaw(UInt bytes) final override
		{
			Splice<Byte> mem_reg = m_Primary.Allocate(bytes);
			if (!mem_reg.Elements)
				mem_reg = m_Fallback.Allocate(bytes);

			return mem_reg;
		}
		virtual Splice<Byte> AllocateRaw(UInt bytes, UInt alignment) final override
		{
			Splice<Byte> mem_reg = m_Primary.Allocate(bytes, alignment);
			if (!mem_reg.Data)
				mem_reg = m_Fallback.Allocate(bytes, alignment);

			return mem_reg;
		}
		virtual void DeallocateRaw(Splice<Byte> memReg) final override
		{
			if (m_Primary.DoesOwn(memReg))
				m_Primary.Deallocate(memReg);
			else
				m_Fallback.Deallocate(memReg);
		}

		template <UInt Size, UInt Alignment>
		Array<Byte, Size>* AllocateRaw()
		{
			Array<Byte, Size>* result = m_Primary.Allocate<Size, Alignment>();
			if (!result)
				result = m_Fallback.Allocate<Size, Alignment>();

			return result;
		}

		template <UInt Alignment>
		Splice<Byte> AllocateRaw(UInt bytes)
		{
			Splice<Byte> mem_reg = m_Primary.Allocate<Alignment>(bytes);
			if (!mem_reg.Elements)
				mem_reg = m_Fallback.Allocate<Alignment>(bytes);

			return mem_reg;
		}

		template <UInt Size>
		void DeallocateRaw(Byte* ptr)
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