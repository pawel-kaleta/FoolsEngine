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

#ifdef FE_EDITOR
		template <typename tnAsset>
		static AssetID CreateEditorAsset()
		{
			FE_PROFILER_FUNC();

			AssetRegistry& reg = s_Instance->m_Registry;
			AssetID assetID = reg.create();

			reg.emplace<ACAssetType>(assetID).Type = tnAsset::GetTypeStatic();
			reg.emplace<tnAsset::Core>(assetID).Init();

			return assetID;
		}
#endif
		template <typename tnAsset>
		static AssetID CreateBaseAsset(UUID uuid)
		{
			FE_PROFILER_FUNC();

			AssetRegistry& reg = s_Instance->m_Registry;
			AssetID assetID = reg.create();

			reg.emplace<ACUUID>(assetID).UUID = uuid;
			s_Instance->m_MapByUUID[uuid] = assetID;

			reg.emplace<ACAssetType>(assetID).Type = tnAsset::GetTypeStatic();
			reg.emplace<tnAsset::Core>(assetID).Init();

			return assetID;
		}
		template <typename tnAsset>
		static AssetID CreateInternalAsset(AssetID master)
		{
			FE_PROFILER_FUNC();

			AssetRegistry& reg = s_Instance->m_Registry;
			AssetID assetID = reg.create();

			auto uuid = reg.emplace<ACUUID>(assetID).UUID;
			s_Instance->m_MapByUUID[uuid] = assetID;

			reg.emplace<ACAssetType>(assetID).Type = tnAsset::GetTypeStatic();
			reg.emplace<ACMasterAsset>(assetID).Master = master;
			reg.emplace<tnAsset::Core>(assetID).Init();

			return assetID;
		}
		template <typename tnAsset>
		static AssetID CreateProjectAsset(const std::filesystem::path& path)
		{
			FE_PROFILER_FUNC();

			AssetRegistry& reg = s_Instance->m_Registry;
			AssetID assetID = reg.create();

			reg.emplace<ACAssetType>(assetID).Type = tnAsset::GetTypeStatic();
			reg.emplace<ACRefsCounters>(assetID);
			reg.emplace<ACFilepath>(assetID).Filepath = path;
			auto uuid = reg.emplace<ACUUID>(assetID).UUID;

			s_Instance->m_MapByUUID[uuid] = assetID;
			s_Instance->m_MapByFilepath[path] = assetID;

			reg.emplace<tnAsset::Core>(assetID).Init();

			return assetID;
		}

		static AssetID GetOrCreateAssetWithUUID(UUID uuid);
		static AssetID GetAssetFromFilepath(const std::filesystem::path& filepath);
		static const std::vector<AssetID>* GetAssetsFromSourceFilepath(const std::filesystem::path& filepath);
	
		static void SetFilepath(AssetID assetID, const std::filesystem::path& filepath);
		static void SetSourcePath(AssetID assetID, const std::filesystem::path& sourcePath);

		static auto GetAll() { return GetRegistry().view<AssetID>(); }
		static void EvaluateAndReload();
	private:
		friend class Application;
		AssetManager() { s_Instance = this; }
		void Shutdown() { };

		static AssetManager* s_Instance;

		AssetRegistry m_Registry;
		std::unordered_map<UUID, AssetID> m_MapByUUID;
		std::unordered_map<std::filesystem::path, AssetID> m_MapByFilepath;
		std::unordered_map<std::filesystem::path, std::vector<AssetID>> m_SourceFileRegistry;
	};
}