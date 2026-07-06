#pragma once

#include "RBuffer.h"

#include "FoolsEngine/Foundation/Memory/Bitset.h"
#include "FoolsEngine/Foundation/Memory/Xar.h"

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

		//true is occupied
		Bitset<memRegCount> Bits;

		virtual RMemReg Allocate(UInt bytes) override final
		{
			FE_CORE_ASSERT(bytes > memRegSize, "This BitmappedPoolAllocator cannot accomdate allocation of this size!");

			RMemReg result;
			UInt position = Bits.FirstFalse();

			if (position != -1)
			{
				Bits.Set(position);
				result.Buffer = Buffer;
				result.Offset = MemRegSize * position;
				result.Size = bytes;
			}
			else
			{
				result.Buffer = nullptr;
				result.Offset = 0;
				result.Size = 0;
			}

			return result;
		}
		virtual RMemReg Allocate(UInt bytes, UInt alignment) override final
		{
			FE_CORE_ASSERT(memRegSize % alignment == 0, "This allocator cannot accomodate this allignment");
			return Allocate(bytes);
		}
		virtual void Deallocate(RMemReg memReg) override final
		{
			FE_CORE_ASSERT(memReg.Buffer == Buffer, "This BitmappedPoolAllocator does not own this MemReg");

			UInt index = memReg.Offset / memRegSize;
			Bits.Set(index, false);
		}

		template <UInt Size, UInt Alignment>
		RMemReg Allocate()
		{
			FE_CORE_ASSERT(memRegSize % Alignment == 0, "This allocator cannot accomodate this allignment");
			FE_CORE_ASSERT(Size > memRegSize, "This BitmappedPoolAllocator cannot accomdate allocation of this size!");

			RMemReg result;
			UInt position = Bits.FirstFalse();

			if (position != -1)
			{
				Bits.Set(position);
				result.Buffer = Buffer;
				result.Offset = MemRegSize * position;
				result.Size = Size;
			}
			else
			{
				result.Buffer = nullptr;
				result.Offset = 0;
				result.Size = 0;
			}

			return result;
		}

		template <UInt Alignment>
		RMemReg Allocate(UInt size)
		{
			FE_CORE_ASSERT(memRegSize % Alignment == 0, "This allocator cannot accomodate this allignment");
			return Allocate(bytes);
		}

		template <UInt Size>
		void Deallocate(RMemReg memReg)
		{
			FE_CORE_ASSERT(memReg.Buffer == Buffer, "This BitmappedPoolAllocator does not own this MemReg");
			FE_CORE_ASSERT(memReg.Size == Size, "MemReg size != provided size in deallocation");

			UInt index = memReg.Offset / memRegSize;
			Bits.Set(index, false);
		}
	};

	class RCoallesingAlloc : public RAllocator
	{
	public:
		struct Region
		{
			UInt Offset;
			UInt Size;

		};

		RBuffer* Buffer;
		Xarr<Region> Regions;

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