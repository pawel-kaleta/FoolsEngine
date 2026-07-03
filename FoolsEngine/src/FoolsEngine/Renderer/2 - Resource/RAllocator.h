#pragma once

#include "RBuffer.h"

#include "FoolsEngine/Foundation/Memory/Bitset.h"

namespace fe::Resource
{
	class RAllocator
	{
	public:
		virtual RMemReg Allocate(UInt bytes) = 0;
		virtual RMemReg Allocate(UInt bytes, UInt alignment) = 0;
		virtual void Deallocate(RMemReg memReg) = 0;

		template <UInt Size, UInt Alignment>
		RMemReg Allocate() { return Allocate(Size, Alignment); }

		template <UInt Alignment>
		RMemReg Allocate(UInt bytes) { return Allocate(bytes, Alignment); }

		template <UInt Size>
		void Deallocate(RMemReg memReg) { Deallocate(memReg); } // assert Size == memReg.Size
	};

	class RArenaAlloc : public RAllocator
	{
	public:
		RBuffer* Buffer;
		UInt Free;

		void Clear() { Free = 0; }

		virtual RMemReg Allocate(UInt bytes) override final
		{
			RMemReg memReg;
			memReg.Offset = (U32)AlignTo<8>((Byte*)Free);
			UInt new_free = memReg.Offset + bytes;
			if (new_free > Buffer->Size)
			{
				FE_CORE_ASSERT(false, "RArenaAllocator overflow!");
				memReg.Buffer = nullptr;
				return memReg;
			}
			memReg.Buffer = Buffer;
			memReg.Size = bytes;
			Free = new_free;
			return memReg;
		}
		virtual RMemReg Allocate(UInt bytes, UInt alignment) override final
		{
			RMemReg memReg;
			memReg.Offset = (U32)AlignTo((Byte*)Free, alignment);
			UInt new_free = memReg.Offset + bytes;
			if (new_free > Buffer->Size)
			{
				FE_CORE_ASSERT(false, "RArenaAllocator overflow!");
				memReg.Buffer = nullptr;
				return memReg;
			}
			memReg.Buffer = Buffer;
			memReg.Size = bytes;
			Free = new_free;
			return memReg;
		}
		virtual void Deallocate(RMemReg memReg) override final { FE_CORE_ASSERT(false, "RArenaAllocator does not deallocate individually"); }

		template <UInt Size, UInt Alignment>
		RMemReg Allocate()
		{
			RMemReg memReg;
			memReg.Offset = (U32)AlignTo<Alignment>((Byte*)Free);
			UInt new_free = memReg.Offset + Size;
			if (new_free > Buffer->Size)
			{
				FE_CORE_ASSERT(false, "RArenaAllocator overflow!");
				memReg.Buffer = nullptr;
				return memReg;
			}
			memReg.Buffer = Buffer;
			memReg.Size = Size;
			Free = new_free;
			return memReg;
		}

		template <UInt Alignment>
		RMemReg Allocate(UInt bytes)
		{
			RMemReg memReg;
			memReg.Offset = (U32)AlignTo<Alignment>((Byte*)Free);
			UInt new_free = memReg.Offset + bytes;
			if (new_free > Buffer->Size)
			{
				FE_CORE_ASSERT(false, "RArenaAllocator overflow!");
				memReg.Buffer = nullptr;
				return memReg;
			}
			memReg.Buffer = Buffer;
			memReg.Size = bytes;
			Free = new_free;
			return memReg;
		}

		template <UInt Size>
		void Deallocate(Byte* ptr) { FE_CORE_ASSERT(false, "RArenaAllocator does not deallocate individually"); }
	};
	
	template <UInt memRegSize, UInt memRegCount>
	class RBitmappedPoolAlloc : public RAllocator
	{
	public:
		inline static constexpr UInt MemRegSize = memRegSize;
		inline static constexpr UInt MemRegCount = memRegCount;
		inline static constexpr UInt TotalSize = memRegSize * 64;

		RBuffer* Buffer;
		//true is free

		Bitset<memRegCount> Bits;

		virtual RMemReg Allocate(UInt bytes) override final
		{
			RMemReg result;

			FE_CORE_ASSERT(false, "Not implemented");

			FE_CORE_ASSERT(bytes > memRegSize, "This BitmappedPoolAllocator cannot accomdate allocation of this size!");

			for (UInt i = 0; i < Bits.WordsCount; i++)
			{
				if (Bits.Data[i] == -1)
					continue;

				unsigned long outIndex;
				MSB64(&outIndex, m_BitMapping);
				U64 flag_mask = (U64)1 << outIndex;
				m_BitMapping &= ~flag_mask;
			}

			if (!m_BitMapping)
			{// out of free MemRegs
				result.Buffer = nullptr;
				return result;
			}

			

			result.Buffer = Buffer;
			result.Offset = MemRegSize * (outIndex-1);
			result.Size = bytes;

			return result;
		}
		virtual RMemReg Allocate(UInt bytes, UInt alignment) override final
		{
			RMemReg result;

			FE_CORE_ASSERT(bytes > memRegSize, "This BitmappedPoolAllocator cannot accomdate allocation of this size!");

			if (!m_BitMapping)
			{// out of free MemRegs
				result.Buffer = nullptr;
				return result;
			}

			unsigned long outIndex;
			MSB64(&outIndex, m_BitMapping);
			U64 flag_mask = (U64)1 << outIndex;
			m_BitMapping &= ~flag_mask;

			UInt base_offset = MemRegSize * outIndex;
			UInt alligned_offset = (UInt)AlignTo((Byte*)base_offset, alignment);

			if (alligned_offset - base_offset + bytes > memRegSize)
			{
				result.Buffer = nullptr;
				return result;
			}

			result.Offset = alligned_offset;
			result.Buffer = Buffer;
			result.Size = bytes;

			return result;
		}
		virtual void Deallocate(RMemReg memReg) override final
		{
			FE_CORE_ASSERT(memReg.Buffer == Buffer, "This BitmappedPoolAllocator does not own this MemReg");

			UInt index = memReg.Offset / memRegSize;
			U64 flag_mask = (U64)1 << (63 - index);
			m_BitMapping &= flag_mask;
		}

		template <UInt Size, UInt Alignment>
		RMemReg Allocate()
		{
			RMemReg result;
			FE_CORE_ASSERT(false, "Not implemented");
			return result;
		}

		template <UInt Alignment>
		RMemReg Allocate(UInt size)
		{
			RMemReg result;
			FE_CORE_ASSERT(false, "Not implemented");
			return result;
		}

		template <UInt Size>
		void Deallocate(RMemReg memReg)
		{
			FE_CORE_ASSERT(memReg.Buffer == Buffer, "This BitmappedPoolAllocator does not own this MemReg");

			UInt index = memReg.Offset / memRegSize;
			U64 flag_mask = (U64)1 << (63 - index);
			m_BitMapping &= flag_mask;
		}
	};

	class RCoallesingAlloc : public RAllocator
	{
	public:
		RBuffer* Buffer;

		virtual RMemReg Allocate(UInt bytes) override final
		{
			RMemReg result;

			FE_CORE_ASSERT(false, "Not implemented");
			

			return result;
		}
		virtual RMemReg Allocate(UInt bytes, UInt alignment) override final
		{
			RMemReg result;
			FE_CORE_ASSERT(false, "Not implemented");
			
			return result;
		}
		virtual void Deallocate(RMemReg memReg) override final
		{
			FE_CORE_ASSERT(memReg.Buffer == Buffer, "This BitmappedPoolAllocator does not own this MemReg");
			FE_CORE_ASSERT(false, "Not implemented");
			
		}

		template <UInt Size, UInt Alignment>
		RMemReg Allocate()
		{
			RMemReg result;
			FE_CORE_ASSERT(false, "Not implemented");
			return result;
		}

		template <UInt Alignment>
		RMemReg Allocate(UInt size)
		{
			RMemReg result;
			FE_CORE_ASSERT(false, "Not implemented");
			return result;
		}

		template <UInt Size>
		void Deallocate(RMemReg memReg)
		{
			FE_CORE_ASSERT(false, "Not implemented");
		}
	};
}