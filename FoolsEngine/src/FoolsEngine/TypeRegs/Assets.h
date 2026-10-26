#pragma once

#include "FoolsEngine/Assets/Asset.h"
#include "FoolsEngine/Assets/Loaders/LoaderType.h"
#include "FoolsEngine/Foundation/Containers/String.h"
#include "FoolsEngine/Foundation/Containers/Arena.h"


namespace YAML { class Emitter; }

namespace fe::TypeRegs
{
	class Assets
	{
	public:
		struct Item
		{
			void (* EmplaceCore)(AssetID);
			bool (* LoadMetadata)(AssetID);
			void (* SaveMetadata)(YAML::Emitter&, AssetID);
			String MetaFileExtension;
			AssetType Type;
		};

		static const Item* GetItem(AssetType assetType);
		static const Item* GetItem(String extension);
		
		static DynamicArena<Item> sItems;

		
		static void Init();
		
		template <typename tnAsset>
		static void RegisterAssetType();
		
		static void RegisterAssetTypes();
		
		static void Shutdown() {};
	};
}