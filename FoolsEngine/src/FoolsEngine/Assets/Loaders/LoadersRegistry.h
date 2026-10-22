#pragma once

#include "LoaderType.h"

#include "FoolsEngine/Assets/AssetTypes.h"
#include "FoolsEngine/Foundation/Containers/String.h"

namespace fe
{
	class LoadersRegistry
	{
	public:
		struct Item
		{
			bool (* IsKnownExtensionFunkPtr)(String);
			bool (* IsKnownAssetType)(AssetType assetType);
			String SourceExtensionAlias;
			LoaderType Type;
			// AssetType::None means multiple types? 
			AssetType AssetType;
		};

		static DynamicArena<Item> sItems;

		static const Item* GetItem(String extension);
		static const Item* GetItem(AssetType assetType);


		static void Init();
		static void RegisterLoaders();
		static void Shutdown() {};

		template <typename tnLoader>
		static void RegisterLoader();
	};
}