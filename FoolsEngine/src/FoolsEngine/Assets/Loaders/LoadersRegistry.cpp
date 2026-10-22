#include "LoadersRegistry.h"

#include "ShaderLoader.h"
#include "TextureLoader.h"
#include "GeometryLoader.h"

namespace fe
{
	DynamicArena<LoadersRegistry::Item> LoadersRegistry::sItems;

	const LoadersRegistry::Item* LoadersRegistry::GetItem(String extension)
	{
		for (const auto& item : sItems)
		{
			if ((*item.IsKnownExtensionFunkPtr)(extension))
				return &item;
		}
		return nullptr;
	}

	const LoadersRegistry::Item* LoadersRegistry::GetItem(AssetType assetType)
	{
		if (assetType == AssetType::None) return nullptr;

		for (const auto& item : sItems)
		{
			if (item.AssetType == AssetType::None)
				if ((*item.IsKnownAssetType)(assetType))
					return &item;
			if (item.AssetType == assetType)
				return &item;
		}

		return nullptr;
	}

	template <typename tnLoader>
	void LoadersRegistry::RegisterLoader()
	{
		sItems.Append(
			Item{
				&tnLoader::IsKnownExtension,
				&tnLoader::IsKnownAssetType,
				tnLoader::GetExtensionAlias(),
				tnLoader::GetTypeStatic(),
				tnLoader::GetAssetTypeStatic()
			}
		);
	}

	void LoadersRegistry::Init()
	{
		sItems.Init();
	}

	void LoadersRegistry::RegisterLoaders()
	{
		RegisterLoader<TextureLoader>();
		RegisterLoader<GeometryLoader>();
		RegisterLoader<ShaderLoader>();
	}
}