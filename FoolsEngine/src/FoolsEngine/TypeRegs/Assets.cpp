#include "Assets.h"

#include "FoolsEngine/Assets/AssetTypesList.h"

namespace fe::TypeRegs
{
	DynamicArena<Assets::Item> Assets::sItems;

	const Assets::Item* Assets::GetItem(AssetType assetType)
	{
		for (const auto& item : sItems)
		{
			if (item.Type == assetType)
				return &item;
		}
		return nullptr;
	}

	const Assets::Item* Assets::GetItem(String extension)
	{
		for (const auto& item : sItems)
		{
			if (CompareStringsEqual(extension, item.MetaFileExtension))
				return &item;
		}

		return nullptr;
	}

	void Assets::Init()
	{
		sItems.Init();
	}

	template <typename tnAsset>
	void Assets::RegisterAssetType()
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

#define _REGISTER_ASSET_DEF(x) template void Assets::RegisterAssetType<x>();
	FE_FOR_EACH(_REGISTER_ASSET_DEF, FE_ASSET_TYPES_LIST);

	void Assets::RegisterAssetTypes()
	{
#define _REGISTER_ASSET_TYPE_CALL(x) RegisterAssetType<x>();
		FE_FOR_EACH(_REGISTER_ASSET_TYPE_CALL, FE_ASSET_TYPES_LIST);
	}
}