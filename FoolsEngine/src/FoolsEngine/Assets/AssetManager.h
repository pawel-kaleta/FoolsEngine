#pragma once

#include "Asset.h"

#include <entt/entity/registry.hpp>
#include <entt/entity/handle.hpp>
#include <entt/entity/helper.hpp>

namespace fe
{
	using AssetRegistry = entt::basic_registry<AssetID>;

	using ECS_AssetHandle = entt::basic_handle<AssetRegistry>;
	using Const_ECS_AssetHandle = entt::basic_handle<const AssetRegistry>;

	class AssetManager
	{
	public:
		template <typename tAssetBody>
		constexpr static AssetRegistry const* GetRegistry() { return &m_Registries[(int)tAssetBody::GetStaticType()]; }

	private:
		AssetManager();

		static AssetRegistry m_Registries[(int)AssetType::TypesCount];
		static std::unordered_map<UUID, AssetID> m_AssetMapByUUID[(int)AssetType::TypesCount];

		static std::vector<AssetSource> m_AssetSources;
		struct AssetSourceMaps
		{
			static std::unordered_map<UUID                 , AssetSourceID> ByUUID;
			static std::unordered_map<std::filesystem::path, AssetSourceID> ByFilepath;
		};

	};
}