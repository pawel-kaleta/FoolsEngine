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
			m_FlagMask(((uint64_t)1) << (63 - s_Count)),		// ???????
			m_FrontFlagsMask(((uint64_t)-1) >> (s_Count + 1))	// ???????????
		{
			s_RollbackFlags |= m_FlagMask;
			s_Count++;
		};
		Scratchpad(const Scratchpad&) = delete;
		Scratchpad& operator=(const Scratchpad& other) = delete;

		template<class U, class... CtorArgs>
		U* NewObject(CtorArgs&&... ctor_args)
		{
			U* p = (U*)do_allocate(sizeof(U), alignof(U));
			std::pmr::polymorphic_allocator<U> alloc(this);
			alloc.construct(p, std::forward<CtorArgs>(ctor_args)...);
			return p;
		}

		template<class U>
		void DeleteObject(U* p)
		{
			p->~U();
			//std::pmr::polymorphic_allocator<U> alloc(this);
			//alloc.deallocate(p, 1); // this->do_deallocate() is no-op
		}

		virtual ~Scratchpad() final override
		{
			s_Count--;
			bool rollback_flag = s_RollbackFlags & m_FlagMask;
			s_Free = (std::byte*)((uint64_t)m_Begin * rollback_flag + (uint64_t)s_Free * !rollback_flag);
		}
	private:
		constexpr const static size_t s_BufferSize = 524'288;
		static std::byte s_Buffer[s_BufferSize];
		static std::byte* s_Free;
		static uint64_t s_RollbackFlags;
		static uint32_t s_Count;

		friend class Application;
		static void Init()
		{
			s_Free = s_Buffer;
			s_Count = 0;
		};

		std::byte* m_Begin;
		std::byte* m_End;
		const uint64_t m_FlagMask;
		const uint64_t m_FrontFlagsMask;

		virtual void* do_allocate(std::size_t bytes, std::size_t alignment) final override 
		{
			s_RollbackFlags &= m_FrontFlagsMask;

			const bool at_front = m_End == s_Free;
			const bool rollback_flag = s_RollbackFlags & m_FlagMask;
			const bool reset_begin = !at_front && !rollback_flag;
			
			m_Begin = (std::byte*)(((uint64_t)s_Free * (reset_begin)) + ((uint64_t)m_Begin * !reset_begin));
			s_RollbackFlags |= m_FlagMask;

			std::byte* const aligned = (std::byte*)(((uint64_t)s_Free + (alignment - 1)) & ~(alignment - 1));
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