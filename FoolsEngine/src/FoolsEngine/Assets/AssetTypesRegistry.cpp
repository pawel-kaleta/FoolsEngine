#include "AssetTypesRegistry.h"

#include "AssetTypes.h"

namespace fe
{
	DynamicArena<AssetTypesRegistry::Item> AssetTypesRegistry::sItems;

	const AssetTypesRegistry::Item* AssetTypesRegistry::GetItem(AssetType assetType)
	{
		for (const auto& item : sItems)
		{
			if (item.Type == assetType)
				return &item;
		}
		return nullptr;
	}

	const AssetTypesRegistry::Item* AssetTypesRegistry::GetItem(String extension)
	{
		for (const auto& item : sItems)
		{
			if (CompareStringsEqual(extension, item.MetaFileExtension))
				return &item;
		}

		return nullptr;
	}

	void AssetTypesRegistry::Init()
	{
		sItems.Init();
	}

	template <typename tnAsset>
	void AssetTypesRegistry::RegisterAssetType()
	{
		sItems.Append(
			Item{
				&tnAsset::EmplaceCore,
				nullptr,
				nullptr,
				tnAsset::GetMetaFileExtension(),
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