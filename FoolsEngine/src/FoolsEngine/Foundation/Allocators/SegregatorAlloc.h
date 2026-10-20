#pragma once

#include "Allocator.h"
#include "FoolsEngine/Foundation/Debug/Asserts.h"
#include "FoolsEngine/Foundation/Utils/Core.h"

namespace fe
{
	template <class SmallAllocator, class BigAllocator, UInt threshold>
	class SegragatorAlloc : public Allocator
	{
	public:
		SmallAllocator m_SmallAllocator;
		BigAllocator m_BigAllocator;

		virtual Splice<Byte> AllocateRaw(UInt bytes) override final
		{
			if (bytes <= threshold)
				return m_SmallAllocator.Allocate(bytes);

			return m_BigAllocator.Allocate(bytes);
		}
		virtual Splice<Byte> AllocateRaw(UInt bytes, UInt alignment) override final
		{
			if (bytes <= threshold)
				return m_SmallAllocator.Allocate(bytes, alignment);

			return m_BigAllocator.Allocate(bytes, alignment);
		}
		virtual void DeallocateRaw(Splice<Byte> memReg) override final
		{
			if (memReg.Count <= threshold)
				m_SmallAllocator.Deallocate(memReg);
			else
				m_BigAllocator.Deallocate(memReg);
		}

		virtual Splice<Byte> ReallocateRaw(Splice<Byte> allocation, Splice<Byte> moveRegion, UInt newSize, UInt alignment) override final
		{
			FE_CORE_ASSERT(AlignTo(allocation.begin(), alignment) == allocation.begin(), "Cannot realloc to a different alignment");

			if constexpr (newSize <= threshold && allocation.Count <= threshold)
				return m_SmallAllocator.Reallocate(allocation, moveRegion, newSize, alignment);
			else if (newSize <= threshold)
			{
				Splice<Byte> result = m_SmallAllocator.Allocate(newSize, alignment);

				if (result.Elements && result.Count)
				{
					UInt move_offset = moveRegion.begin() - allocation.begin();
					std::memcpy(result.begin() + move_offset, allocation.begin(), min(allocation.Count, newSize));
				}

				m_BigAllocator.Deallocate(allocation);;

				return result;
			}
			else
			{
				Splice<Byte> result = m_BigAllocator.Allocate(newSize, alignment);

				if (result.Elements && result.Count)
				{
					UInt move_offset = moveRegion.begin() - allocation.begin();
					std::memcpy(result.begin() + move_offset, allocation.begin(), min(allocation.Count, newSize));
				}

				m_SmallAllocator.Deallocate(allocation);;

				return result;
			}
		};

		template <UInt Alignment>
		Splice<Byte> ReallocateRaw(Splice<Byte> allocation, Splice<Byte> moveRegion, UInt newSize)
		{
			FE_CORE_ASSERT(AlignTo(allocation.begin(), Alignment) == allocation.begin(), "Cannot realloc to a different alignment");

			if constexpr (newSize <= threshold && allocation.Count <= threshold)
				return m_SmallAllocator.Reallocate<Alignment>(allocation, moveRegion, newSize);
			else if (newSize <= threshold)
			{
				Splice<Byte> result = m_SmallAllocator.Allocate<Alignment>(newSize);

				if (result.Elements && result.Count)
				{
					UInt move_offset = moveRegion.begin() - allocation.begin();
					std::memcpy(result.begin() + move_offset, allocation.begin(), min(allocation.Count, newSize));
				}

				m_BigAllocator.Deallocate(allocation);;

				return result;
			}
			else if (allocation.Count <= threshold)
			{
				Splice<Byte> result = m_BigAllocator.Allocate<Alignment>(newSize);

				if (result.Elements && result.Count)
				{
					UInt move_offset = moveRegion.begin() - allocation.begin();
					std::memcpy(result.begin() + move_offset, allocation.begin(), min(allocation.Count, newSize));
				}

				m_SmallAllocator.Deallocate(allocation);;

				return result;
			}
			else
			{
				return m_BigAllocator.Reallocate<Alignment>(allocation, moveRegion, newSize);
			}
		};

		template <UInt Alignment>
		Splice<Byte> ReallocateRaw(Splice<Byte> allocation, UInt newSize)
		{
			FE_CORE_ASSERT(AlignTo(allocation.begin(), Alignment) == allocation.begin(), "Cannot realloc to a different alignment");

			if constexpr (newSize <= threshold && allocation.Count <= threshold)
				return m_SmallAllocator.Reallocate<Alignment>(allocation, allocation, newSize);
			else if (newSize <= threshold)
			{
				Splice<Byte> result = m_SmallAllocator.Allocate<Alignment>(newSize);

				if (result.Elements && result.Count)
					std::memcpy(result.begin(), allocation.begin(), allocation.Count);

				m_BigAllocator.Deallocate(allocation);;

				return result;
			}
			else  if (allocation.Count <= threshold)
			{
				Splice<Byte> result = m_BigAllocator.Allocate<Alignment>(newSize);

				if (result.Elements && result.Count)
					std::memcpy(result.begin(), allocation.begin(), allocation.Count);

				m_SmallAllocator.Deallocate(allocation);;

				return result;
			}
			else
			{
				return m_BigAllocator.Reallocate<Alignment>(allocation, allocation, newSize);
			}
		};

		template <UInt Size, UInt Alignment = 8>
		Array<Byte, Size>* AllocateRaw()
		{
			if constexpr (Size <= threshold)
				return m_SmallAllocator.Allocate<Size, Alignment>();
			else
				return m_BigAllocator.Allocate<Size, Alignment>();
		}

		template <UInt Alignment>
		Splice<Byte> AllocateRaw(UInt bytes) { return Allocate(bytes, Alignment); }

		template <UInt Size>
		void DeallocateRaw(Byte* ptr)
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