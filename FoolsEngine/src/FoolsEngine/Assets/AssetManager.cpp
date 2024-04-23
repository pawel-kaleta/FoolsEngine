#include "FE_pch.h"

#include "AssetManager.h"

namespace fe
{
	AssetRegistry AssetManager::s_Registries[AssetTypesCount];

	std::unordered_map<UUID, AssetID> AssetManager::s_AssetMapByUUID[AssetTypesCount];

	std::vector<AssetSource> AssetManager::s_AssetSources;

	AssetManager::AssetSourceMaps AssetManager::s_AssetSourceMaps;
}