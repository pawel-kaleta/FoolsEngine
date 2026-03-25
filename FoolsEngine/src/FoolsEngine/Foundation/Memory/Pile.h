#pragma once

#include "DataTypes.h"
#include "Splice.h"
#include "Allocators/Allocator.h"

namespace fe
{
	class PileBase : public Allocator
	{
	public:
		PileBase(PileBase&&) = delete;
		PileBase(const PileBase&) = delete;
		PileBase& operator=(PileBase&& other) = delete;
		PileBase& operator=(const PileBase& other) = delete;

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

	protected:
		virtual Splice<Byte> Allocate(UInt bytes) final override
		{
			Splice<Byte> result;

			const bool at_front = m_End == s_Free;
			const bool rollback_flag = s_RollbackFlags & m_FlagMask;
			const bool reset_begin = !at_front && !rollback_flag;

			m_Begin = (Byte*)(((UInt)s_Free * reset_begin) + ((UInt)m_Begin * !reset_begin));

			s_RollbackFlags &= m_FrontFlagsAntiMask;
			s_RollbackFlags |= m_FlagMask;

			result.Elements = (Byte*)(((UInt)s_Free + (7)) & ~(7));
			result.Count = bytes;

			s_Free = result.Elements + bytes;
			m_End = s_Free;

			FE_CORE_ASSERT(s_Free < (s_Buffer + s_BufferSize), "Out of memory in scrachpad!");

#ifdef FE_INTERNAL_BUILD
			bool new_max = s_Free > s_MaxFree;
			s_MaxFree = (Byte*)(((UInt)s_Free * new_max) + ((UInt)s_MaxFree * !new_max));
#endif // FE_INTERNAL_BUILD

			return result;
		}
		virtual Splice<Byte> Allocate(UInt bytes, UInt alignment) final override
		{
			Splice<Byte> result;

			const bool at_front = m_End == s_Free;
			const bool rollback_flag = s_RollbackFlags & m_FlagMask;
			const bool reset_begin = !at_front && !rollback_flag;

			m_Begin = (Byte*)(((UInt)s_Free * reset_begin) + ((UInt)m_Begin * !reset_begin));

			s_RollbackFlags &= m_FrontFlagsAntiMask;
			s_RollbackFlags |= m_FlagMask;

			result.Elements = (Byte*)(((UInt)s_Free + (alignment - 1)) & ~(alignment - 1));
			result.Count = bytes;

			s_Free = result.Elements + bytes;
			m_End = s_Free;

			FE_CORE_ASSERT(s_Free < (s_Buffer + s_BufferSize), "Out of memory in scrachpad!");

#ifdef FE_INTERNAL_BUILD
			bool new_max = s_Free > s_MaxFree;
			s_MaxFree = (Byte*)(((UInt)s_Free * new_max) + ((UInt)s_MaxFree * !new_max));
#endif // FE_INTERNAL_BUILD

			return result;
		}
		virtual void Deallocate(Splice<Byte> memReg) final override
		{
			bool is_at_front = s_Free == memReg.Elements + memReg.Count;
			s_Free = (Byte*)((UInt)memReg.Elements * is_at_front + (UInt)s_Free * !is_at_front);
		}

		template <UInt Size, UInt Alignment>
		Splice<Byte> Allocate()
		{
			Splice<Byte> result;

			const bool at_front = m_End == s_Free;
			const bool rollback_flag = s_RollbackFlags & m_FlagMask;
			const bool reset_begin = !at_front && !rollback_flag;

			m_Begin = (Byte*)(((UInt)s_Free * reset_begin) + ((UInt)m_Begin * !reset_begin));

			s_RollbackFlags &= m_FrontFlagsAntiMask;
			s_RollbackFlags |= m_FlagMask;

			result.Elements = (Byte*)(((UInt)s_Free + (Alignment - 1)) & ~(Alignment - 1));
			result.Count = Size;

			s_Free = result.Elements + Size;
			m_End = s_Free;

			FE_CORE_ASSERT(s_Free < (s_Buffer + s_BufferSize), "Out of memory in scrachpad!");

#ifdef FE_INTERNAL_BUILD
			bool new_max = s_Free > s_MaxFree;
			s_MaxFree = (Byte*)(((UInt)s_Free * new_max) + ((UInt)s_MaxFree * !new_max));
#endif // FE_INTERNAL_BUILD

			return result;
		}

		template <UInt Alignment>
		Splice<Byte> Allocate(UInt size)
		{
			Splice<Byte> result;

			const bool at_front = m_End == s_Free;
			const bool rollback_flag = s_RollbackFlags & m_FlagMask;
			const bool reset_begin = !at_front && !rollback_flag;

			m_Begin = (Byte*)(((UInt)s_Free * reset_begin) + ((UInt)m_Begin * !reset_begin));

			s_RollbackFlags &= m_FrontFlagsAntiMask;
			s_RollbackFlags |= m_FlagMask;

			result.Elements = (Byte*)(((UInt)s_Free + (Alignment - 1)) & ~(Alignment - 1));
			result.Count = size;

			s_Free = result.Elements + size;
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
			bool is_at_front = s_Free == ptr + Size;
			s_Free = (Byte*)((UInt)ptr * is_at_front + (UInt)s_Free * !is_at_front);
		}

		PileBase() :
			m_Begin(s_Free),
			m_End(s_Free),
			m_FlagMask(U64(1) << (U64(63) - s_Count)),
			m_FrontFlagsAntiMask(U64(-1) << (U64(63) - s_Count))
		{
			s_RollbackFlags |= m_FlagMask;
			++s_Count;

			StackCheck();
		};

		~PileBase()
		{
			--s_Count;
			bool rollback_flag = s_RollbackFlags & m_FlagMask;
			s_Free = (Byte*)((UInt)m_Begin * rollback_flag + (UInt)s_Free * !rollback_flag);
		}

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

	class Pile final : public PileBase
	{
	public:
		Pile() = default;
		~Pile() = default;

		template <typename T, UInt Count>
		Array<T, Count>* Allocate()
		{
			const bool at_front = m_End == s_Free;
			const bool rollback_flag = s_RollbackFlags & m_FlagMask;
			const bool reset_begin = !at_front && !rollback_flag;

			m_Begin = (Byte*)(((UInt)s_Free * reset_begin) + ((UInt)m_Begin * !reset_begin));

			s_RollbackFlags &= m_FrontFlagsAntiMask;
			s_RollbackFlags |= m_FlagMask;

			Array<T, Count>* result = (Array<T, Count>*)(((UInt)s_Free + (alignof(T) - 1)) & ~(alignof(T) - 1));

			s_Free = (Byte*)((UInt)result + sizeof(T) * Count);
			m_End = s_Free;

			FE_CORE_ASSERT(s_Free < (s_Buffer + s_BufferSize), "Out of memory in scrachpad!");

#ifdef FE_INTERNAL_BUILD
			bool new_max = s_Free > s_MaxFree;
			s_MaxFree = (Byte*)(((UInt)s_Free * new_max) + ((UInt)s_MaxFree * !new_max));
#endif // FE_INTERNAL_BUILD

			return result;
		}

		template <typename T>
		T* Allocate(UInt count)
		{
			const bool at_front = m_End == s_Free;
			const bool rollback_flag = s_RollbackFlags & m_FlagMask;
			const bool reset_begin = !at_front && !rollback_flag;

			m_Begin = (Byte*)(((UInt)s_Free * reset_begin) + ((UInt)m_Begin * !reset_begin));

			s_RollbackFlags &= m_FrontFlagsAntiMask;
			s_RollbackFlags |= m_FlagMask;

			T* result = (T*)(((UInt)s_Free + (alignof(T) - 1)) & ~(alignof(T) - 1));

			s_Free = (Byte*)((UInt)result + sizeof(T) * count);
			m_End = s_Free;

			FE_CORE_ASSERT(s_Free < (s_Buffer + s_BufferSize), "Out of memory in scrachpad!");

#ifdef FE_INTERNAL_BUILD
			bool new_max = s_Free > s_MaxFree;
			s_MaxFree = (Byte*)(((UInt)s_Free * new_max) + ((UInt)s_MaxFree * !new_max));
#endif // FE_INTERNAL_BUILD

			return result;
		}

		template <typename T>
		void Deallocate(T* ptr)
		{
			bool is_at_front = s_Free == ptr + sizeof(T);
			s_Free = (Byte*)((UInt)ptr * is_at_front + (UInt)s_Free * !is_at_front);
		}

		template <typename T, UInt Count>
		void Deallocate(Array<T, Count>* ptr)
		{
			bool is_at_front = s_Free == (Byte*)ptr + sizeof(T) * Count;
			s_Free = (Byte*)((UInt)ptr * is_at_front + (UInt)s_Free * !is_at_front);
		}

		template <typename T>
		void Deallocate(Splice<T> splice)
		{
			bool is_at_front = s_Free == (Byte*)splice.Elements + sizeof(T) * splice.Count;
			s_Free = (Byte*)((UInt)splice.Elements * is_at_front + (UInt)s_Free * !is_at_front);
		}
	};
}