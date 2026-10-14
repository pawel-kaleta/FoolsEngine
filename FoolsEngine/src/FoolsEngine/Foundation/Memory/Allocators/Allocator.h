#pragma once

#include "FoolsEngine/Foundation/Debug/Asserts.h"
#include "FoolsEngine/Foundation/Utils/BitOperations.h"
#include "FoolsEngine/Foundation/Memory/DataTypes.h"
#include "FoolsEngine/Foundation/Memory/Splice.h"

#include <cstdlib>
#include <memory_resource>

namespace fe
{
	class Allocator
	{
	public:
		virtual Splice<Byte> AllocateRaw(UInt bytes) = 0;
		virtual Splice<Byte> AllocateRaw(UInt bytes, UInt alignment) = 0;
		virtual void DeallocateRaw(Splice<Byte> memReg) = 0;

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

	// example simple allocator
	class NullAlloc : public Allocator
	{
	public:
		virtual Splice<Byte> AllocateRaw(UInt bytes) override final { return Splice<Byte>(); }
		virtual Splice<Byte> AllocateRaw(UInt bytes, UInt alignment) override final { return Splice<Byte>(); }
		virtual void DeallocateRaw(Splice<Byte> memReg) override final { FE_CORE_ASSERT(!memReg.Elements, "NullAllocator should not be used for deallocation"); }

		template <UInt Size, UInt Alignment>
		Array<Byte, Size>* AllocateRaw() { return nullptr; }

		template <UInt Alignment>
		Splice<Byte> AllocateRaw(UInt bytes) { return Splice<Byte>(); }

		template <UInt Size>
		void DeallocateRaw(Byte* ptr) { FE_CORE_ASSERT(!ptr, "NullAllocator should not be used for deallocation"); }
	};

	template <typename tAlloc>
	class TypedAlloc : public tAlloc
	{
	public:
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
			return *(Splice<T>*) & result;
		}
		
		//splice argument for type detection, e.g. my_splice = alloc->Allocate(my_splice, 5);
		template <typename T>
		Splice<T> Allocate(Splice<T> splice, UInt count)
		{
			Splice<Byte> result = this->tAlloc::AllocateRaw<alignof(T)>(sizeof(T) * count);
			result.Count = count;
			return *(Splice<T>*) & result;
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