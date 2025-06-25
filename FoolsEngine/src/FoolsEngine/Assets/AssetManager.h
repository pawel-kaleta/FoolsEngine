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
				entt::get<ACLoadedFlag>,
				entt::exclude<
					ACLoadFlag<AssetLoadingPriority::Low>,
					ACLoadFlag<AssetLoadingPriority::Standard>,
					ACLoadFlag<AssetLoadingPriority::High>,
					ACLoadFlag<AssetLoadingPriority::Critical>
				>
			)) Unload;

			decltype(m_Registry.group<>(
				entt::get<ACLoadFlag<AssetLoadingPriority::Low>>,
				entt::exclude<ACLoadedFlag>
			)) Low;

			decltype(m_Registry.group<>(
				entt::get< ACLoadFlag<AssetLoadingPriority::Standard>>,
				entt::exclude<ACLoadedFlag>
			)) Standard;

			decltype(m_Registry.group<>(
				entt::get<ACLoadFlag<AssetLoadingPriority::High>>,
				entt::exclude<ACLoadedFlag>
			)) High;

			decltype(m_Registry.group<>(
				entt::get<ACLoadFlag<AssetLoadingPriority::Critical>>,
				entt::exclude<ACLoadedFlag>
			)) Critical;


		} m_LoadingGroups;

	};
}