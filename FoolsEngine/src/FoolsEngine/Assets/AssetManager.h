#pragma once

#include "Asset.h"

#include <entt/entity/registry.hpp>

namespace fe
{
	struct AssetSource
	{
		std::filesystem::path FilePath;
		std::vector< std::pair<AssetID, AssetType> > Assets;
	};

	template <typename tnAsset>
	class AssetHandle;

	class AssetManager
	{
	public:
		static AssetRegistry* GetRegistry(AssetType type) { return &(s_Instance->m_Registries[type]); }

		template <typename tnAsset>
		static AssetRegistry* GetRegistry() { return GetRegistry(tnAsset::GetTypeStatic()); }

		static auto GetAll(AssetType type) { return &(s_Instance->m_Registries[type]).view<AssetID>(); }

		template <typename tnAsset>
		static auto GetAll() { return GetRegistry(tnAsset::GetTypeStatic())->view<AssetID>(); }

		template <typename tnAsset>
		static AssetID CreateAsset(const std::filesystem::path& proxyFilePath)
		{
			return CreateAsset(proxyFilePath, tnAsset::GetTypeStatic());
		}
		static AssetID CreateAsset(const std::filesystem::path& proxyFilePath, AssetType type);

		template <typename tnAsset>
		static AssetID GetAssetWithUUID(UUID uuid)
		{
			return GetAssetWithUUID(uuid, tnAsset::GetTypeStatic());
		}
		static AssetID GetAssetWithUUID(UUID uuid, AssetType type);

		template <typename tnAsset>
		static AssetID GetOrCreateAsset(const std::filesystem::path& proxyFilePath)
		{
			return GetOrCreateAsset(proxyFilePath, tnAsset::GetTypeStatic());
		}
		static AssetID GetOrCreateAsset(const std::filesystem::path& proxyFilePath, AssetType type)
		{
			auto& map = s_Instance->m_AssetMaps[type].ByProxyPath;
			if (map.find(proxyFilePath) != map.end())
			{
				return map.at(proxyFilePath);
			}
			else
			{
				return CreateAsset(proxyFilePath, type);
			}
		}
		
		static void EvaluateAndReload();
	private:
		friend class Application;
		friend class Asset;

		AssetManager() { s_Instance = this; }
		void Shutdown() {};

		static AssetManager* s_Instance;

		AssetRegistry m_Registries[AssetType::Count];
		struct AssetMaps
		{
			std::unordered_map<UUID, AssetID> ByUUID;
			std::unordered_map<std::filesystem::path, AssetID> ByProxyPath;
		} m_AssetMaps[AssetType::Count];

		std::vector<AssetSource> m_AssetSources;
		struct AssetSourceMaps
		{
			std::unordered_map<UUID, uint32_t> ByUUID;
			std::unordered_map<std::filesystem::path, uint32_t> ByFilepath;
		} m_AssetSourceMaps;

		template <typename tnAsset>
		static AssetID NewID() { return s_Instance->m_Registries[tnAsset::GetTypeStatic()].create(); }
		static AssetID NewID(AssetType type) { return s_Instance->m_Registries[type].create(); }

		template <typename tnAsset>
		static AssetID NewID(uint32_t requestID) { return s_Instance->m_Registries[tnAsset::GetTypeStatic()].create(requestID); }
		static AssetID NewID(AssetType type, uint32_t requestID) { return s_Instance->m_Registries[type].create(requestID); }

		friend class Renderer;
		template <typename tnAsset>
		static AssetID NewBaseAsset(AssetID requestID, const std::string& name)
		{
			return NewBaseAsset(requestID, name, tnAsset::GetTypeStatic());
		}
		static AssetID NewBaseAsset(AssetID requestID, const std::string& name, AssetType type);
	};
}