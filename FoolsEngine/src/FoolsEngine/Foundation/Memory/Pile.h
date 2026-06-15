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

		virtual Splice<Byte> AllocateRaw(UInt bytes) final override
		{
			Splice<Byte> result;

			const bool at_front = m_End == s_Free;
			const bool rollback_flag = s_RollbackFlags & m_FlagMask;
			const bool reset_begin = !at_front && !rollback_flag;

			m_Begin = (Byte*)(((UInt)s_Free * reset_begin) + ((UInt)m_Begin * !reset_begin));

			s_RollbackFlags &= m_FrontFlagsAntiMask;
			s_RollbackFlags |= m_FlagMask;

			result.Elements = AlignTo<8>(s_Free);
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
		virtual Splice<Byte> AllocateRaw(UInt bytes, UInt alignment) final override
		{
			Splice<Byte> result;

			const bool at_front = m_End == s_Free;
			const bool rollback_flag = s_RollbackFlags & m_FlagMask;
			const bool reset_begin = !at_front && !rollback_flag;

			m_Begin = (Byte*)(((UInt)s_Free * reset_begin) + ((UInt)m_Begin * !reset_begin));

			s_RollbackFlags &= m_FrontFlagsAntiMask;
			s_RollbackFlags |= m_FlagMask;

			result.Elements = AlignTo(s_Free, alignment);
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
		virtual void DeallocateRaw(Splice<Byte> memReg) final override
		{
			bool is_at_front = s_Free == memReg.Elements + memReg.Count;
			s_Free = (Byte*)((UInt)memReg.Elements * is_at_front + (UInt)s_Free * !is_at_front);
		}

		template <UInt Size, UInt Alignment>
		Splice<Byte> AllocateRaw()
		{
			Splice<Byte> result;

			const bool at_front = m_End == s_Free;
			const bool rollback_flag = s_RollbackFlags & m_FlagMask;
			const bool reset_begin = !at_front && !rollback_flag;

			m_Begin = (Byte*)(((UInt)s_Free * reset_begin) + ((UInt)m_Begin * !reset_begin));

			s_RollbackFlags &= m_FrontFlagsAntiMask;
			s_RollbackFlags |= m_FlagMask;

			result.Elements = AlignTo<Alignment>(s_Free);
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
		Splice<Byte> AllocateRaw(UInt size)
		{
			Splice<Byte> result;

			const bool at_front = m_End == s_Free;
			const bool rollback_flag = s_RollbackFlags & m_FlagMask;
			const bool reset_begin = !at_front && !rollback_flag;

			m_Begin = (Byte*)(((UInt)s_Free * reset_begin) + ((UInt)m_Begin * !reset_begin));

			s_RollbackFlags &= m_FrontFlagsAntiMask;
			s_RollbackFlags |= m_FlagMask;

			result.Elements = AlignTo<Alignment>(s_Free);
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
		void DeallocateRaw(Byte* ptr)
		{
			bool is_at_front = s_Free == ptr + Size;
			s_Free = (Byte*)((UInt)ptr * is_at_front + (UInt)s_Free * !is_at_front);
		}


		PileBase() :
			m_Begin(s_Free),
			m_End(s_Free),
			m_FlagMask(U64(1) << (UInt(63) - s_Count)),
			m_FrontFlagsAntiMask(U64(-1) << (UInt(63) - s_Count))
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

	using Pile = TypedAlloc<PileBase>;
}