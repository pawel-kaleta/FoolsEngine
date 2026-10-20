#pragma once

#include "Allocator.h"
#include "FoolsEngine/Foundation/Debug/Asserts.h"

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

		virtual Splice<Byte> ReallocateRaw(Splice<Byte> allocation, Splice<Byte> moveRegion, UInt newSize, UInt alignment) override final
		{
			if (m_Primary.DoesOwn(allocation))
			{
				Splice<Byte> result = m_Primary.ReallocateRaw(allocation, moveRegion, newSize, alignment);
				if (!(result.Elements && result.Count))
					return m_Fallback.ReallocateRaw(allocation, moveRegion, newSize, alignment);
				return result;
			}
			else
				return m_Fallback.ReallocateRaw(allocation, moveRegion, newSize, alignment);
		};

		template <UInt Alignment>
		Splice<Byte> ReallocateRaw(Splice<Byte> allocation, Splice<Byte> moveRegion, UInt newSize)
		{
			if (m_Primary.DoesOwn(allocation))
			{
				Splice<Byte> result = m_Primary.ReallocateRaw<Alignment>(allocation, moveRegion, newSize);
				if (!(result.Elements && result.Count))
					return m_Fallback.ReallocateRaw<Alignment>(allocation, moveRegion, newSize);
				return result;
			}
			else
				return m_Fallback.ReallocateRaw<Alignment>(allocation, moveRegion, newSize);
		};

		template <UInt Alignment>
		Splice<Byte> ReallocateRaw(Splice<Byte> allocation, UInt newSize)
		{
			if (m_Primary.DoesOwn(allocation))
			{
				Splice<Byte> result = m_Primary.ReallocateRaw<Alignment>(allocation, newSize);
				if (!(result.Elements && result.Count))
					return m_Fallback.ReallocateRaw<Alignment>(allocation, newSize);
				return result;
			}
			else
				return m_Fallback.ReallocateRaw<Alignment>(allocation, newSize);
		};

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