#pragma once

#include <memory>

#ifdef FE_PLATFORM_WINDOWS
#else
	#error FoolsEngine does not support this platform!
#endif

#define PMR_STRING_TEMPLATE_PARAMS char, std::char_traits<char>, std::pmr::polymorphic_allocator<char>

namespace fe
{
	using U08 = uint8_t;
	using U16 = uint16_t;
	using U32 = uint32_t;
	using U64 = uint64_t;

	using S08 = int8_t;
	using S16 = int16_t;
	using S32 = int32_t;
	using S64 = int64_t;

	using F32 = float;
	using F64 = double;

	using UInt = uintptr_t;
	using SInt = intptr_t;

	using Byte = std::byte;

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