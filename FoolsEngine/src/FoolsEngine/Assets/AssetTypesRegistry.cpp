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
		return nullptr;
	}

	const AssetTypesRegistry::Item* AssetTypesRegistry::GetItem(const std::pmr::string& extension)
	{
		for (const auto& item : s_Instance->m_Items)
		{
			if (item.MetaFileExtension == extension)
				return &item;
		}

		return nullptr;
	}

	template <typename tnAsset>
	void AssetTypesRegistry::RegisterAssetType()
	{
		m_Items.push_back(
			Item{
				&tnAsset::EmplaceCore,
				&tnAsset::LoadMetadata,
				&tnAsset::SaveMetadata,
				tnAsset::GetMetaFileExtension(),
				tnAsset::GetTypeStatic().ToConstCharPtr(),
				tnAsset::GetTypeStatic()
			}
		);
	}

#define _REGISTER_ASSET_DEF(x) template void AssetTypesRegistry::RegisterAssetType<x>();
	FE_FOR_EACH(_REGISTER_ASSET_DEF, FE_ASSET_TYPES_LIST);

	void AssetTypesRegistry::RegisterAssetTypes()
	{
#define _REGISTER_ASSET_TYPE_CALL(x) RegisterAssetType<x>();
		FE_FOR_EACH(_REGISTER_ASSET_TYPE_CALL, FE_ASSET_TYPES_LIST);
	}
}