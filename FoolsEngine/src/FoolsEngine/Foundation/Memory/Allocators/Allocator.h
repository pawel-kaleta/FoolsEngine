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
		virtual Splice<Byte> Allocate(UInt bytes) = 0;
		virtual Splice<Byte> Allocate(UInt bytes, UInt alignment) = 0;
		virtual void Deallocate(Splice<Byte> memReg) = 0;

		// below is interface definition for specific allocators
		template <UInt Size, UInt Alignment>
		Array<Byte, Size>* Allocate() { return (Array<Byte, Size>*)Allocate(Size, Alignment).Elements; }

		template <UInt Alignment>
		Splice<Byte> Allocate(UInt bytes) { return Allocate(bytes, Alignment); }

		template <UInt Size>
		void Deallocate(Byte* ptr)
		{
			Splice<Byte> memReg;
			memReg.Elements = ptr;
			memReg.Count = Size;
			Deallocate(memReg);
		}
	};

	// example simple allocator
	class NullAlloc : public Allocator
	{
	public:
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
		virtual Splice<Byte> Allocate(UInt bytes) override final { return Splice<Byte>(); }
		virtual Splice<Byte> Allocate(UInt bytes, UInt alignment) override final { return Splice<Byte>(); }
		virtual void Deallocate(Splice<Byte> memReg) override final { FE_CORE_ASSERT(!memReg.Elements, "NullAllocator should not be used for deallocation"); }

		template <UInt Size, UInt Alignment>
		Array<Byte, Size>* Allocate() { return nullptr; }

		template <UInt Alignment>
		Splice<Byte> Allocate(UInt bytes) { return Splice<Byte>(); }

		template <UInt Size>
		void Deallocate(Byte* ptr) { FE_CORE_ASSERT(!ptr, "NullAllocator should not be used for deallocation"); }
	};

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
			Splice<Byte> allocation = fe_allocator->Allocate(bytes, alignment);
			return allocation.Elements;
		};
		virtual void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) final override
		{
			Splice<Byte> mem_reg;
			mem_reg.Elements = (Byte*)p;
			mem_reg.Count = bytes;
			fe_allocator->Deallocate(mem_reg);
		};
		virtual bool do_is_equal(const std::pmr::memory_resource& other) const noexcept final override { return false; };
	};
}