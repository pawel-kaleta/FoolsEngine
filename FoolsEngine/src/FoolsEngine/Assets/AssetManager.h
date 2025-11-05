#pragma once

#include "Asset.h"
#include <tuple>

namespace fe
{
	template <typename tnAsset>
	class AssetHandle;

	class AssetManager
	{
	public:
		static AssetRegistry& GetRegistry() { return s_Instance->m_Registry; }
		static UUID GetUUID(AssetID assetID) { return s_Instance->m_Registry.get<ACUUID>(assetID).UUID; }

		struct AssetCreation
		{
#ifdef FE_EDITOR
			template <typename tnAsset>
			static AssetID EditorAsset();
#endif

			template <typename tnAsset>
			static AssetID BaseAsset(const std::filesystem::path& path, UUID uuid);

			template <typename tnAsset>
			static AssetID InternalAsset(AssetID master);

			template <typename tnAsset>
			static AssetID ProjectAsset(const std::filesystem::path& path);
		};

		static AssetID GetOrCreateAssetWithUUID(UUID uuid);
		static AssetID GetAssetFromFilepath(const std::filesystem::path& filepath);
		static const std::vector<AssetID>* GetAssetsFromSourceFilepath(const std::filesystem::path& filepath);
	
		static void SetFilepath(AssetID assetID, const std::filesystem::path& filepath);
		static void SetSourcePath(AssetID assetID, const std::filesystem::path& sourcePath);

		static auto GetAll() { return GetRegistry().view<AssetID>(); }
		static void EvaluateAndReload();

	private:
		friend class Application;
		//friend class decltype(AssetCreation);
		AssetManager();

		static AssetManager* s_Instance;

		AssetRegistry m_Registry;
		std::unordered_map<UUID, AssetID> m_MapByUUID;
		std::unordered_map<std::filesystem::path, AssetID> m_MapByFilepath;
		std::unordered_map<std::filesystem::path, std::vector<AssetID>> m_SourceFileRegistry;

		struct 
		{
			decltype(m_Registry.group<>(
				entt::get<ACLoadedCPU>,
				entt::exclude<
					ACLoadedAsDependenceCPU,
					ACLoadFlag<AssetLoadingPriority::Minimal>,
					ACLoadFlag<AssetLoadingPriority::VeryLow>,
					ACLoadFlag<AssetLoadingPriority::Low>,
					ACLoadFlag<AssetLoadingPriority::Standard>,
					ACLoadFlag<AssetLoadingPriority::High>,
					ACLoadFlag<AssetLoadingPriority::VeryHigh>,
					ACLoadFlag<AssetLoadingPriority::Critical>
				>
			)) Unload;

			decltype(m_Registry.group<>(
				entt::get<ACLoadFlag<AssetLoadingPriority::Minimal>>,
				entt::exclude<ACLoadedCPU>
			)) Minimal;

			decltype(m_Registry.group<>(
				entt::get<ACLoadFlag<AssetLoadingPriority::VeryLow>>,
				entt::exclude<ACLoadedCPU>
			)) VeryLow;

			decltype(m_Registry.group<>(
				entt::get<ACLoadFlag<AssetLoadingPriority::Low>>,
				entt::exclude<ACLoadedCPU>
			)) Low;

			decltype(m_Registry.group<>(
				entt::get<ACLoadFlag<AssetLoadingPriority::Standard>>,
				entt::exclude<ACLoadedCPU>
			)) Standard;

			decltype(m_Registry.group<>(
				entt::get<ACLoadFlag<AssetLoadingPriority::High>>,
				entt::exclude<ACLoadedCPU>
			)) High;

			decltype(m_Registry.group<>(
				entt::get<ACLoadFlag<AssetLoadingPriority::VeryHigh>>,
				entt::exclude<ACLoadedCPU>
			)) VeryHigh;

			decltype(m_Registry.group<>(
				entt::get<ACLoadFlag<AssetLoadingPriority::Critical>>,
				entt::exclude<ACLoadedCPU>
			)) Critical;

		} m_LoadingGroups;
	};
}