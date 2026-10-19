#pragma once

#include "FoolsEngine/Foundation/Memory/DataTypes.h"
#include "FoolsEngine/Foundation/Memory/Splice.h"

#include <cstdlib>
#include <memory_resource>

namespace fe
{
	class Allocator
	{
	public:
		FE_TYPE_SETUP(Allocator)

		virtual Splice<Byte> AllocateRaw(UInt bytes) = 0;
		virtual Splice<Byte> AllocateRaw(UInt bytes, UInt alignment) = 0;
		virtual void DeallocateRaw(Splice<Byte> memReg) = 0;

		virtual Splice<Byte> ReallocateRaw(Splice<Byte> allocation, Splice<Byte> moveRegion, UInt newSize, UInt alignment) = 0;

		template <UInt Alignment>
		Splice<Byte> ReallocateRaw(Splice<Byte> allocation, Splice<Byte> moveRegion, UInt newSize) { return ReallocateRaw(allocation, moveRegion, newSize, Alignment); };

		template <UInt Alignment>
		Splice<Byte> ReallocateRaw(Splice<Byte> allocation, UInt newSize) { return ReallocateRaw(allocation, allocation, newSize, Alignment); };

		template <UInt Size, UInt Alignment>
		Array<Byte, Size>* AllocateRaw() { return (Array<Byte, Size>*)AllocateRaw(Size, Alignment).Elements; }

		template <UInt Alignment>
		Splice<Byte> AllocateRaw(UInt bytes) { return AllocateRaw(bytes, Alignment); }

		template <UInt Size>
		void DeallocateRaw(Byte* ptr)
		{
			Splice<Byte> memReg;
			memReg.Elements = ptr;
			memReg.Count = Size;
			DeallocateRaw(memReg);
		}
	};

	template <typename tAlloc>
	class TypedAlloc : public tAlloc
	{
	public:
		FE_TYPE_SETUP(TypedAlloc<tAlloc>)

		template <typename T, UInt Count>
		Array<T, Count>* Allocate()
		{
			auto result = this->tAlloc::AllocateRaw<Count * sizeof(T), alignof(T)>();
			return (Array<T, Count>*)result;
		}
		
		template <typename T>
		T* Allocate() { return (T*)(this->tAlloc::AllocateRaw<sizeof(T), alignof(T)>()); }
		
		template <typename T>
		Splice<T> Allocate(UInt Count)
		{
			Splice<Byte> result = this->tAlloc::AllocateRaw<alignof(T)>(sizeof(T) * Count);
			result.Count = Count;
			return result.Cast<Splice<T>>();
		}
		
		//splice argument for type detection, e.g. my_splice = alloc->Allocate(my_splice, 5);
		template <typename T>
		Splice<T> Allocate(Splice<T> splice, UInt count)
		{
			Splice<Byte> result = this->tAlloc::AllocateRaw<alignof(T)>(sizeof(T) * count);
			result.Count = count;
			return result.Cast<Splice<T>>();
		}

		template <typename T>
		Splice<T> Reallocate(Splice<T> allocation, Splice<T> moveRegion, UInt newCount)
		{
			if (allocation.Count == 0 || allocation.Elements == nullptr)
				return Splice<T>().Init();

			allocation.Count = allocation.Count * sizeof(T);
			moveRegion.Count = moveRegion.Count * sizeof(T);

			Splice<Byte> result = this->tAlloc::ReallocateRaw<alignof(T)>(allocation.Cast<Splice<Byte>>(), moveRegion.Cast<Splice<Byte>>(), newCount * sizeof(T));
			result.Count = newCount;
			return result.Cast<Splice<T>>();
		}

		template <typename T>
		Splice<T> Reallocate(Splice<T> allocation, UInt newCount)
		{
			if (allocation.Count == 0 || allocation.Elements == nullptr)
				return Splice<T>().Init();

			allocation.Count = allocation.Count * sizeof(T);

			Splice<Byte> result = this->tAlloc::ReallocateRaw<alignof(T)>(allocation.Cast<Splice<Byte>>(), newCount * sizeof(T));
			result.Count = newCount;
			return result.Cast<Splice<T>>();
		}

		template <typename T>
		void Deallocate(T* ptr) { this->tAlloc::DeallocateRaw<sizeof(T)>((Byte*)ptr); }
		
		template <typename T, UInt Count>
		void Deallocate(Array<T, Count>* ptr) { this->tAlloc::DeallocateRaw<sizeof(T) * Count>((Byte*)ptr); }
		
		template <typename T>
		void Deallocate(Splice<T> splice)
		{
			if (splice.Count == 0 || splice.Elements == nullptr) return;

			Splice<Byte> splice_byte;
			splice_byte.Elements = (Byte*)splice.Elements;
			splice_byte.Count = splice.Count * sizeof(T);
			
			//this->tAlloc::DeallocateRaw(splice_byte);
			Allocator* nasty_fix = (Allocator*)this;
			nasty_fix->DeallocateRaw(splice_byte);
		}
	};

	using PMAlloc = TypedAlloc<Allocator>;

	template <class tnAllocator>
	class STD_PMR_Allocator final : public std::pmr::memory_resource
	{
	public:
		tnAllocator* fe_allocator = nullptr;

		STD_PMR_Allocator() = default;
		STD_PMR_Allocator(tnAllocator* alloc) : fe_allocator(alloc) { }

	private:
		virtual void* do_allocate(std::size_t bytes, std::size_t alignment) final override
		{
			Splice<Byte> allocation = fe_allocator->AllocateRaw((UInt)bytes, (UInt)alignment);
			return allocation.Elements;
		};
		virtual void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) final override
		{
			Splice<Byte> mem_reg;
			mem_reg.Elements = (Byte*)p;
			mem_reg.Count = bytes;
			fe_allocator->DeallocateRaw(mem_reg);
		};
		virtual bool do_is_equal(const std::pmr::memory_resource& other) const noexcept final override { return false; };
	};
}