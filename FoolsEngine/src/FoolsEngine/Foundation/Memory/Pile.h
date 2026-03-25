#pragma once

#include "DataTypes.h"
#include "Splice.h"
#include "Allocator.h"

namespace fe
{
	class Pile final : public Allocator
	{
	public:
		Pile() :
			m_Begin(s_Free),
			m_End(s_Free),
			m_FlagMask(U64(1) << (U64(63) - s_Count)),
			m_FrontFlagsAntiMask(U64(-1) << (U64(63) - s_Count))
		{
			s_RollbackFlags |= m_FlagMask;
			++s_Count;

			StackCheck();
		};
		Pile(Pile&&) = delete;
		Pile(const Pile&) = delete;
		Pile& operator=(Pile&& other) = delete;
		Pile& operator=(const Pile& other) = delete;

		virtual MemReg Allocate(UInt bytes, UInt alignment) final override
		{
			MemReg result;

			const bool at_front = m_End == s_Free;
			const bool rollback_flag = s_RollbackFlags & m_FlagMask;
			const bool reset_begin = !at_front && !rollback_flag;

			m_Begin = (Byte*)(((UInt)s_Free * reset_begin) + ((UInt)m_Begin * !reset_begin));

			s_RollbackFlags &= m_FrontFlagsAntiMask;
			s_RollbackFlags |= m_FlagMask;

			result.Data = (Byte*)(((UInt)s_Free + (alignment - 1)) & ~(alignment - 1));
			result.Size = bytes;

			s_Free = result.Data + bytes;
			m_End = s_Free;

			FE_CORE_ASSERT(s_Free < (s_Buffer + s_BufferSize), "Out of memory in scrachpad!");

#ifdef FE_INTERNAL_BUILD
			bool new_max = s_Free > s_MaxFree;
			s_MaxFree = (Byte*)(((UInt)s_Free * new_max) + ((UInt)s_MaxFree * !new_max));
#endif // FE_INTERNAL_BUILD

			return result;
		}

		virtual void Deallocate(MemReg& memReg) final override
		{
			bool is_at_front = s_Free == memReg.Data + memReg.Size;
			s_Free = (Byte*)((UInt)memReg.Data * is_at_front + (UInt)s_Free * !is_at_front);

			memReg.Data = nullptr;
		}

		template <UInt Size, UInt Alignment = 8>
		MemReg Allocate()
		{
			MemReg result;

			const bool at_front = m_End == s_Free;
			const bool rollback_flag = s_RollbackFlags & m_FlagMask;
			const bool reset_begin = !at_front && !rollback_flag;

			m_Begin = (Byte*)(((UInt)s_Free * reset_begin) + ((UInt)m_Begin * !reset_begin));

			s_RollbackFlags &= m_FrontFlagsAntiMask;
			s_RollbackFlags |= m_FlagMask;

			result.Data = (Byte*)(((UInt)s_Free + (Alignment - 1)) & ~(Alignment - 1));
			result.Size = Size;

			s_Free = result.Data + Size;
			m_End = s_Free;

			FE_CORE_ASSERT(s_Free < (s_Buffer + s_BufferSize), "Out of memory in scrachpad!");

#ifdef FE_INTERNAL_BUILD
			bool new_max = s_Free > s_MaxFree;
			s_MaxFree = (Byte*)(((UInt)s_Free * new_max) + ((UInt)s_MaxFree * !new_max));
#endif // FE_INTERNAL_BUILD

			return result;
		}

		template <typename T, UInt Count = 1>
		MemReg Allocate() { return Allocate<sizeof(T) * Count, alignof(T)>(); }

		template <typename T>
		MemReg Allocate(UInt count)
		{
			MemReg result;

			const bool at_front = m_End == s_Free;
			const bool rollback_flag = s_RollbackFlags & m_FlagMask;
			const bool reset_begin = !at_front && !rollback_flag;

			m_Begin = (Byte*)(((UInt)s_Free * reset_begin) + ((UInt)m_Begin * !reset_begin));

			s_RollbackFlags &= m_FrontFlagsAntiMask;
			s_RollbackFlags |= m_FlagMask;

			result.Data = (Byte*)(((UInt)s_Free + (alignof(T) - 1)) & ~(alignof(T) - 1));
			result.Size = sizeof(T) * count;

			s_Free = result.Data + sizeof(T) * count;
			m_End = s_Free;

			FE_CORE_ASSERT(s_Free < (s_Buffer + s_BufferSize), "Out of memory in scrachpad!");

#ifdef FE_INTERNAL_BUILD
			bool new_max = s_Free > s_MaxFree;
			s_MaxFree = (Byte*)(((UInt)s_Free * new_max) + ((UInt)s_MaxFree * !new_max));
#endif // FE_INTERNAL_BUILD

			return result;
		}

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

		~Pile()
		{
			--s_Count;
			bool rollback_flag = s_RollbackFlags & m_FlagMask;
			s_Free = (Byte*)((UInt)m_Begin * rollback_flag + (UInt)s_Free * !rollback_flag);
		}

		void Clear()
		{
			bool rollback_flag = s_RollbackFlags & m_FlagMask;
			s_Free = (Byte*)((UInt)m_Begin * rollback_flag + (UInt)s_Free * !rollback_flag);

			s_RollbackFlags |= m_FlagMask;
			m_Begin = s_Free;
			m_End = s_Free;
		}

#ifdef FE_INTERNAL_BUILD
		static Byte* s_MaxFree;
#endif // FE_INTERNAL_BUILD
	private:
		static constexpr UInt s_BufferSize = 20 * 1024; // 20KB
		static Byte s_Buffer[s_BufferSize];
		static Byte* s_Free;
		static U64 s_RollbackFlags;
		static UInt s_Count;
		// we don't use std::bitset, because it handles any number of bits
		// solution for specific size that fits in one non-array type is slightly faster
		// this is very hot code

		Byte* m_Begin;
		Byte* m_End;
		const U64 m_FlagMask;
		const U64 m_FrontFlagsAntiMask;

		void StackCheck()
		{
#ifdef FE_INTERNAL_BUILD
			char dummy;
			ptrdiff_t displacement = &dummy - reinterpret_cast<char*>(this);
			FE_CORE_ASSERT(-10000 < displacement && displacement < 10000, "Don't put this on the heap!");
#endif // FE_INTERNAL_BUILD
		}
	};
}