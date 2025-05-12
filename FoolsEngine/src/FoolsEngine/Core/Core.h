#pragma once

#include <memory>

#ifdef FE_PLATFORM_WINDOWS
#else
	#error FoolsEngine does not support this platform!
#endif

#define BIT_FLAG(x) (1 << x)
#define WIDE_BIT_FLAG(x) ((uint64_t)1 << x)

#define PMR_STRING_TEMPLATE_PARAMS char, std::char_traits<char>, std::pmr::polymorphic_allocator<char>

namespace fe
{
	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;

	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}