#pragma once

#include "MemReg.h"

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
		virtual MemReg Allocate(UInt bytes, UInt alignment = 8) = 0;
		virtual void Deallocate(MemReg& memReg) = 0;

		template <UInt Size, UInt Alignment = 8>
		MemReg Allocate() { return Allocate(Size, Alignment); }

		template <typename T, UInt Count = 1>
		MemReg Allocate() { return Allocate<sizeof(T) * Count, alignof(T)>(); }

		template <typename T>
		MemReg Allocate(UInt count) { return Allocate(sizeof(T) * count, alignof(T)); }

		template <UInt Size>
		void Deallocate(Byte* ptr)
		{
			MemReg memReg;
			memReg.Data = ptr;
			memReg.Size = Size;
			return Deallocate(memReg);
		}

		template <typename T, UInt Count = 1>
		void Deallocate(T* ptr) { return Deallocate<sizeof(T) * Count>(ptr); }

		template <typename T>
		void Deallocate(T* ptr, UInt count)
		{
			MemReg memReg;
			memReg.Data = ptr;
			memReg.Size = sizeof(T) * count;
			return Deallocate(memReg);
		}
	};

	class NullAllocator final : public Allocator
	{
	public:
		virtual MemReg Allocate(UInt bytes, UInt alignment = 8) override final { return MemReg(); }
		virtual void Deallocate(MemReg& memReg) override final { FE_CORE_ASSERT(!memReg.Data, "NullAllocator should not be used for deallocation"); }

		template <UInt Size, UInt Alignment = 8>
		MemReg Allocate() { return MemReg(); }

		template <typename T, UInt Count = 1>
		MemReg Allocate() { return MemReg(); }
		
		template <typename T>
		MemReg Allocate(UInt count) { return MemReg(); }

		template <UInt Size>
		void Deallocate(Byte* ptr) { FE_CORE_ASSERT(!ptr, "NullAllocator should not be used for deallocation"); }

		template <typename T, UInt Count = 1>
		void Deallocate(T* ptr) { FE_CORE_ASSERT(!ptr, "NullAllocator should not be used for deallocation"); }

		template <typename T>
		void Deallocate(T* ptr, UInt count) { FE_CORE_ASSERT(!ptr, "NullAllocator should not be used for deallocation"); }
	};

	class ArenaAllocator final : public Allocator
	{
	public:
		MemRegFiller m_MemRegFiller;

		virtual MemReg Allocate(UInt bytes, UInt alignment = 8) override final
		{
			MemReg memReg;
			memReg.Size = bytes;
			memReg.Data = (Byte*)(((UInt)m_MemRegFiller.Free + (alignment - 1)) & ~(alignment - 1));

			if (memReg.Data + memReg.Size > m_MemRegFiller.Data + m_MemRegFiller.Size)
			{
				FE_CORE_ASSERT(false, "ArenaAllocator overflow!");
				memReg.Data = nullptr;
			}
			return memReg;
		}
		virtual void Deallocate(MemReg& memReg) override final { FE_CORE_ASSERT(DoesOwn(memReg), "ArenaAllocator does not own this memory"); memReg.Data = nullptr; }

		template <UInt Size, UInt Alignment = 8>
		MemReg Allocate()
		{
			MemReg memReg;
			memReg.Size = Size;
			memReg.Data = (Byte*)(((UInt)m_MemRegFiller.Free + (Alignment - 1)) & ~(Alignment - 1));

			if (memReg.Data + memReg.Size > m_MemRegFiller.Data + m_MemRegFiller.Size)
			{
				FE_CORE_ASSERT(false, "ArenaAllocator overflow!");
				memReg.Data = nullptr;
			}
			return memReg;
		}

		template <typename T, UInt Count>
		MemReg Allocate() { return Allocate<sizeof(T) * Count, alignof(T)>(); }

		template <typename T>
		MemReg Allocate(UInt count) { return Allocate<sizeof(T) * count, alignof(T)>(); }

		template <UInt Size, UInt Alignment>
		void Deallocate(Byte* ptr)
		{
			MemReg memReg;
			memReg.Data = ptr;
			memReg.Size = Size;
			return Deallocate(memReg);
		}

		template <typename T, UInt Count>
		void Deallocate(T* ptr) { return Deallocate<sizeof(T) * Count, alignof(T)>(ptr); }

		template <typename T>
		void Deallocate(T* ptr, UInt count) { return Deallocate<sizeof(T) * count, alignof(T)>(ptr); }

		void Clear() { m_MemRegFiller.Free = m_MemRegFiller.Data; }

		bool DoesOwn(const MemReg& memReg)
		{
			if (memReg.Data >= m_MemRegFiller.Data && memReg.Data < m_MemRegFiller.Free)
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

		virtual MemReg Allocate(UInt bytes, UInt alignment = 8) final override
		{
			MemReg mem_reg = m_Primary.Allocate(bytes, alignment);
			if (!mem_reg.Data)
				mem_reg = m_Fallback.Allocate(bytes, alignment);

			return mem_reg;
		}

		virtual void Deallocate(MemReg& memReg) final override
		{
			if (m_Primary.DoesOwn(memReg))
				m_Primary.Deallocate(memReg);
			else
				m_Fallback.Deallocate(memReg);
		}

		template <UInt Size, UInt Alignment = 8>
		MemReg Allocate() { return Allocate(Size, Alignment); }

		template <typename T, UInt Count = 1>
		MemReg Allocate() { return Allocate<sizeof(T) * Count, alignof(T)>(); }

		template <typename T>
		MemReg Allocate(UInt count) { return Allocate(sizeof(T) * count, alignof(T)); }

		template <UInt Size>
		void Deallocate(Byte* ptr)
		{
			MemReg memReg;
			memReg.Data = ptr;
			memReg.Size = Size;
			return Deallocate(memReg);
		}

		template <typename T, UInt Count = 1>
		void Deallocate(T* ptr) { return Deallocate<sizeof(T) * Count>(ptr); }

		template <typename T>
		void Deallocate(T* ptr, UInt count)
		{
			MemReg memReg;
			memReg.Data = ptr;
			memReg.Size = sizeof(T) * count;
			return Deallocate(memReg);
		}

		bool DoesOwn(const MemReg& memReg) { return m_Primary.DoesOwn(memReg) || m_Fallback.DoesOwn(memReg); }
	};

	class MallocAllocator final : public Allocator
	{
	public:
		virtual MemReg Allocate(UInt bytes, UInt alignment = 8) override final
		{
			FE_CORE_ASSERT(alignment > 8, "MallocAllocator does not align to anything more then 8");
			MemReg result;
			result.Data = (Byte*) operator new (bytes);
			result.Size = bytes;

			return result;
		}

		virtual void Deallocate(MemReg& memReg) override final
		{
			operator delete (memReg.Data);
			memReg.Data = nullptr;
		}

		template <UInt Size, UInt Alignment = 8>
		MemReg Allocate() { return Allocate(Size, Alignment); }

		template <typename T, UInt Count = 1>
		MemReg Allocate() { return Allocate<sizeof(T) * Count, alignof(T)>(); }

		template <typename T>
		MemReg Allocate(UInt count) { return Allocate(sizeof(T) * count, alignof(T)); }

		template <UInt Size>
		void Deallocate(Byte* ptr)
		{
			MemReg memReg;
			memReg.Data = ptr;
			memReg.Size = Size;
			return Deallocate(memReg);
		}

		template <typename T, UInt Count = 1>
		void Deallocate(T* ptr) { return Deallocate<sizeof(T) * Count>(ptr); }

		template <typename T>
		void Deallocate(T* ptr, UInt count)
		{
			MemReg memReg;
			memReg.Data = ptr;
			memReg.Size = sizeof(T) * count;
			return Deallocate(memReg);
		}
	};

	template <class SmallAllocator, class BigAllocator, UInt threshold>
	class SegragatorAllocator final : public Allocator
	{
	public:
		SmallAllocator m_SmallAllocator;
		BigAllocator m_BigAllocator;

		virtual MemReg Allocate(UInt bytes, UInt alignment) override final
		{
			if (bytes <= threshold)
				return m_SmallAllocator.Allocate(bytes, alignment);
			
			return m_BigAllocator.Allocate(bytes, alignment);
		}

		virtual void Deallocate(MemReg& memReg) override final
		{
			if (memReg.Size <= threshold)
				m_SmallAllocator.Deallocate(memReg);
			else
				m_BigAllocator.Deallocate(memReg);
		}

		template <UInt Size, UInt Alignment = 8>
		MemReg Allocate()
		{
			if constexpr (Size <= threshold)
				return m_SmallAllocator.Allocate<Size, Alignment>();
			else
				return m_BigAllocator.Allocate<Size, Alignment>();
		}

		template <typename T, UInt Count = 1>
		MemReg Allocate()
		{
			if constexpr (sizeof(T) * Count <= threshold)
				return m_SmallAllocator.Allocate<sizeof(T) * Count, alignof(T)>();
			else
				return m_BigAllocator.Allocate<sizeof(T) * Count, alignof(T)>();
		}

		template <typename T>
		MemReg Allocate(UInt count) { return Allocate(sizeof(T) * count, alignof(T)); }

		template <UInt Size>
		void Deallocate(Byte* ptr)
		{
			if constexpr (Size <= threshold)
				return m_SmallAllocator.Deallocate<Size>(ptr);
			else
				return m_BigAllocator.Deallocate<Size>(ptr);
		}

		template <typename T, UInt Count = 1>
		void Deallocate(T* ptr) { return Deallocate<sizeof(T) * Count>(ptr); }

		template <typename T>
		void Deallocate(T* ptr, UInt count)
		{
			MemReg memReg;
			memReg.Data = ptr;
			memReg.Size = sizeof(T) * count;
			return Deallocate(memReg);
		}

		bool DoesOwn(const MemReg& memReg)
		{
			if (memReg.Size <= threshold)
				return m_SmallAllocator.DoesOwn(memReg);
			else
				return m_BigAllocator.DoesOwn(memReg);
		}

		template <UInt Size>
		bool DoesOwn(const MemReg& memReg)
		{
			if constexpr (Size <= threshold)
				return m_SmallAllocator.DoesOwn(memReg);
			else
				return m_BigAllocator.DoesOwn(memReg);
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

		virtual MemReg Allocate(UInt bytes, UInt alignment) override final
		{
			MemReg result;

			FE_CORE_ASSERT(alignment > memRegAlignment, "Overalignment in BitmappedPoolAllocator");
			FE_CORE_ASSERT(bytes > memRegSize, "This BitmappedPoolAllocator cannot accomdate allocation of this size!");

			if (!m_BitMapping)
				// out of free MemRegs
				return result;

			unsigned long outIndex;
			MSB64(&outIndex, m_BitMapping);
			U64 flag_mask = (U64)1 << outIndex;
			m_BitMapping &= ~flag_mask;

			result.Data = m_Regions + MemRegSize * outIndex;
			result.Size = bytes;

			return result;
		}

		virtual void Deallocate(MemReg& memReg) override final
		{
			FE_CORE_ASSERT(DoesOwn(memReg), "This BitmappedPoolAllocator does not own this MemReg");

			UInt index = (memReg.Data - m_Regions) / memRegSize;
			U64 flag_mask = (U64)1 << (63 - index);
			m_BitMapping &= flag_mask;
			memReg.Data = nullptr;
		}

		template <UInt Size, UInt Alignment = 8>
		MemReg Allocate()
		{
			static_assert(Alignment > memRegAlignment, "Overalignment in BitmappedPoolAllocator");
			static_assert(Size > memRegSize, "This BitmappedPoolAllocator cannot accomdate allocation of this size!");
			return Allocate(Size, Alignment);
		}

		template <typename T, UInt Count = 1>
		MemReg Allocate()
		{
			static_assert(alignof(T) > memRegAlignment, "Overalignment in BitmappedPoolAllocator");
			static_assert(sizeof(T) * Count > memRegSize, "This BitmappedPoolAllocator cannot accomdate allocation of this size!");
			return Allocate<sizeof(T) * Count, alignof(T)>();
		}

		template <typename T>
		MemReg Allocate(UInt count)
		{
			static_assert(alignof(T) > memRegAlignment, "Overalignment in BitmappedPoolAllocator");

			FE_CORE_ASSERT(sizeof(T) * count > memRegSize, "This BitmappedPoolAllocator cannot accomdate allocation of this size!");

			return Allocate(sizeof(T) * count, alignof(T));
		}

		template <UInt Size>
		void Deallocate(Byte* ptr)
		{
			static_assert(Size > memRegSize, "Oversized deallocation in BitmappedPoolAllocator!");

			MemReg memReg;
			memReg.Data = ptr;
			memReg.Size = Size;

			FE_CORE_ASSERT(DoesOwn(memReg), "This BitmappedPoolAllocator does not own this MemReg");

			return Deallocate(memReg);
		}

		template <typename T, UInt Count = 1>
		void Deallocate(T* ptr)
		{
			static_assert(sizeof(T) * Count > memRegSize, "Oversized deallocation in BitmappedPoolAllocator!");
			return Deallocate<sizeof(T) * Count>(ptr);
		}

		template <typename T>
		void Deallocate(T* ptr, UInt count)
		{
			FE_CORE_ASSERT(sizeof(T) * count > memRegSize, "Oversized deallocation in BitmappedPoolAllocator!");

			MemReg memReg;
			memReg.Data = ptr;
			memReg.Size = sizeof(T) * count;

			FE_CORE_ASSERT(DoesOwn(memReg), "This BitmappedPoolAllocator does not own this MemReg");

			return Deallocate(memReg);
		}

		bool DoesOwn(const MemReg& memReg)
		{
			bool lower_bound = memReg.Data >= m_Regions;
			bool  uper_bound = memReg.Data < RegionsEnd();
			if (memReg.Data && lower_bound && uper_bound)
				return true;
			return false;
		}
	};

	class MonotonicAllocator final : public Allocator
	{
	public:
		std::pmr::monotonic_buffer_resource MBR;

		virtual MemReg Allocate(UInt bytes, UInt alignment = 8) final override
		{
			FE_CORE_ASSERT(alignment <= 8, "OverAlignment in MonotonicAllocator");
			void* allocation = MBR.allocate(bytes);
			MemReg memReg;
			memReg.Data = (Byte*)allocation;
			memReg.Size = bytes;
			return memReg;
		}
		virtual void Deallocate(MemReg& memReg) final override { MBR.deallocate(memReg.Data, memReg.Size); memReg.Data = nullptr; }

		template <UInt Size, UInt Alignment = 8>
		MemReg Allocate() { return Allocate(Size, Alignment); }

		template <typename T, UInt Count = 1>
		MemReg Allocate() { return Allocate<sizeof(T) * Count, alignof(T)>(); }

		template <typename T>
		MemReg Allocate(UInt count) { return Allocate(sizeof(T) * count, alignof(T)); }

		template <UInt Size>
		void Deallocate(Byte* ptr)
		{
			MemReg memReg;
			memReg.Data = ptr;
			memReg.Size = Size;
			return Deallocate(memReg);
		}

		template <typename T, UInt Count = 1>
		void Deallocate(T* ptr) { return Deallocate<sizeof(T) * Count>(ptr); }

		template <typename T>
		void Deallocate(T* ptr, UInt count)
		{
			MemReg memReg;
			memReg.Data = ptr;
			memReg.Size = sizeof(T) * count;
			return Deallocate(memReg);
		}

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
			MemReg allocation = fe_allocator->Allocate(bytes, alignment);
			return allocation.Data;
		};
		virtual void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) final override
		{
			MemReg deallocation;
			deallocation.Data = (Byte*)p;
			deallocation.Size = bytes;
			fe_allocator->Deallocate(deallocation);
		};
		virtual bool do_is_equal(const std::pmr::memory_resource& other) const noexcept final override { return false; };
	};
}