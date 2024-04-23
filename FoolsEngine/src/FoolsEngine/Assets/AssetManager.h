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

	constexpr int AssetTypesCount = (int)(AssetType::TypesCount);

	class AssetManager
	{
	public:
		AssetManager() = delete;
		
		template <typename tAssetBody>
		constexpr static AssetRegistry* GetRegistry() { return &(s_Registries[(int)(tAssetBody::GetStaticType())]); }

		template <typename tAssetBody>
		AssetID NewID() { AssetRegistry* x = AssetManager::GetRegistry<tAssetBody>(); x->create(); }
	private:
		static AssetRegistry s_Registries[AssetTypesCount];
		static std::unordered_map<UUID, AssetID> s_AssetMapByUUID[AssetTypesCount];

		static std::vector<AssetSource> s_AssetSources;
		struct AssetSourceMaps
		{
			static std::unordered_map<UUID, AssetSourceID> ByUUID;
			static std::unordered_map<std::filesystem::path, AssetSourceID> ByFilepath;
		};
		static AssetSourceMaps s_AssetSourceMaps;
	};
}