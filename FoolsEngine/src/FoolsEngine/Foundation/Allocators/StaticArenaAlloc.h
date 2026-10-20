#pragma once

#include "Allocator.h"
#include "FoolsEngine/Foundation/Debug/Asserts.h"
#include "FoolsEngine/Foundation/Utils/Core.h"
#include "FoolsEngine/Foundation/Utils/BitOperations.h"


namespace fe
{
	template <UInt Size>
	class StaticArenaAlloc : public Allocator
	{
	public:
		Array<Byte, Size> Buffer;
		Byte* Free;

		void Clear() { Free = Buffer.Elements; }

		const Byte* End() const { return Buffer.end(); }
		bool IsFull() const { return Free == End(); }

		virtual Splice<Byte> AllocateRaw(UInt bytes) override final
		{
			Splice<Byte> result;
			result.Count = bytes;
			result.Elements = AlignTo<8>(Free);
			if (result.end() > End())
			{
				FE_CORE_ASSERT(false, "ArenaAllocator overflow!");
				return result.Init();
			}
			Free = result.end();
			return result;
		}
		virtual Splice<Byte> AllocateRaw(UInt bytes, UInt alignment) override final
		{
			Splice<Byte> result;
			result.Count = bytes;
			result.Elements = AlignTo(Free, alignment);
			if (result.end() > End())
			{
				FE_CORE_ASSERT(false, "ArenaAllocator overflow!");
				return result.Init();
			}
			Free = result.end();
			return result;
		}

		virtual void DeallocateRaw(Splice<Byte> memReg) override final
		{
			FE_CORE_ASSERT(DoesOwn(memReg.begin()), "ArenaAllocator does not own this memory");

			if (memReg.end() == Free)
			{
				Free = memReg.begin();
			}
		}

		virtual Splice<Byte> ReallocateRaw(Splice<Byte> allocation, Splice<Byte> moveRegion, UInt newSize, UInt alignment) override final
		{
			FE_CORE_ASSERT(DoesOwn(allocation.begin()), "ArenaAllocator does not own this memory");
			FE_CORE_ASSERT(AlignTo(allocation.begin(), alignment) == allocation.begin(), "Cannot realloc to a different alignment");

			if (allocation.end() == Free)
			{
				Free += (newSize - allocation.Count);
				allocation.Count = newSize;
				return allocation;
			}

			UInt move_offset = moveRegion.begin() - allocation.begin();

			Splice<Byte> result = AllocateRaw(newSize, alignment);
			if (result.Elements && result.Count)
				std::memcpy(result.begin() + move_offset, moveRegion.begin(), min(moveRegion.Count, newSize));
			return result;
		};

		template <UInt Alignment>
		Splice<Byte> ReallocateRaw(Splice<Byte> allocation, Splice<Byte> moveRegion, UInt newSize)
		{
			FE_CORE_ASSERT(DoesOwn(allocation.begin()), "ArenaAllocator does not own this memory");
			FE_CORE_ASSERT(AlignTo(allocation.begin(), Alignment) == allocation.begin(), "Cannot realloc to a different alignment");

			if (allocation.end() == Free)
			{
				Free += (newSize - allocation.Count);
				allocation.Count = newSize;
				return allocation;
			}

			UInt move_offset = moveRegion.begin() - allocation.begin();

			Splice<Byte> result = AllocateRaw<Alignment>(newSize);
			if (result.Elements && result.Count)
				std::memcpy(result.begin() + move_offset, moveRegion.begin(), min(moveRegion.Count, newSize));
			return result;
		};

		template <UInt Alignment>
		Splice<Byte> ReallocateRaw(Splice<Byte> allocation, UInt newSize)
		{
			FE_CORE_ASSERT(DoesOwn(allocation.begin()), "ArenaAllocator does not own this memory");
			FE_CORE_ASSERT(AlignTo(allocation.begin(), Alignment) == allocation.begin(), "Cannot realloc to a different alignment");

			if (allocation.end() == Free)
			{
				Free += (newSize - allocation.Count);
				allocation.Count = newSize;
				return allocation;
			}

			Splice<Byte> result = AllocateRaw<Alignment>(newSize);
			if (result.Elements && result.Count)
				std::memcpy(result.begin(), allocation.begin(), min(allocation.Count, newSize));
			return result;
		};

		template <UInt Size, UInt Alignment>
		Array<Byte, Size>* AllocateRaw()
		{
			Byte* allocation = AlignTo<Alignment>(Free);
			Byte* new_free = allocation + Size;
			if (new_free > End())
			{
				FE_CORE_ASSERT(false, "ArenaAllocator overflow!");
				allocation = nullptr;
			}
			Free = new_free;
			return (Array<Byte, Size>*)allocation;
		}

		template <UInt Alignment>
		Splice<Byte> AllocateRaw(UInt bytes)
		{
			Splice<Byte> result;
			result.Count = bytes;
			result.Elements = AlignTo<Alignment>(Free);
			if (result.end() > End())
			{
				FE_CORE_ASSERT(false, "ArenaAllocator overflow!");
				return result.Init();
			}
			Free = result.end();
			return result;
		}

		template <UInt Size>
		void DeallocateRaw(Byte* ptr)
		{
			FE_CORE_ASSERT(DoesOwn(ptr), "ArenaAllocator does not own this memory");

			if (ptr + Size == Free)
			{
				Free = ptr;
			}
		}

		bool DoesOwn(Splice<Byte> memReg) const { return DoesOwn(memReg.Elements); }
		bool DoesOwn(Byte* ptr) const
		{
			if (ptr >= Buffer.Elements && ptr < Free)
				return true;
			return false;
		}
	};
}