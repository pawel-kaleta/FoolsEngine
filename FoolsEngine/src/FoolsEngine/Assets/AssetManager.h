#pragma once

#include "Asset.h"

namespace fe
{
	template <typename tnAsset>
	class AssetHandle;

	class AssetManager
	{
	public:
		static AssetRegistry& GetRegistry() { return s_Instance->m_Registry; }

		template <typename tnAsset>
		static AssetID CreateAsset(const std::filesystem::path& path)
		{
			FE_PROFILER_FUNC();

			Data& data = s_Instance->m_Data[tnAsset::GetTypeStatic()];

			AssetRegistry& reg = data.Registry;
			AssetID assetID = reg.create();

			reg.emplace<ACRefsCounters>(assetID);
			reg.emplace<ACAssetData>(assetID);
			auto uuid = reg.emplace<ACUUID>(assetID).UUID;

			data.MapByUUID[uuid] = assetID;
			data.MapByFilepath[path] = assetID;

			AssetUser<tnAsset>(ECS_handle(reg, assetID)).PlaceCoreComponents();
			return assetID;
		}
		template <typename tnAsset>
		static AssetID CreateCompositeAsset(AssetID parentAssetID, AssetType parentAssetType)
		{
			FE_PROFILER_FUNC();

			Data& data = s_Instance->m_Data[tnAsset::GetTypeStatic()];

			AssetRegistry& reg = data.Registry;
			AssetID assetID = reg.create();

			reg.emplace<ACAssetData>(assetID);

			auto& ac_parent_asset = reg.emplace<ACParentAsset>(assetID);
			ac_parent_asset.ParenAssetType = parentAssetType;
			ac_parent_asset.ParentAssetID = parentAssetID;

			AssetUser<tnAsset>(ECS_handle(reg, assetID)).PlaceCoreComponents();
			return assetID;
		}

		static auto GetAll() { return GetRegistry().view<AssetID>(); }
		
		static AssetID GetAssetFromFilepath(const std::filesystem::path& filepath);
		static AssetID GetAssetWithUUID(UUID uuid);

		static void EvaluateAndReload();
	private:
		friend class Application;
		AssetManager() { s_Instance = this; }
		void Shutdown() { };

		static AssetManager* s_Instance;

		AssetRegistry m_Registry;
		std::unordered_map<std::filesystem::path, AssetID> m_MapByFilepath;
		std::unordered_map<UUID, AssetID> m_MapByUUID;


		static void AddByFilepathMapEntry(const std::filesystem::path& path, AssetID ID)
		{
			s_Instance->m_MapByFilepath[path] = ID;
		}
		static void RemoveByFilepathMapEntry(const std::filesystem::path& path, AssetType type)
		{
			s_Instance->m_MapByFilepath.erase(path);
		}
	};

	Asset::Asset(AssetType type, AssetID assetID) :
		m_ECSHandle(ECS_AssetHandle(AssetManager::GetRegistry(), assetID))
	{ }
}