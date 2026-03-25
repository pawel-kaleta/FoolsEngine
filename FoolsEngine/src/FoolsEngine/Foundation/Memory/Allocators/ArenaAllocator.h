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
			Splice<Byte> memReg;
			memReg.Count = bytes;
			memReg.Elements = (Byte*)(((UInt)Free + (7)) & ~(7)); // 7 is max_natural_align -1
			Byte* new_free = memReg.Elements + memReg.Count;
			if (new_free > End())
			{
				FE_CORE_ASSERT(false, "ArenaAllocator overflow!");
				memReg.Elements = nullptr;
			}
			Free = new_free;
			return memReg;
		}
		virtual Splice<Byte> Allocate(UInt bytes, UInt alignment) override final
		{
			Splice<Byte> memReg;
			memReg.Count = bytes;
			memReg.Elements = (Byte*)(((UInt)Free + (alignment - 1)) & ~(alignment - 1));
			Byte* new_free = memReg.Elements + memReg.Count;
			if (new_free > End())
			{
				FE_CORE_ASSERT(false, "ArenaAllocator overflow!");
				memReg.Elements = nullptr;
			}
			Free = new_free;
			return memReg;
		}
		virtual void Deallocate(Splice<Byte> memReg) override final { FE_CORE_ASSERT(DoesOwn(memReg.Elements), "ArenaAllocator does not own this memory"); }

		template <UInt Size, UInt Alignment>
		Array<Byte, Size>* Allocate()
		{
			Byte* allocation = (Byte*)(((UInt)Free + (Alignment - 1)) & ~(Alignment - 1));
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
		Splice<Byte> Allocate(UInt bytes)
		{
			Splice<Byte> memReg;
			memReg.Count = bytes;
			memReg.Elements = (Byte*)(((UInt)Free + (Alignment - 1)) & ~(Alignment - 1));
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
		void Deallocate(Byte* ptr) { FE_CORE_ASSERT(DoesOwn(ptr), "ArenaAllocator does not own this memory"); }

		bool DoesOwn(Splice<Byte> memReg) const { return DoesOwn(memReg.Elements); }
		bool DoesOwn(Byte* ptr) const
		{
			if (ptr >= Buffer.Elements && ptr < Free)
				return true;
			return false;
		}
	};
}