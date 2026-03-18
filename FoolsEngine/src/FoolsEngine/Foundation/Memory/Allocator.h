#pragma once

#include "MemReg.h"

#include "DataTypes.h"
#include "FoolsEngine/Foundation/Debug/Asserts.h"
#include "FoolsEngine/Foundation/Utils/BitOperations.h"

#include <cstdlib>

namespace fe
{
	class Allocator
	{
	public:
		template <typename tn>
		MemReg Allocate(UInt count = 1) { return Allocate(sizeof(tn), alignof(tn)); }

		virtual MemReg Allocate(UInt bytes, UInt alignment) = 0;
		virtual void Allocate(MemReg& memReg, UInt alignment) = 0;

		virtual void Deallocate(MemReg& memReg) = 0;
	};

	class NullAllocator final : public Allocator
	{
	public:
		virtual MemReg Allocate(UInt bytes, UInt alignment) override final { return MemReg(); }
		virtual void Allocate(MemReg& memReg, UInt alignment) override final { memReg.Data = nullptr; }

		virtual void Deallocate(MemReg& memReg) override final { FE_CORE_ASSERT(!memReg.Data, "NullAllocator should not be used for deallocation"); }
	};

	class ArenaAllocator final : public Allocator
	{
	public:
		MemRegFiller m_MemRegFiller;

		template <typename tn>
		MemReg Allocate(UInt count = 1) { return Allocate(sizeof(tn), alignof(tn)); }

		virtual MemReg Allocate(UInt bytes, UInt alignment) override final { return MemReg(); }
		virtual void Allocate(MemReg& memReg, UInt alignment) override final { memReg.Data = nullptr; }

		virtual void Deallocate(MemReg& memReg) override final
		{
			if (memReg.Data == m_MemRegFiller.Data && memReg.Size == m_MemRegFiller.Size)
			{
				Clear();
				FE_LOG_CORE_WARN("Clearing arena by deallocating!");
				return;
			}
			FE_CORE_ASSERT(false, "ArenaAllocator does not deallocate individually");
		};

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

		template <typename tn>
		MemReg Allocate(UInt count = 1) { return Allocate(sizeof(tn), alignof(tn)); }

		virtual MemReg Allocate(UInt bytes, UInt alignment) final override
		{
			MemReg mem_reg = m_Primary.Allocate(bytes, alignment);
			if (!mem_reg.Data)
				mem_reg = m_Fallback.Allocate(bytes, alignment);

			return mem_reg;
		}

		virtual void Allocate(MemReg& memReg, UInt alignment) final override
		{
			m_Primary.Allocate(memReg, alignment);
			if (!memReg.Data)
				m_Fallback.Allocate(memReg, alignment);
		}

		virtual void Deallocate(MemReg& memReg) final override
		{
			if (m_Primary.DoesOwn(memReg))
				m_Primary.Deallocate(memReg);
			else
				m_Fallback.Deallocate(memReg);
		}

		bool DoesOwn(const MemReg& memReg) { return m_Primary.DoesOwn(memReg) || m_Fallback.DoesOwn(memReg); }
	};

	class MallocAllocator final : public Allocator
	{
	public:
		template <typename tn>
		MemReg Allocate(UInt count = 1) { return Allocate(sizeof(tn), alignof(tn)); }

		virtual MemReg Allocate(UInt bytes, UInt alignment) override final
		{
			MemReg result;
			result.Data = (Byte*) operator new (bytes, std::align_val_t(alignment));
			result.Size = bytes;
			
			return result;
		}
		virtual void Allocate(MemReg& memReg, UInt alignment) override final { memReg.Data = (Byte*) operator new (memReg.Size, std::align_val_t(alignment)); }

		virtual void Deallocate(MemReg& memReg) override final
		{
			operator delete (memReg.Data);
			memReg.Data = nullptr;
		};
	};

	template <class SmallAllocator, class BigAllocator, UInt threshold>
	class SegragatorAllocator final : public Allocator
	{
	public:
		SmallAllocator m_SmallAllocator;
		BigAllocator m_BigAllocator;

		template <typename tn>
		MemReg Allocate(UInt count = 1) { return Allocate(sizeof(tn), alignof(tn)); }

		virtual MemReg Allocate(UInt bytes, UInt alignment) override final
		{
			if (bytes <= threshold)
				return m_SmallAllocator.Allocate(bytes, alignment);
			
			return m_BigAllocator.Allocate(bytes, alignment);
		}

		virtual void Allocate(MemReg& memReg, UInt alignment) override final
		{
			if (memReg.Size <= threshold)
				m_SmallAllocator.Allocate(memReg, alignment);
			else
				m_BigAllocator.Allocate(memReg, alignment);
		}

		virtual void Deallocate(MemReg& memReg) override final
		{
			if (memReg.Size <= threshold)
				m_SmallAllocator.Deallocate(memReg);
			else
				m_BigAllocator.Deallocate(memReg);
		};

		bool DoesOwn(const MemReg& memReg)
		{
			if (memReg.Size <= threshold)
				return m_SmallAllocator.DoesOwn(memReg);
			else
				return m_BigAllocator.DoesOwn(memReg);
		}
	};

	template <UInt memRegSize, UInt memRegAlignment = 8>
	class BitmappedPoolAllocator final : public Allocator
	{
		static_assert(memRegSize % memRegAlignment == 0, "Non matching size-alignment in BitmappedPoolAllocator");
	public:
		inline static constexpr UInt MemRegAlignment = memRegAlignment;
		inline static constexpr UInt MemRegSize = memRegSize;
		inline static constexpr UInt TotalSize = memRegSize * 64;
		inline Byte* RegionsEnd() { return m_Regions + TotalSize; }

		Byte* m_Regions = nullptr;
		//true is free
		U64 m_BitMapping = 0;

		template <typename tn>
		MemReg Allocate(UInt count = 1) { return Allocate(sizeof(tn), alignof(tn)); }

		virtual MemReg Allocate(UInt bytes, UInt alignment) override final
		{
			MemReg result;

			if (alignment > memRegAlignment)
			{
				FE_CORE_ASSERT(false, "Overalignment in BitmappedPoolAllocator");
				return result;
			}
			
			if (bytes > memRegSize)
			{
				FE_CORE_ASSERT(false, "This BitmappedPoolAllocator cannot accomdate allocation of this size!");
				return result;
			}

			if (!m_BitMapping)
				// out of free MemRegs
				return result;

			unsigned long outIndex;
			MSB64(&outIndex, m_BitMapping);
			U64 flag_mask = (U64)1 << (63 - outIndex);
			m_BitMapping &= ~flag_mask;

			result.Data = m_Regions + MemRegSize * outIndex;
			result.Size = bytes;

			return result;
		}

		virtual void Allocate(MemReg& memReg, UInt alignment) override final
		{
			if (alignment > memRegAlignment)
			{
				FE_CORE_ASSERT(false, "Overalignment in BitmappedPoolAllocator");
				memReg.Data = nullptr;
				return;
			}

			if (memReg.Size > memRegSize)
			{
				FE_CORE_ASSERT(false, "This BitmappedPoolAllocator cannot accomdate allocation of this size!");
				memReg.Data = nullptr;
				return;
			}

			if (!m_BitMapping)
			{
				// out of free MemRegs
				memReg.Data = nullptr;
				return;
			}

			unsigned long out_index;
			MSB64(&out_index, m_BitMapping);
			U64 flag_mask = (U64)1 << (63 - out_index);
			m_BitMapping &= ~flag_mask;

			memReg.Data = m_Regions + memRegSize * out_index;

			return;
		}

		virtual void Deallocate(MemReg& memReg) override final
		{
			UInt index = (memReg.Data - m_Regions) / memRegSize;
			U64 flag_mask = (U64)1 << (63 - index);
			m_BitMapping &= flag_mask;
			memReg.Data = nullptr;
		};

		bool DoesOwn(const MemReg& memReg)
		{
			bool lower_bound = memReg.Data >= m_Regions;
			bool  uper_bound = memReg.Data < RegionsEnd();
			if (memReg.Data && lower_bound && uper_bound)
				return true;
			return false;
		}
	};
}