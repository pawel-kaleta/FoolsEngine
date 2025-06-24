#include "FE_pch.h"
#include "AssetManager.h"
#include "AssetTypes.h"

namespace fe
{
#ifdef FE_EDITOR
	template <typename tnAsset>
	AssetID AssetManager::AssetCreation::EditorAsset()
	{
		FE_PROFILER_FUNC();

		AssetRegistry& reg = s_Instance->m_Registry;
		AssetID assetID = reg.create();

		reg.emplace<ACAssetType>(assetID).Type = tnAsset::GetTypeStatic();
		reg.emplace<tnAsset::Core>(assetID).Init();

		return assetID;
	}
#endif

#define _CREATE_EDITOR_ASSET_DEF(x) template AssetID AssetManager::AssetCreation::EditorAsset<x>();
	FE_FOR_EACH(_CREATE_EDITOR_ASSET_DEF, FE_ASSET_TYPES_LIST);


	template <typename tnAsset>
	static AssetID AssetManager::AssetCreation::BaseAsset(const std::filesystem::path& path, UUID uuid)
	{
		FE_PROFILER_FUNC();

		AssetRegistry& reg = s_Instance->m_Registry;
		AssetID assetID = reg.create();

		reg.emplace<ACAssetType>(assetID).Type = tnAsset::GetTypeStatic();
		reg.emplace<ACFilepath>(assetID).Filepath = path;
		reg.emplace<ACUUID>(assetID).UUID = uuid;
		s_Instance->m_MapByFilepath[path] = assetID;
		s_Instance->m_MapByUUID[uuid] = assetID;

		reg.emplace<tnAsset::Core>(assetID).Init();

		return assetID;
	}

#define _CREATE_BASE_ASSET_DEF(x) template AssetID AssetManager::AssetCreation::BaseAsset<x>(const std::filesystem::path& path, UUID uuid);
	FE_FOR_EACH(_CREATE_BASE_ASSET_DEF, FE_ASSET_TYPES_LIST);


	template <typename tnAsset>
	static AssetID AssetManager::AssetCreation::InternalAsset(AssetID master)
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

#define _CREATE_INTERNAL_ASSET_DEF(x) template AssetID AssetManager::AssetCreation::InternalAsset<x>(AssetID master);
	FE_FOR_EACH(_CREATE_INTERNAL_ASSET_DEF, FE_ASSET_TYPES_LIST);


	template <typename tnAsset>
	static AssetID AssetManager::AssetCreation::ProjectAsset(const std::filesystem::path& path)
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

#define _CREATE_PROJECT_ASSET_DEF(x) template AssetID AssetManager::AssetCreation::ProjectAsset<x>(const std::filesystem::path& path);
	FE_FOR_EACH(_CREATE_PROJECT_ASSET_DEF, FE_ASSET_TYPES_LIST);
}