#include "FE_pch.h"
#include "AssetTypesRegistry.h"

#include "AssetTypes.h"

namespace fe
{
	AssetTypesRegistry* AssetTypesRegistry::s_Instance;

	const AssetTypesRegistry::Item* AssetTypesRegistry::GetItem(AssetType assetType)
	{
		for (const auto& item : s_Instance->m_Items)
		{
			if (item.Type == assetType)
				return &item;
		}
	}

	const AssetTypesRegistry::Item* AssetTypesRegistry::GetItem(const std::pmr::string extension)
	{
		for (const auto& item : s_Instance->m_Items)
		{
			if (item.MetaFileExtension == extension)
				return &item;
		}

		return nullptr;
	}

	void AssetTypesRegistry::RegisterAssetTypes()
	{
#define _REGISTER_ASSET_TYPE_CALL(x) RegisterAssetType<x>();
		FE_FOR_EACH(_REGISTER_ASSET_TYPE_CALL, FE_ASSET_TYPES_LIST);
	}
}