#pragma once

#include "Allocator.h"

namespace fe
{
	class ArenaAlloc : public Allocator
	{
	public:
		Splice<Byte> Buffer;
		Byte* Free;

		void Clear() { Free = Buffer.Elements; }

		const Byte* End() const { return Buffer.Elements + Buffer.Count; }
		bool IsFull() const { return Free == End(); }

		virtual Splice<Byte> AllocateRaw(UInt bytes) override final
		{
			Splice<Byte> memReg;
			memReg.Count = bytes;
			memReg.Elements = AlignTo<8>(Free);
			Byte* new_free = memReg.Elements + memReg.Count;
			if (new_free > End())
			{
				FE_CORE_ASSERT(false, "ArenaAllocator overflow!");
				memReg.Elements = nullptr;
			}
			Free = new_free;
			return memReg;
		}
		virtual Splice<Byte> AllocateRaw(UInt bytes, UInt alignment) override final
		{
			Splice<Byte> memReg;
			memReg.Count = bytes;
			memReg.Elements = AlignTo(Free, alignment);
			Byte* new_free = memReg.Elements + memReg.Count;
			if (new_free > End())
			{
				FE_CORE_ASSERT(false, "ArenaAllocator overflow!");
				memReg.Elements = nullptr;
			}
			Free = new_free;
			return memReg;
		}
		virtual void DeallocateRaw(Splice<Byte> memReg) override final { FE_CORE_ASSERT(DoesOwn(memReg.Elements), "ArenaAllocator does not own this memory"); }
	
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
			Splice<Byte> memReg;
			memReg.Count = bytes;
			memReg.Elements = AlignTo<Alignment>(Free);
			Byte* new_free = memReg.Elements + bytes;
			if (new_free > End())
			{
				FE_CORE_ASSERT(false, "ArenaAllocator overflow!");
				memReg.Elements = nullptr;
			}
			Free = new_free;
			return memReg;
		}

		template <UInt Size>
		void DeallocateRaw(Byte* ptr) { FE_CORE_ASSERT(DoesOwn(ptr), "ArenaAllocator does not own this memory"); }

		bool DoesOwn(Splice<Byte> memReg) const { return DoesOwn(memReg.Elements); }
		bool DoesOwn(Byte* ptr) const
		{
			if (ptr >= Buffer.Elements && ptr < Free)
				return true;
			return false;
		}
	};
}