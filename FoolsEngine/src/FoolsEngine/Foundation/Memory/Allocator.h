#pragma once

#include "Splice.h"

#include "DataTypes.h"
#include "FoolsEngine/Foundation/Debug/Asserts.h"
#include "FoolsEngine/Foundation/Utils/BitOperations.h"

#include <cstdlib>
#include <memory_resource>

namespace fe
{
	class Allocator
	{
	public:
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

	template <typename tnAlloc>
	class TypedAlloc
	{
	public:
		tnAlloc* m_Alloc = nullptr;

		TypedAlloc() = default;
		TypedAlloc(tnAlloc* alloc) : m_Alloc(alloc) { }

		template <typename T, UInt Count>
		Array<T, Count>* Allocate() { return (Array<T, Count>*)m_Alloc->Allocate<Count * sizeof(T), alignof(T)>(); }

		template <typename T>
		T* Allocate() { return (T*)m_Alloc->Allocate<sizeof(T), alignof(T)>(); }

		template <typename T>
		Splice<T> Allocate(UInt Count)
		{
			Splice<Byte> result = m_Alloc->Allocate<alignof(T)>(sizeof(T) * Count);
			result.Count = Count;
			return *(Splice<T>*) & result;
		}

		template <typename T>
		void Deallocate(T* ptr) { m_Alloc->Deallocate<sizeof(T)>((Byte*)ptr); }

		template <typename T, UInt Count>
		void Deallocate(Array<T, Count>* ptr) { m_Alloc->Deallocate<sizeof(T) * Count>((Byte*)ptr); }

		template <typename T>
		void Deallocate(Splice<T> splice)
		{
			splice.Count *= sizeof(T);
			m_Alloc->Deallocate(*(Splice<Byte>*) & splice);
		}
	};

	class NullAllocator final : public Allocator
	{
	public:
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

	class ArenaAllocator final : public Allocator
	{
	public:
		Splice<Byte> Buffer;
		Byte* Free;

		const Byte* End() const { return Buffer.Elements + Buffer.Count; }
		bool IsFull() const { return Free == End(); }

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

		void Clear() { Free = Buffer.Elements; }

		bool DoesOwn(Splice<Byte> memReg) const { return DoesOwn(memReg.Elements); }
		bool DoesOwn(Byte* ptr) const
		{
			if (ptr >= Buffer.Elements && ptr < Free)
				return true;
			return false;
		}
	};

	template <class Primary, class Fallback>
	class FallbackAllocator final : public Allocator
	{
	public:
		Primary m_Primary;
		Fallback m_Fallback;

		virtual Splice<Byte> Allocate(UInt bytes) final override
		{
			Splice<Byte> mem_reg = m_Primary.Allocate(bytes);
			if (!mem_reg.Elements)
				mem_reg = m_Fallback.Allocate(bytes);

			return mem_reg;
		}
		virtual Splice<Byte> Allocate(UInt bytes, UInt alignment) final override
		{
			Splice<Byte> mem_reg = m_Primary.Allocate(bytes, alignment);
			if (!mem_reg.Data)
				mem_reg = m_Fallback.Allocate(bytes, alignment);

			return mem_reg;
		}
		virtual void Deallocate(Splice<Byte> memReg) final override
		{
			if (m_Primary.DoesOwn(memReg))
				m_Primary.Deallocate(memReg);
			else
				m_Fallback.Deallocate(memReg);
		}

		template <UInt Size, UInt Alignment>
		Array<Byte, Size>* Allocate()
		{
			Array<Byte, Size>* result = m_Primary.Allocate<Size, Alignment>();
			if (!result)
				result = m_Fallback.Allocate<Size, Alignment>();

			return result;
		}

		template <UInt Alignment>
		Splice<Byte> Allocate(UInt bytes)
		{
			Splice<Byte> mem_reg = m_Primary.Allocate<Alignment>(bytes);
			if (!mem_reg.Elements)
				mem_reg = m_Fallback.Allocate<Alignment>(bytes);

			return mem_reg;
		}

		template <UInt Size>
		void Deallocate(Byte* ptr)
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

	class MallocAllocator final : public Allocator
	{
	public:
		virtual Splice<Byte> Allocate(UInt bytes) override final
		{
			Splice<Byte> result;
			result.Elements = (Byte*) operator new (bytes);
			result.Count = bytes;

			return result;
		}
		virtual Splice<Byte> Allocate(UInt bytes, UInt alignment) override final
		{
			FE_CORE_ASSERT(alignment > 8, "MallocAllocator does not align to anything more then 8");
			return Allocate(bytes); //we dont allign, because delete() takes alignment and we dont require it in interface
		}
		virtual void Deallocate(Splice<Byte> memReg) override final { operator delete (memReg.Elements); }

		template <UInt Size, UInt Alignment>
		Array<Byte, Size>* Allocate() { return Allocate(Size, Alignment); }

		template <UInt Alignment>
		Splice<Byte> Allocate(UInt bytes) { return Allocate(bytes, Alignment); }

		template <UInt Size>
		void Deallocate(Byte* ptr) { operator delete (ptr); }
	};

	template <class SmallAllocator, class BigAllocator, UInt threshold>
	class SegragatorAllocator final : public Allocator
	{
	public:
		SmallAllocator m_SmallAllocator;
		BigAllocator m_BigAllocator;
		
		virtual Splice<Byte> Allocate(UInt bytes) override final
		{
			if (bytes <= threshold)
				return m_SmallAllocator.Allocate(bytes);

			return m_BigAllocator.Allocate(bytes);
		}
		virtual Splice<Byte> Allocate(UInt bytes, UInt alignment) override final
		{
			if (bytes <= threshold)
				return m_SmallAllocator.Allocate(bytes, alignment);
			
			return m_BigAllocator.Allocate(bytes, alignment);
		}
		virtual void Deallocate(Splice<Byte> memReg) override final
		{
			if (memReg.Count <= threshold)
				m_SmallAllocator.Deallocate(memReg);
			else
				m_BigAllocator.Deallocate(memReg);
		}

		template <UInt Size, UInt Alignment = 8>
		Array<Byte, Size>* Allocate()
		{
			if constexpr (Size <= threshold)
				return m_SmallAllocator.Allocate<Size, Alignment>();
			else
				return m_BigAllocator.Allocate<Size, Alignment>();
		}

		template <UInt Alignment>
		Splice<Byte> Allocate(UInt bytes) { return Allocate(bytes, Alignment); }

		template <UInt Size>
		void Deallocate(Byte* ptr)
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

	template <UInt memRegSize, UInt memRegAlignment>
	class BitmappedPoolAllocator final : public Allocator
	{
	public:
		inline static constexpr UInt MemRegAlignment = memRegAlignment;
		inline static constexpr UInt MemRegSize = memRegSize;
		inline static constexpr UInt TotalSize = memRegSize * 64;
		inline Byte* RegionsEnd() { return m_Regions + TotalSize; }

		Byte* m_Regions = nullptr;
		//true is free
		U64 m_BitMapping = 0;

		virtual Splice<Byte> Allocate(UInt bytes) override final
		{
			Splice<Byte> result;

			FE_CORE_ASSERT(8 > memRegAlignment, "Overalignment in BitmappedPoolAllocator");
			FE_CORE_ASSERT(bytes > memRegSize, "This BitmappedPoolAllocator cannot accomdate allocation of this size!");

			if (!m_BitMapping)
				// out of free MemRegs
				return result;

			unsigned long outIndex;
			MSB64(&outIndex, m_BitMapping);
			U64 flag_mask = (U64)1 << outIndex;
			m_BitMapping &= ~flag_mask;

			result.Elements = m_Regions + MemRegSize * outIndex;
			result.Count = bytes;

			return result;
		}
		virtual Splice<Byte> Allocate(UInt bytes, UInt alignment) override final
		{
			Splice<Byte> result;

			FE_CORE_ASSERT(alignment > memRegAlignment, "Overalignment in BitmappedPoolAllocator");
			FE_CORE_ASSERT(bytes > memRegSize, "This BitmappedPoolAllocator cannot accomdate allocation of this size!");

			if (!m_BitMapping)
				// out of free MemRegs
				return result;

			unsigned long outIndex;
			MSB64(&outIndex, m_BitMapping);
			U64 flag_mask = (U64)1 << outIndex;
			m_BitMapping &= ~flag_mask;

			result.Elements = m_Regions + MemRegSize * outIndex;
			result.Count = bytes;

			return result;
		}
		virtual void Deallocate(Splice<Byte> memReg) override final
		{
			FE_CORE_ASSERT(DoesOwn(memReg), "This BitmappedPoolAllocator does not own this MemReg");

			UInt index = (memReg.Elements - m_Regions) / memRegSize;
			U64 flag_mask = (U64)1 << (63 - index);
			m_BitMapping &= flag_mask;
		}

		template <UInt Size, UInt Alignment>
		Array<Byte, Size>* Allocate()
		{
			static_assert(Alignment > memRegAlignment, "Overalignment in BitmappedPoolAllocator");
			static_assert(Size > memRegSize, "This BitmappedPoolAllocator cannot accomdate allocation of this size!");
			return (Array<Byte, Size>*)Allocate(Size, Alignment).Elements;
		}

		template <UInt Alignment>
		Splice<Byte> Allocate(UInt size)
		{
			static_assert(Alignment > memRegAlignment, "Overalignment in BitmappedPoolAllocator");
			return Allocate(size, Alignment);
		}

		template <UInt Size>
		void Deallocate(Byte* ptr)
		{
			static_assert(Size > memRegSize, "Oversized deallocation in BitmappedPoolAllocator!");
			FE_CORE_ASSERT(DoesOwn(ptr), "This BitmappedPoolAllocator does not own this MemReg");

			UInt index = (ptr - m_Regions) / memRegSize;
			U64 flag_mask = (U64)1 << (63 - index);
			m_BitMapping &= flag_mask;
		}

		bool DoesOwn(Splice<Byte> memReg) { return DoesOwn(memReg.Elements); }
		bool DoesOwn(Byte* ptr)
		{
			bool lower_bound = ptr >= m_Regions;
			bool  uper_bound = ptr < RegionsEnd();
			if (ptr && lower_bound && uper_bound)
				return true;
			return false;
		}
	};

	class MonotonicAllocator final : public Allocator
	{
	public:
		std::pmr::monotonic_buffer_resource MBR;

		virtual Splice<Byte> Allocate(UInt bytes) final override
		{
			Splice<Byte> mem_reg;
			mem_reg.Elements = (Byte*)MBR.allocate(bytes);
			mem_reg.Count = bytes;
			return mem_reg;
		}
		virtual Splice<Byte> Allocate(UInt bytes, UInt alignment) final override
		{
			Splice<Byte> mem_reg;
			mem_reg.Elements = (Byte*)MBR.allocate(bytes, alignment);
			mem_reg.Count = bytes;
			return mem_reg;
		}
		virtual void Deallocate(Splice<Byte> memReg) final override { FE_LOG_CORE_WARN("Deallocation in MonotonicAllocator"); }

		template <UInt Size, UInt Alignment>
		Array<Byte, Size>* Allocate() { return (Array<Byte, Size>*)MBR.allocate(Size, Alignment); }
		
		template <UInt Alignment>
		Splice<Byte> Allocate(UInt size) { return Allocate(size, Alignment); }
		
		template <UInt Size>
		void Deallocate(Byte* ptr) { FE_LOG_CORE_WARN("Deallocation in MonotonicAllocator"); }

		void Clear() { MBR.release(); }
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