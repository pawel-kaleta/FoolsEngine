#pragma once
#include "FE_pch.h"
#include <entt.hpp>


namespace fe
{
	using SetID = uint32_t;
	inline constexpr entt::null_t NullSet{};
	using Registry = entt::basic_registry<SetID>;

	using ECS_handle = entt::basic_handle<Registry>;
	using const_ECS_handle = entt::basic_handle<const Registry>;

	// std::forward_as_tuple(list of pointers to storages)
	template<typename T, typename U>
	decltype(auto) ComponentsQuerry(T get, U exclude)
	{
		return entt::basic_view(get, exclude);
	}
/*
	auto view = entt::basic_view(
		std::forward_as_tuple(reg.storage<A>("Y"_hs), reg.storage<B>("X"_hs)), // has component in these storages
		std::forward_as_tuple(reg.storage<C>("X"_hs), reg.storage<D>("Z"_hs))  // does not have a component in these storages
	);
*/

}