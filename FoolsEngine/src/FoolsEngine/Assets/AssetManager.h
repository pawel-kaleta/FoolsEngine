#pragma once

#include "Asset.h"

namespace fe
{
	template <typename tnAsset>
	class AssetHandle;

	class AssetManager
	{
	public:
		static AssetRegistry* GetRegistry(AssetType type) { return &(s_Instance->m_Data[type].Registry); }

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

		template <typename tnAsset>
		static auto GetAll() { return GetAll(tnAsset::GetTypeStatic()); }
		static auto GetAll(AssetType type) { return GetRegistry(type)->view<AssetID>(); }
		
		template <typename tnAsset>
		static AssetID GetAssetFromFilepath(const std::filesystem::path& filepath)
		{
			return GetAssetFromDataFilepath(filepath, tnAsset::GetTypeStatic());
		}
		static AssetID GetAssetFromFilepath(const std::filesystem::path& filepath, AssetType type);
		
		template <typename tnAsset>
		static AssetID GetAssetWithUUID(UUID uuid)
		{
			return GetAssetWithUUID(uuid, tnAsset::GetTypeStatic());
		}
		static AssetID GetAssetWithUUID(UUID uuid, AssetType type);

		static void EvaluateAndReload();
	private:
		friend class Application;
		AssetManager() { s_Instance = this; }
		void Shutdown() { };

		static AssetManager* s_Instance;
		struct Data
		{
			AssetRegistry Registry;
			std::unordered_map<std::filesystem::path, AssetID> MapByFilepath;
			std::unordered_map<UUID, AssetID> MapByUUID;
		} m_Data[AssetType::Count];

		friend class Asset;
		static void AddByFilepathMapEntry(const std::filesystem::path& path, AssetID ID, AssetType type)
		{
			s_Instance->m_Data[type].MapByFilepath[path] = ID;
		}
		static void RemoveByFilepathMapEntry(const std::filesystem::path& path, AssetType type)
		{
			s_Instance->m_Data[type].MapByFilepath.erase(path);
		}
	};
}