#include "Loaders.h"

#include "FoolsEngine/Assets/Loaders/ShaderLoader.h"
#include "FoolsEngine/Assets/Loaders/TextureLoader.h"
#include "FoolsEngine/Assets/Loaders/GeometryLoader.h"

namespace fe::TypeRegs
{
	DynamicArena<Loaders::Item> Loaders::sItems;

	const Loaders::Item* Loaders::GetItem(String extension)
	{
		for (const auto& item : sItems)
		{
			if ((*item.IsKnownExtensionFunkPtr)(extension))
				return &item;
		}
		return nullptr;
	}

	const Loaders::Item* Loaders::GetItem(AssetType assetType)
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
	void Loaders::RegisterLoader()
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

	void Loaders::Init()
	{
		sItems.Init();
	}

	void Loaders::RegisterLoaders()
	{
		RegisterLoader<TextureLoader>();
		RegisterLoader<GeometryLoader>();
		RegisterLoader<ShaderLoader>();
	}
}