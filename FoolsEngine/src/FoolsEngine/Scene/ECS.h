#pragma once
#include "FE_pch.h"
#include <entt.hpp>

namespace fe
{
	using SetID = uint32_t;
	inline constexpr entt::null_t NullSetID{};
	using Registry = entt::basic_registry<SetID>;
	inline constexpr SetID RootID{0};


	using ECS_handle = entt::basic_handle<Registry>;
	using const_ECS_handle = entt::basic_handle<const Registry>;

	// T,U - std::forward_as_tuple(list of references to storages)
	template<typename T, typename U>
	decltype(auto) ComponentsQuerry(T get, U exclude)
	{
		return entt::basic_view(get, exclude);
	}
}