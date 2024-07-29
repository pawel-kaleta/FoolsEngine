#pragma once


#include "FoolsEngine\Core\Application.h"
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
		static AssetID NewAsset()
		{
			auto type = tnAsset::GetTypeStatic();
			auto newID = NewID(type);
			InitAsset(type, newID);
			return newID;
		}

		static AssetID NewAsset(AssetType type)
		{
			auto newID = NewID(type);
			InitAsset(type, newID);
			return newID;
		}

		static AssetID CreateOrGetAssetWithUUID(AssetType type, UUID uuid)
		{
			if (0 == uuid)
				return NullAssetID;
			
			AssetID id = TranslateID(type, uuid);

			if (id == NullAssetID)
				return CreateAssetWithUUID(type, uuid);
			
			return id;
		}

		static void ClearRegistries();
		static void EvaluateAndReload();
	private:
		friend class Application;
		AssetManager() = default;

		static AssetManager& Get() { return *(Application::Get().m_AssetManager); }

		AssetRegistry m_Registries[AssetType::Count];
		std::unordered_map<UUID, AssetID> m_AssetMapByUUID[AssetType::Count];

		static AssetID TranslateID(AssetType type, UUID uuid)
		{
			if (Get().m_AssetMapByUUID[type].find(uuid) == Get().m_AssetMapByUUID[type].end())
				return NullAssetID;

			return Get().m_AssetMapByUUID[type].at(uuid);
		}

		static AssetID CreateAssetWithUUID(AssetType type, UUID uuid);

		static std::vector<AssetSource> m_AssetSources;
		struct AssetSourceMaps
		{
			static std::unordered_map<UUID, AssetSourceID> ByUUID;
			static std::unordered_map<std::filesystem::path, AssetSourceID> ByFilepath;
		};
		static AssetSourceMaps m_AssetSourceMaps;
		
		static void InitAsset(AssetType type, AssetID assetID)
		{
			AssetRegistry& reg = Get().m_Registries[type];

			reg.emplace<ACUUID>(assetID);
			reg.emplace<ACRefsCounters>(assetID);
			reg.emplace<ACDataLocation>(assetID);
			reg.emplace<ACFilepath>(assetID);
		}

		static AssetID NewID(AssetType type) { return Get().m_Registries[type].create(); }
		static AssetID NewID(AssetType type, uint32_t requestID) { return Get().m_Registries[type].create(requestID); }

		friend class Renderer;
		template <typename tnAsset>
		static AssetID NewAsset(AssetID requestID)
		{
			auto type = tnAsset::GetTypeStatic();
			auto newID = NewID(type, requestID);
			InitAsset(type, newID);
			return newID;
		}

	};
}