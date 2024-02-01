#pragma once

#include <memory>

#include <entt/entity/registry.hpp>
#include <entt/entity/handle.hpp>
#include <entt/entity/helper.hpp>

namespace fe
{
	using EntityID = uint32_t;
	inline constexpr entt::null_t NullEntityID{};
	using Registry = entt::basic_registry<EntityID>;
	inline constexpr EntityID RootID{0};

	using ECS_handle = entt::basic_handle<Registry>;
	using const_ECS_handle = entt::basic_handle<const Registry>;

	// T,U - std::forward_as_tuple(list of references to storages)
	template<typename T, typename U>
	decltype(auto) ComponentsQuerry(T get, U exclude)
	{
		return entt::basic_view(get, exclude);
	}
}