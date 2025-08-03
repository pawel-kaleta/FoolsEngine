#pragma once

#include "FoolsEngine/Debug/Asserts.h"

#include <memory_resource>

namespace fe
{
	class Scratchpad final : public std::pmr::memory_resource
	{
	public:
		Scratchpad() :
			m_Begin(s_Free),
			m_End(s_Free),
			m_FlagMask(uint64_t{1} << (63 - s_Count)),
			m_FrontFlagsAntiMask(uint64_t(-1) << (63 - s_Count))
		{
			s_RollbackFlags |= m_FlagMask;
			s_Count++;
		};
		Scratchpad(const Scratchpad&) = delete;
		Scratchpad& operator=(const Scratchpad& other) = delete;

		template<class U, class... CtorArgs>
		U* NewObject(CtorArgs&&... ctor_args)
		{
			return std::pmr::polymorphic_allocator(this).new_object<U>(std::forward<CtorArgs>(ctor_args)...);
		}

		template<class U>
		void DeleteObject(U* p)
		{
			p->~U();
			//std::pmr::polymorphic_allocator<U> alloc(this);
			//alloc.deallocate(p, 1); // just calls this->do_deallocate() that is no-op
		}

		virtual ~Scratchpad() final override
		{
			s_Count--;
			bool rollback_flag = s_RollbackFlags & m_FlagMask;
#pragma warning(disable : 6323)
			s_Free = (std::byte*)((uintptr_t)m_Begin * rollback_flag + (uintptr_t)s_Free * !rollback_flag);
#pragma warning(default : 6323)
		}
	private:
		constexpr const static size_t s_BufferSize = 524'288;
		static std::byte s_Buffer[s_BufferSize];
		static std::byte* s_Free;
		static uint32_t s_Count;
		static uint64_t s_RollbackFlags;
		// we don't use std::bitset, because it handles any number of bits
		// solution for specific size that fits in one non-array type is slightly faster
		// this is very hot code

		friend class Application;
		static void Shutdown() {};
		static void Init()
		{
			s_Free = s_Buffer;
			s_Count = 0;
		};

		std::byte* m_Begin;
		std::byte* m_End;
		const uint64_t m_FlagMask;
		const uint64_t m_FrontFlagsAntiMask;

		virtual void* do_allocate(std::size_t bytes, std::size_t alignment) final override 
		{
			s_RollbackFlags &= m_FrontFlagsAntiMask;

			const bool at_front = m_End == s_Free;
			const bool rollback_flag = s_RollbackFlags & m_FlagMask;
			const bool reset_begin = !at_front && !rollback_flag;
			
#pragma warning(disable : 6323)
			m_Begin = (std::byte*)(((uintptr_t)s_Free * reset_begin) + ((uintptr_t)m_Begin * !reset_begin));
#pragma warning(default : 6323)
			s_RollbackFlags |= m_FlagMask;

			std::byte* const aligned = (std::byte*)(((uintptr_t)s_Free + (alignment - 1)) & ~(alignment - 1));
			s_Free = aligned + bytes;
			m_End = s_Free;

			FE_CORE_ASSERT(s_Free < (s_Buffer + s_BufferSize), "Out of memory in scrachpad!");

			return aligned;
		};
		virtual void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) final override { };
		virtual bool do_is_equal(const std::pmr::memory_resource& other)  const noexcept final override { return this == &other; };
	};

#if 0
	{
		Scratchpad sp;
	
		auto vec = sp.NewObject<std::pmr::vector<std::pmr::string>>();
		vec->push_back("test");
		sp.DeleteObject(vec); // no need if no need for destructors, all gets released with death of sp
	
		std::pmr::vector<std::pmr::string> vec2(&sp);
		vec2.push_back("aaaa");
		// destructor called automatically when vec2 goes out of scope
	}
#endif
}