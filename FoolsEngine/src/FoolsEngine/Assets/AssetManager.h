#pragma once

#include "Asset.h"

#include <entt/entity/registry.hpp>

namespace fe
{
	struct AssetSource
	{
		std::filesystem::path FilePath;
		fe::UUID m_UUID;

		std::vector<AssetID> AssetIDs;
	};

	template <typename tnAsset>
	class AssetHandle;

	class AssetManager
	{
	public:
		static AssetRegistry* GetRegistry(AssetType type) { return &(Get().m_Registries[type]); }

		template <typename tnAsset>
		static AssetRegistry* GetRegistry() { return GetRegistry(tnAsset::GetTypeStatic()); }

		static auto GetAll(AssetType type) { return &(Get().m_Registries[type]).view<AssetID>(); }

		template <typename tnAsset>
		static auto GetAll() { return GetRegistry(tnAsset::GetTypeStatic())->view<AssetID>(); }

		template <typename tnAsset>
		static AssetID CreateAsset(const std::filesystem::path& filePath)
		{
			return CreateAsset(filePath, tnAsset::GetTypeStatic());
		}
		static AssetID CreateAsset(const std::filesystem::path& filePath, AssetType type);

		template <typename tnAsset>
		static AssetID GetAssetWithUUID(UUID uuid)
		{
			return GetAssetWithUUID(uuid, tnAsset::GetTypeStatic());
		}
		static AssetID GetAssetWithUUID(UUID uuid, AssetType type);

		template <typename tnAsset>
		static AssetID CreateAssetWithUUID(UUID uuid, const std::filesystem::path& filePath)
		{
			return CreateAssetWithUUID(uuid, filePath, tnAsset::GetTypeStatic());
		}
		static AssetID CreateAssetWithUUID(UUID uuid, const std::filesystem::path& filePath, AssetType type);
		
		static void EvaluateAndReload();
	private:
		friend class Application;
		AssetManager() { s_Instance = this; }
		void Shutdown() {};

		static AssetManager* s_Instance;
		static AssetManager& Get() { return *s_Instance; }

		AssetRegistry m_Registries[AssetType::Count];
		std::unordered_map<UUID, AssetID> m_AssetMapByUUID[AssetType::Count];
		std::unordered_map<std::filesystem::path, AssetID> m_AssetMapByPath[AssetType::Count];

		static std::vector<AssetSource> m_AssetSources;
		struct AssetSourceMaps
		{
			static std::unordered_map<UUID, AssetSourceID> ByUUID;
			static std::unordered_map<std::filesystem::path, AssetSourceID> ByFilepath;
		} m_AssetSourceMaps;

		template <typename tnAsset>
		static AssetID NewID() { return Get().m_Registries[tnAsset::GetTypeStatic()].create(); }
		static AssetID NewID(AssetType type) { return Get().m_Registries[type].create(); }

		template <typename tnAsset>
		static AssetID NewID(uint32_t requestID) { return Get().m_Registries[tnAsset::GetTypeStatic()].create(requestID); }
		static AssetID NewID(AssetType type, uint32_t requestID) { return Get().m_Registries[type].create(requestID); }

		friend class Renderer;
		template <typename tnAsset>
		static AssetID NewBaseAsset(AssetID requestID, const std::string& name)
		{
			return NewBaseAsset(requestID, name, tnAsset::GetTypeStatic());
		}
		static AssetID NewBaseAsset(AssetID requestID, const std::string& name, AssetType type);
	};
}