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
			CString SourceExtensionAlias;
			LoaderType Type;
			// AssetType::None means multiple types? 
			AssetType AssetType;
		};

		DynamicArena<Item> m_Items;

		static const Item* GetItem(String extension);
		static const Item* GetItem(AssetType assetType);
	private:
		static LoadersRegistry* s_Instance;

		friend class Application;
		LoadersRegistry() { s_Instance = this; };
		void RegisterLoaders();
		void Shutdown() {};

		template <typename tnLoader>
		void RegisterLoader();
	};
}